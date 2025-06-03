/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlayAnimEntity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPlayAnimEntityDecl();
CKERROR CreatePlayAnimEntityProto(CKBehaviorPrototype **pproto);
int PlayAnimEntity(const CKBehaviorContext &behcontext);
int A_CurveInterpolation(float current_step, CKObjectAnimation *anim, CK2dCurve *curve, CKBehavior *beh);

CKObjectDeclaration *FillBehaviorPlayAnimEntityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Play Animation 3D Entity");
    od->SetDescription("Makes a 3D Entity perform an animation.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>One Loop Played: </SPAN>is activated once the animation has been performed.<BR>
    <BR>
    <SPAN CLASS=pin>Animation: </SPAN>animation to be performed.<BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the animation.<BR>
    <SPAN CLASS=pin>Loop: </SPAN>TRUE means the animation will be looped.<BR>
    <BR>
    <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.<BR>
    <BR>
    See Also: 'Animation Recorder', 'Play Global Animation', 'Set Animation Step on 3D entity'.<BR>
    */
    /* warning:
    - as there are no 'Hierarchy' input parameter, all the object's children will stay parented during the motion.
    Therefore you can't ask a object to move without moving its children.<BR>
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    */
    od->SetCategory("3D Transformations/Animation");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x64221225, 0x769a143f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlayAnimEntityProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePlayAnimEntityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Play Animation 3D Entity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("One Loop Played");

    proto->DeclareInParameter("Animation", CKPGUID_OBJECTANIMATION);
    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 5s 0ms");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Loop", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE);

    proto->DeclareLocalParameter(NULL, CKPGUID_PERCENTAGE); //"Current Step"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PlayAnimEntity);

    *pproto = proto;
    return CK_OK;
}

int PlayAnimEntity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetOwner();

    // Get Animation
    CKObjectAnimation *anim = (CKObjectAnimation *)beh->GetInputParameterObject(0);
    if (!anim)
        return CKBR_PARAMETERERROR;

    // Get Duration
    float duration = 5000.0f;

    // patch for intermediary version
    CKParameterIn *pin = beh->GetInputParameter(1);
    if (pin && (pin->GetGUID() == CKPGUID_INT))
    {
        int duration_int = 5000;
        pin->GetValue(&duration_int);
        duration = (float)duration_int;
    }
    else
    {
        beh->GetInputParameterValue(1, &duration);
    }

    if (duration < 0.001f)
        return CKBR_OK;

    // Get Curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(2, &curve);

    // Get Loop?
    CKBOOL loop = TRUE;
    beh->GetInputParameterValue(3, &loop);

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(4, &k);
    k = !k;

    // Get 'Local' Current Step
    // 'local' and 'output' current_step are equal
    // we just keep the 'local' on for compatibilities
    float current_step = 0;
    beh->GetLocalParameterValue(0, &current_step);

    if (beh->IsInputActive(1))
    { // if we enter by 'OFF' ...
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    current_step += behcontext.DeltaTime / duration;

    if (beh->IsInputActive(0))
    { // if we enter by 'ON' for the first time ...
        beh->ActivateInput(0, FALSE);
        current_step = 0.0f;
    }

    if (current_step > 1.0f)
    {

        beh->ActivateOutput(0);

        if (loop)
        {
            current_step -= (float)((int)current_step);
        }
        else
        {
            current_step = 1.0f;
            A_CurveInterpolation(current_step, anim, curve, beh);
            return CKBR_OK;
        }
    }

    A_CurveInterpolation(current_step, anim, curve, beh);

    return CKBR_ACTIVATENEXTFRAME;
}

//
//  Curve Interpolation
//
int A_CurveInterpolation(float current_step, CKObjectAnimation *anim, CK2dCurve *curve, CKBehavior *beh)
{
    beh->SetLocalParameterValue(0, &current_step);
    beh->SetOutputParameterValue(0, &current_step);

    current_step = curve->GetY(current_step);
    anim->SetStep(current_step);

    return 1;
}