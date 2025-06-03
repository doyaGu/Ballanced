/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetAsActiveCamera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAsActiveCameraDecl();
CKERROR CreateSetAsActiveCameraProto(CKBehaviorPrototype **pproto);
int SetAsActiveCamera(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAsActiveCameraDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set As Active Camera");
    od->SetDescription("Tells the camera to be the active one.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    This building block enables you to switch dynamically between cameras.<BR>
    See Also: 'Get Current Camera'.<BR>
    */
    od->SetCategory("Cameras/Montage");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x368f0ab1, 0x2d8957e4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAsActiveCameraProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateSetAsActiveCameraProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set As Active Camera");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetAsActiveCamera);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetAsActiveCamera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    if (behcontext.CurrentRenderContext)
        behcontext.CurrentRenderContext->AttachViewpointToCamera(cam);

    return CKBR_OK;
}
