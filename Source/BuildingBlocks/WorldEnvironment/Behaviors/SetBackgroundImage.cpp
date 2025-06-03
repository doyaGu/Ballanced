/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetBackgroundImage
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetBackgroundImageDecl();
CKERROR CreateSetBackgroundImageProto(CKBehaviorPrototype **);
int SetBackgroundImage(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBackgroundImageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Background Image");
    od->SetDescription("Places a background image.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Background Texture: </SPAN>texture to be used as the background image.<BR>
    <BR>
    */
    od->SetCategory("World Environments/Background");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1254d1ce, 0x001d0a58));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBackgroundImageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetBackgroundImageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Background Image");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Background Texture", CKPGUID_TEXTURE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetBackgroundImage);

    *pproto = proto;
    return CK_OK;
}

int SetBackgroundImage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set io states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get texture
    CKTexture *texture = (CKTexture *)beh->GetInputParameterObject(0);

    // Get material
    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    CKMaterial *mat = rcontext->GetBackgroundMaterial();
    mat->SetTexture0(texture);

    return CKBR_OK;
}
