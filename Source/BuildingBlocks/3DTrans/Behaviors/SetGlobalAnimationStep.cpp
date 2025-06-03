/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Set Global Animation Step
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetGlobalAnimationStepDecl();
CKERROR CreateSetGlobalAnimationStepProto(CKBehaviorPrototype **pproto);
int SetGlobalAnimationStep(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGlobalAnimationStepDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Global Animation Step");
    od->SetDescription("Sets the advancement in percentage of the global animation.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Animation : </SPAN>specify which animation should be used.<BR>
    <SPAN CLASS=pin>Step : </SPAN>advancement in percentage of the animation (between 0 and 100).<BR>
    <BR>
    You should use this building block to have an accurate control on the object's animation.
    See Also: 'Animation Recorder', 'Play Animation 3DEntity', 'Play Global Animation'.<BR>
    */
    /* warning:
    - as there are no 'Hierarchy' input parameter, all the object's children will stay parented during the motion.
    Therefore, you can't ask an object to move without moving its children.<BR>
    */
    od->SetCategory("3D Transformations/Animation");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7ba27c0a, 0x7bd21db6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGlobalAnimationStepProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGlobalAnimationStepProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Global Animation Step");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Animation", CKPGUID_ANIMATION, "0");
    proto->DeclareInParameter("Step", CKPGUID_PERCENTAGE, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGlobalAnimationStep);

    *pproto = proto;
    return CK_OK;
}

int SetGlobalAnimationStep(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Animation
    CKAnimation *anim = (CKAnimation *)beh->GetInputParameterObject(0);

    // Get step
    float step = 0;
    beh->GetInputParameterValue(1, &step);

    CKCharacter *carac = NULL;
    if (CKIsChildClassOf(anim, CKCID_KEYEDANIMATION))
    {
        CKKeyedAnimation *kanim = (CKKeyedAnimation *)anim;
        int i, count = kanim->GetAnimationCount();
        for (i = 0; i < count; ++i)
        {
            CKObjectAnimation *oanim = kanim->GetAnimation(i);
            CK3dEntity *ent = oanim->Get3dEntity();
            if (CKIsChildClassOf(ent, CKCID_BODYPART))
            {
                CKCharacter *c = ((CKBodyPart *)ent)->GetCharacter();
                if (c && carac != c)
                {
                    VxMatrix Identity;
                    Vx3DMatrixIdentity(Identity);
                    carac = c;
                    carac->SetWorldMatrix(Identity);
                }
            }
        }
    }

    if (anim)
        anim->SetStep(step);

    return CKBR_OK;
}
