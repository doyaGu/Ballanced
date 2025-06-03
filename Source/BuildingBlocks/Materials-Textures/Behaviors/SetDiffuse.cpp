/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Diffuse
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetDiffuseDecl();
CKERROR CreateSetDiffuseProto(CKBehaviorPrototype **);
int SetDiffuse(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetDiffuseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Diffuse");
    od->SetDescription("Sets the Diffuse Color of a Material");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Diffuse Color: </SPAN>the diffuse color.<BR>
    <SPAN CLASS=pin>Keep Alpha Component: </SPAN>specifies whether or not to keep the transparency of the material. (ie: whether or not the alpha field of the color will be considered.)<BR>
    <BR>
    */
    /* warning:
    - This building block won't have any effect on prelited meshes.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe1e1e1e1, 0x1e1e1e1e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSetDiffuseProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetDiffuseProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Diffuse");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Diffuse Color", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Keep Alpha Component", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetDiffuse);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetDiffuse(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    // Diffuse Color
    VxColor col(1.0f, 1.0f, 1.0f), col2;
    beh->GetInputParameterValue(0, &col);

    // Keep Tramsparency
    CKBOOL k;
    beh->GetInputParameterValue(1, &k);

    if (k)
    {
        col2 = mat->GetDiffuse();
        col.a = col2.a;
    }
    else
    {
        if (beh->GetVersion() < 0x00020000)
        {
            if (col.a < 1.0f)
            {
                mat->EnableAlphaBlend(TRUE);
                mat->SetSourceBlend(VXBLEND_SRCALPHA);
                mat->SetDestBlend(VXBLEND_INVSRCALPHA);
            }

            if (col.a == 1.0f)
            {
                mat->EnableAlphaBlend(FALSE);
            }
        }
    }

    mat->SetDiffuse(col);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
