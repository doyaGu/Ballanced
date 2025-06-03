/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          SetFOV Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetFOVDecl();
CKERROR CreateSetFOVProto(CKBehaviorPrototype **pproto);
int SetFOV(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFOVDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set FOV");
    od->SetDescription("Sets the Field Of View of the camera");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Angle: </SPAN>field of view angle, expressed in radians.<BR>
    <BR>
    See Also: 'Set Zoom'.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xaaaacccc, 0xaaaacccc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFOVProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    od->SetCategory("Cameras/Basic");
    return od;
}

CKERROR CreateSetFOVProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set FOV");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Angle", CKPGUID_ANGLE, "0:50");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFOV);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetFOV(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    float angle = 50.0f * PI / 180;
    beh->GetInputParameterValue(0, &angle);

    cam->SetFov(angle);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
