/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          SetZoom Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetZoomDecl();
CKERROR CreateSetZoomProto(CKBehaviorPrototype **pproto);
int SetZoom(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetZoomDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Zoom");
    od->SetDescription("Sets the Zoom (or Lens) of a perspective camera (millimeter).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Focal Length: </SPAN>lens size expressed in millimeters.
    <BR>
    See Also: 'Set FOV'.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x40540a0d, 0x000aaaaa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetZoomProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    od->SetCategory("Cameras/Basic");
    return od;
}

CKERROR CreateSetZoomProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Zoom");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Focal Length (mm)", CKPGUID_FLOAT, "50");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetZoom);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetZoom(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    float zoom = 50; // Zoom in mm
    beh->GetInputParameterValue(0, &zoom);

    float fov = 2.0f * atanf(18.0f / zoom);
    cam->SetFov(fov);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
