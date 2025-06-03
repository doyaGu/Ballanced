/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MakeTransparent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMakeTransparentDecl();
CKERROR CreateMakeTransparentProto(CKBehaviorPrototype **pproto);
int MakeTransparent(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMakeTransparentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Make Transparent");
    od->SetDescription("Makes a Render Object (3D or 2D Entity) Transparent.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Source Blend: </SPAN>see 'Set Blend Modes' for detailed information on blending factors.<BR>
    <SPAN CLASS=pin>Destination Blend: </SPAN>see 'Set Blend Modes' for detailed information on blending factors<BR>
    <SPAN CLASS=pin>Alpha: </SPAN>amount of opacity for the transparency.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>when checked, the behavior applies to the children in the hierarchy of the 3D Entity.<BR>
    <BR>
    "Make Transparent" BB simplifies the process of rendering an object with transparency (blending transparency).<BR>
    "Make Transparent" BB works also with prelit meshes.<BR>
    */
    /* warning:
    - Make Transparent BB render all materials applied to the 3d entity transparent, so other objects using the same material will also be
    rendered with transparency.<BR>
    - The material are set to be transparent, but the transparency flag is only put on the meshes of the selected 2D or 3D
    Entity. Therefore, other objects using the same material will also be rendered with transparency, but the transparency
    flag won't be put on these other meshes (You'd have some Z sorting problems...).
    If you want these other meshes to be rendered correctly, you should use on them the 'Set transparent' building block.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x16cc600c, 0x702836d1));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMakeTransparentProto);
    od->SetCompatibleClassId(CKCID_RENDEROBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateMakeTransparentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Make Transparent");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "Source Alpha");
    proto->DeclareInParameter("Destination Blend", CKPGUID_BLENDFACTOR, "Inverse Source Alpha");
    proto->DeclareInParameter("Alpha", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MakeTransparent);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}
void SetMaterialMode(CKMaterial *mat, int smode, int dmode, float alpha)
{
    mat->SetSourceBlend((VXBLEND_MODE)smode);
    mat->SetDestBlend((VXBLEND_MODE)dmode);
    if (dmode == VXBLEND_ZERO || ((alpha == 1.0f) && (dmode == VXBLEND_INVSRCALPHA) && (smode == VXBLEND_SRCALPHA)))
    {
        mat->EnableAlphaBlend(FALSE);
        mat->EnableZWrite(TRUE);
    }
    else
    {
        mat->EnableAlphaBlend(TRUE);
        mat->EnableZWrite(FALSE);
    }
    VxColor col = mat->GetDiffuse();
    col.a = alpha;
    mat->SetDiffuse(col);
}

void MakeTransparentFunc(CKRenderObject *ro, int smode, int dmode, float alpha)
{
    if (!ro)
        return;

    if (CKIsChildClassOf(ro, CKCID_2DENTITY))
    { // Character
        CKMaterial *mat = (CKMaterial *)((CK2dEntity *)ro)->GetMaterial();
        if (mat)
            SetMaterialMode(mat, smode, dmode, alpha);
    }
    else
    {
        if (CKIsChildClassOf(ro, CKCID_CHARACTER))
        { // Character
            CKCharacter *carac = (CKCharacter *)ro;
            for (int i = 0; i < carac->GetBodyPartCount(); i++)
            {
                MakeTransparentFunc(carac->GetBodyPart(i), smode, dmode, alpha);
            }
        }
        else
        {
            if (CKIsChildClassOf(ro, CKCID_SPRITE3D))
            { // Sprite 3D
                CKMaterial *mat = (CKMaterial *)((CKSprite3D *)ro)->GetMaterial();
                if (mat)
                    SetMaterialMode(mat, smode, dmode, alpha);
            }
            else
            { // 3D Entity
                CK3dEntity *ent = (CK3dEntity *)ro;
                VxColor col;
                CKDWORD dcol;
                for (int i = 0; i < ent->GetMeshCount(); i++)
                {
                    CKMesh *mesh = ent->GetMesh(i);
                    mesh->SetTransparent(TRUE);
                    if (mesh->GetLitMode() == VX_PRELITMESH)
                    {
                        for (int j = 0; j < mesh->GetVertexCount(); j++)
                        {
                            dcol = mesh->GetVertexColor(j);
                            dcol = ColorSetAlpha(dcol, (unsigned char)(255 * alpha));
                            mesh->SetVertexColor(j, dcol);
                        }
                    }

                    for (int j = 0; j < mesh->GetMaterialCount(); ++j)
                    {
                        CKMaterial *mat = mesh->GetMaterial(j);
                        if (mat)
                            SetMaterialMode(mat, smode, dmode, alpha);
                    }
                }
            }
        }
    }
}

void MakeTransparentHierarchicalFunc(CKRenderObject *ro, int smode, int dmode, float alpha)
{
    if (CKIsChildClassOf(ro, CKCID_3DENTITY))
    { // 3D
        CK3dEntity *ent = (CK3dEntity *)ro;
        for (int c = 0; c < ent->GetChildrenCount(); ++c)
        {
            MakeTransparentHierarchicalFunc(ent->GetChild(c), smode, dmode, alpha);
        }
        MakeTransparentFunc(ent, smode, dmode, alpha);
    }
    else
    { // 2D
        CK2dEntity *ent = (CK2dEntity *)ro;
        for (int c = 0; c < ent->GetChildrenCount(); ++c)
        {
            MakeTransparentHierarchicalFunc(ent->GetChild(c), smode, dmode, alpha);
        }
        MakeTransparentFunc(ent, smode, dmode, alpha);
    }
}

int MakeTransparent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    int smode, dmode;
    beh->GetInputParameterValue(0, &smode);
    beh->GetInputParameterValue(1, &dmode);

    float alpha;
    beh->GetInputParameterValue(2, &alpha);

    CKBOOL hierarchy = TRUE;
    beh->GetInputParameterValue(3, &hierarchy);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    if (hierarchy)
    {
        MakeTransparentHierarchicalFunc(ent, smode, dmode, alpha);
    }
    else
        MakeTransparentFunc(ent, smode, dmode, alpha);

    return CKBR_OK;
}
