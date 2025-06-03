/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		      SetCameraTarget for Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetCameraTargetDecl();
CKERROR CreateSetCameraTargetProto(CKBehaviorPrototype **pproto);
int SetCameraTarget(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetCameraTargetDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Camera Target");
    od->SetDescription("Makes a 3D Entity be the target of a camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Target: </SPAN>3D Entity to target.<BR>
    <BR>
    This building block doesn't need to be looped. (ie: the camera will constantly follow the selected object)<BR>
    <BR>
    */
    /* warning:
    - if you delete the camera, the target will be deleted.<BR>
    */
    od->SetCategory("Cameras/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x74123741, 0x4563ffff));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSetCameraTargetProto);
    od->SetCompatibleClassId(CKCID_TARGETCAMERA);
    return od;
}

CKERROR CreateSetCameraTargetProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Camera Target");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Target", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCameraTarget);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetCameraTarget(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTargetCamera *cam = (CKTargetCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    // Get target
    CK3dEntity *ent = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (beh->GetVersion() == 0x0001000)
    {
        VxVector pos(0, 0, 0);
        ent->GetPosition(&pos);
        CK3dEntity *target = cam->GetTarget();
        target->SetPosition(&pos);
        return CKBR_OK;
    }
    if (ent && (ent->GetClassID() == CKCID_3DENTITY))
    {
        cam->SetTarget(ent);
    }
    else
    {
        cam->SetTarget(NULL);
    }

    return CKBR_OK;
}
