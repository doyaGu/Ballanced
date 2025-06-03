/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Prelit Color
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetPrelitColorDecl();
CKERROR CreateSetPrelitColorProto(CKBehaviorPrototype **);
int SetPrelitColor(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPrelitColorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Prelit Color");
    od->SetDescription("Sets the color of a Prelit 3D Entity ( and Prelit the 3D Entity if it wasn't )");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Filtering Color: </SPAN>color by which the texture colors are multiplied... e.g.: to see red components of the texture, set filtering color to (255, 0, 0, 128)..<BR>
    Note that the alpha component is the fourth member of the filtering color
    <BR>
    */
    /* warning:
    - The "Set Prelit Color" building block destroys the 'normals' information of the vertices. (Because the vertex color of a Prelit mesh, and the normal vector on each vertices, overlap)<BR>
    - The "Set Prelit Color" building block can also be used to modify the color of a 3D Sprite.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x60415d44, 0x0d0174ea));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPrelitColorProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetPrelitColorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Prelit Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Filtering Color", CKPGUID_COLOR, "255, 255, 255, 128");
    proto->DeclareInParameter("Additional Color", CKPGUID_COLOR, "0, 0, 0, 0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPrelitColor);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetPrelitColor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // we change the color of the vertex
    VxColor col(1.0f, 1.0f, 1.0f, 0.5f);
    beh->GetInputParameterValue(0, &col); // get the filtering color
    CKDWORD dcol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    col.Set(0, 0, 0, 0);
    beh->GetInputParameterValue(1, &col); // get the additionnal color
    CKDWORD dspeccol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    // Is it a 3D Sprite ?
    if (CKIsChildClassOf(ent, CKCID_SPRITE3D))
    {
        CKMaterial *mat = ((CKSprite3D *)ent)->GetMaterial();
        mat->SetDiffuse(VxColor(dcol));
        mat->SetSpecular(VxColor(dspeccol));
    }
    else
    {
        CKMesh *mesh = ent->GetCurrentMesh();
        if (!mesh)
            return CKBR_OWNERERROR;

        mesh->SetLitMode(VX_PRELITMESH); // set to prelit mode

        for (int a = 0; a < mesh->GetVertexCount(); a++)
        {
            mesh->SetVertexColor(a, dcol);			   // the vertex color is multiplied by the texture
            mesh->SetVertexSpecularColor(a, dspeccol); // the spec. is added to the global color
        }
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
