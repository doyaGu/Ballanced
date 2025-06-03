/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          SetClipping Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetClippingDecl();
CKERROR CreateSetClippingProto(CKBehaviorPrototype **pproto);
int SetClipping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetClippingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Clipping Planes");
    od->SetDescription("Sets the near and the far clipping plane of a camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Near Clip: </SPAN>the distance from the point of view of the Near clipping plane.<BR>
    <SPAN CLASS=pin>Far Clip: </SPAN>the distance from the point of view of the Far clipping plane.<BR>
    <BR>
    Reducing the <SPAN CLASS=pin>Far Clip</SPAN> will speed up rendering, as objects which are too far away and so non-visible will be removed from rendering process. A fog background can be added so that objects on the perimeter edge will not be clipped.<BR>
    Reducing the <SPAN CLASS=pin>Near Clip</SPAN> will avoid clipping objects that are too close from being clipped if the camera gets too close to an object resulting in the object clipping.<BR>
    Increasing the <SPAN CLASS=pin>Near Clip</SPAN> and reducing the <SPAN CLASS=pin>Far Clip</SPAN> will help to correct Z-Buffer inaccuracy in 3d scenes modeled with objects that are too large.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x652316a2, 0x01aa09a9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetClippingProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    od->SetCategory("Cameras/Basic");
    return od;
}

CKERROR CreateSetClippingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Clipping Planes");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Near Clip", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Far Clip", CKPGUID_FLOAT, "200");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetClipping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetClipping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    float near_plane = 0.1f;
    beh->GetInputParameterValue(0, &near_plane);

    float far_plane = 200.0f;
    beh->GetInputParameterValue(1, &far_plane);

    cam->SetFrontPlane(near_plane);
    cam->SetBackPlane(far_plane);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
