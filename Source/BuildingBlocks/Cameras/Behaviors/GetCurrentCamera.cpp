/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetCurrentCamera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetCurrentCameraDecl();
CKERROR CreateGetCurrentCameraProto(CKBehaviorPrototype **pproto);
int GetCurrentCamera(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurrentCameraDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Current Camera");
    od->SetDescription("States the current camera in the active scene.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Current Camera: </SPAN>the camera currently used in the main viewport.<BR>
    <BR>
    See Also: 'Set As Active Camera'.<BR>
    */
    od->SetCategory("Cameras/Montage");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x368f22b1, 0x222957e4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurrentCameraProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetCurrentCameraProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Current Camera");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Current Camera", CKPGUID_CAMERA);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurrentCamera);

    *pproto = proto;
    return CK_OK;
}

int GetCurrentCamera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCamera *cam = behcontext.CurrentRenderContext->GetAttachedCamera();

    beh->SetOutputParameterObject(0, cam);

    return CKBR_OK;
}
