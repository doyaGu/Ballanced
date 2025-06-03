/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Set Animation Step Entity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAnimationStepEntityDecl();
CKERROR CreateSetAnimationStepEntityProto(CKBehaviorPrototype **pproto);
int SetAnimationStepEntity(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAnimationStepEntityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Animation Step Entity");
    od->SetDescription("Sets the advancement in percentage of the character's animation.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Animation: </SPAN>specify which animation should be used.<BR>
    <SPAN CLASS=pin>Step: </SPAN>advancement in percentage of the animation (between 0 and 100).<BR>
    <BR>
    You should use this building block to have an accurate control on the object's animation.
    See Also: 'Animation Recorder', 'Play Animation 3DEntity', 'Play Global Animation'.<BR>
    */
    /* warning:
    - as there are no 'Hierarchy' input parameter, all the object's children will stay parented during the motion.
    Therefore you can't ask a object to move without moving its children.<BR>
    */
    od->SetCategory("3D Transformations/Animation");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x14782b1d, 0x52692e21));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAnimationStepEntityProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetAnimationStepEntityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Animation Step Entity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Animation", CKPGUID_OBJECTANIMATION, "0");
    proto->DeclareInParameter("Step", CKPGUID_PERCENTAGE, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetAnimationStepEntity);

    *pproto = proto;
    return CK_OK;
}

int SetAnimationStepEntity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Animation
    CKObjectAnimation *anim = (CKObjectAnimation *)beh->GetInputParameterObject(0);
    if (!anim)
        return CKBR_OK;

    // Get step
    float step = 0;
    beh->GetInputParameterValue(1, &step);

    anim->SetStep(step);

    return CKBR_OK;
}
