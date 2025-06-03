/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PathFollow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPathFollowDecl();
CKERROR CreatePathFollowProto(CKBehaviorPrototype **pproto);
int PathFollow(const CKBehaviorContext &behcontext);
int PathFollowTime(const CKBehaviorContext &behcontext);
int PathFollowTimeOld(const CKBehaviorContext &behcontext);
CKERROR PathFollowCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPathFollowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Curve Follow");
    od->SetDescription("Moves a 3D Entity along a path.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Curve to cover: </SPAN>3D Curve to follow.<BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the 3D Entity along its movement.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.
  Useful if you need to interpolates at the same time (in the same loop) a color vlaue, a vector, a orientation or something like this.<BR>
    <SPAN CLASS=pout>Value: </SPAN>current Bezier-interpolated value.<BR>
  <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame Rate dependant. Making this building block Time dependant has the advantage that compositions will run at the same rate on all computer configurations.<BR>
    <BR>
  See Also: 'Position On Curve'.<BR>
    */
    /* warning:
    - If your not in Time Based mode, changing the Velocity during loop will have no effect.<BR>
  - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
  The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
  This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
  - also Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
  (for the 'Progression' output parameter)<BR>
  */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00ed010b, 0x00fd010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreatePathFollowProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePathFollowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Curve Follow");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Curve to cover", CKPGUID_CURVE);
    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 3s 0ms");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");
    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); //"stepsRemaining"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);    //"curveLastX"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);    //"curveDeltaX"

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PathFollowTime);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PathFollowCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR PathFollowCallBack(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = FALSE;
        beh->GetLocalParameterValue(3, &time);

        CKParameterIn *pin = beh->GetInputParameter(1);
        CKGUID currentGuid = pin->GetGUID();
        CKBOOL isOldVersion = (beh->GetVersion() < 0x00010005);

        // Time based or not ?
        if (time)
        {
            if (isOldVersion && (currentGuid != CKPGUID_TIME))
            {
                beh->SetFunction(PathFollowTimeOld);
                pin->SetGUID(CKPGUID_INT, TRUE, "Duration (ms)");
            }
            else
            {
                beh->SetFunction(PathFollowTime);
                pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
                if (isOldVersion)
                    beh->SetVersion(0x00010005);
            }
        }
        else
        {
            beh->SetFunction(PathFollow);
            pin->SetGUID(CKPGUID_FLOAT, TRUE, "Velocity");
        }
        /*
        if(time) {
            beh->SetFunction(PathFollowTime);
    pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
        } else {
            beh->SetFunction(PathFollow);
    pin->SetGUID(CKPGUID_FLOAT, TRUE, "Velocity");
  }
        */
    }
    }

    return CKBR_OK;
}

/*******************************************************/
/*       Path Follow (TIME Based new version (float))  */
/*******************************************************/
int PathFollowTime(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the  entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // getting the 3d curve
    CKCurve *path = (CKCurve *)beh->GetInputParameterObject(0);
    if (!path)
        return CKBR_PARAMETERERROR;

    // Get the total time
    float time = 1000;
    beh->GetInputParameterValue(1, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(2, &curve);

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3, &k);
    k = !k;

    // Local Variables
    float elapsedTime, value;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0; // start elapsed time
        beh->SetLocalParameterValue(0, &elapsedTime);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);
        beh->GetLocalParameterValue(0, &elapsedTime);
    }

    elapsedTime += behcontext.DeltaTime;

    float progression = elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;
    value = curve->GetY(progression);

    // output Progression
    beh->SetOutputParameterValue(0, &progression);

    // output Value
    beh->SetOutputParameterValue(1, &value);

    //____________________________ Motion
    VxVector trans;
    path->GetLocalPos(value, &trans);

    ent->SetPosition(&trans, path, k);

    //____________________________ End ?
    if (elapsedTime > time)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    //____________________________ or Loop ?
    beh->ActivateOutput(1);

    // memorize elapsedTime
    beh->SetLocalParameterValue(0, &elapsedTime);

    return CKBR_OK;
}

/*******************************************************/
/*      Path Follow (TIME Based old version (int))     */
/*******************************************************/
int PathFollowTimeOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the  entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // getting the 3d curve
    CKCurve *path = (CKCurve *)beh->GetInputParameterObject(0);
    if (!path)
        return CKBR_PARAMETERERROR;

    // Get the total time
    int time = 1000;
    beh->GetInputParameterValue(1, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(2, &curve);

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3, &k);
    k = !k;

    // Local Variables
    int elapsedTime;
    float value;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0; // start elapsed time
        beh->SetLocalParameterValue(0, &elapsedTime);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);
        beh->GetLocalParameterValue(0, &elapsedTime);
    }

    elapsedTime += (int)behcontext.DeltaTime;

    float progression = (float)elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;
    value = curve->GetY(progression);

    // output Progression
    beh->SetOutputParameterValue(0, &progression);

    // output Value
    beh->SetOutputParameterValue(1, &value);

    //____________________________ Motion
    VxVector trans;
    path->GetLocalPos(value, &trans);

    ent->SetPosition(&trans, path, k);

    //____________________________ End ?
    if (elapsedTime > time)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    //____________________________ or Loop ?
    beh->ActivateOutput(1);

    // memorize elapsedTime
    beh->SetLocalParameterValue(0, &elapsedTime);

    return CKBR_OK;
}

/*******************************************************/
/*               Path Follow (FPS)                     */
/*******************************************************/
int PathFollow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the  entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set Input states
    if (beh->IsInputActive(0))
    {
        // Input : In
        beh->ActivateInput(0, FALSE);

        // we get the number of frame the curve following must last (input parameter)
        float nbframes = 10.0f;
        beh->GetInputParameterValue(1, &nbframes);

        int stepsRemaining = 0;
        float curveDeltaX = 0.0f;

        if (nbframes <= 0)
            return CKBR_PARAMETERERROR;

        // we calculate the steps remaining
        stepsRemaining = (int)nbframes - 1;
        // we calculate the curveDeltaX
        curveDeltaX = 1.0f / stepsRemaining;

        ///////////////////////////////
        // writing the local parameters

        // writing the stepsRemaining
        beh->SetLocalParameterValue(0, &stepsRemaining);

        // writing the curveLastX
        float clx = 0.0f;
        beh->SetLocalParameterValue(1, &clx);

        // writing the curveDeltaX
        beh->SetLocalParameterValue(2, &curveDeltaX);

        // getting the 3d curve
        CKCurve *path = (CKCurve *)beh->GetInputParameterObject(0);
        if (!path)
            return CKBR_PARAMETERERROR;

        // hierarchy
        CKBOOL k = TRUE;
        beh->GetInputParameterValue(3, &k);
        k = !k;

        // Init positionning
        VxVector trans;
        path->GetLocalPos(0.0f, &trans);
        ent->SetPosition(&trans, path, k);

        // Set IO states
        beh->ActivateOutput(1);

        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(1))
        {
            // Input : Loop In
            beh->ActivateInput(1, FALSE);

            // getting the 3d curve
            CKCurve *path = (CKCurve *)beh->GetInputParameterObject(0);
            if (!path)
                return CKBR_PARAMETERERROR;

            // getting the 2d curve
            CK2dCurve *accelerationCurve = NULL;
            beh->GetInputParameterValue(2, &accelerationCurve);
            if (!accelerationCurve)
                return CKBR_PARAMETERERROR;

            ///////////////////////////////
            // getting the local parameters

            // getting the stepsRemaining
            int stepsRemaining;
            beh->GetLocalParameterValue(0, &stepsRemaining);

            // getting the curveLastX
            float curveLastX;
            beh->GetLocalParameterValue(1, &curveLastX);

            // getting the curveDeltaX
            float curveDeltaX;
            beh->GetLocalParameterValue(2, &curveDeltaX);

            // calculating the deltaY of the curve
            float progression = curveLastX + curveDeltaX;
            float value = accelerationCurve->GetY(progression);

            // output Progression
            beh->SetOutputParameterValue(0, &progression);

            // output Value
            beh->SetOutputParameterValue(1, &value);

            // hierarchy
            CKBOOL k = TRUE;
            beh->GetInputParameterValue(3, &k);
            k = !k;

            // positionning
            VxVector trans;
            path->GetLocalPos(value, &trans);
            ent->SetPosition(&trans, path, k);

            ///////////////////////////////
            // writing the local parameters

            // writing the stepsRemaining
            stepsRemaining--;
            beh->SetLocalParameterValue(0, &stepsRemaining);

            // writing the curveLastX
            curveLastX += curveDeltaX;
            beh->SetLocalParameterValue(1, &curveLastX);

            if (stepsRemaining > 0)
            {
                // PathFollow not finished
                beh->ActivateOutput(1);

                return CKBR_OK;
            }
            else
            {
                // loop accomplished
                beh->ActivateOutput(0);

                return CKBR_OK;
            }
        }
    }

    return CKBR_OK;
}
