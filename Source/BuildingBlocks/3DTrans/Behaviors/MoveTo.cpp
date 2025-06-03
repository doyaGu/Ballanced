/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MoveTo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMoveToDecl();
CKERROR CreateMoveToProto(CKBehaviorPrototype **pproto);
int MoveTo(const CKBehaviorContext &behcontext);
int MoveToTime(const CKBehaviorContext &behcontext);
int MoveToTimeOld(const CKBehaviorContext &behcontext);
CKERROR MoveToCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMoveToDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Move To");
    od->SetDescription("Makes a 3D Entity move to a specified position.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out:</SPAN>is activated when the process needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Destination Point: </SPAN>3D position expressed in 'Destination Referential' co-ordinate system.<BR>
    <SPAN CLASS=pin>Destination Referential: </SPAN>coordinate system in which the position is expressed.<BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the 3D Entity along its movement.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
        <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.
    Useful if you need to interpolates at the same time (in the same loop) a color vlaue, a vector, a orientation or something like this.<BR>
        <SPAN CLASS=pout>Value: </SPAN>current Bezier-interpolated value.<BR>
    <BR>
        <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame
        Rate dependant. Making this building block Time dependant has the advantage that compositions
        will run at the same rate on all computer configurations.<BR>
    Tip: Since the 'Progression' value is given as an output parameter, you can use it in the loop to interpolate the orientation of the object at the same time (with the 'Interpolator Orientation' building block for example).<BR>
    */
        /* warning:
        - This building block has been designed to work with 3D Entities only. See 'Character Go To' for character displacement.<BR>
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    - When you use the 'Time Based' version of this building block, you can move the 'Destination Referential' and the 3DEntity will follow it (it is a new functionnality; specific to the 'Time Based' version).
    But as the 'Not Time Based' version is older than the 'Time Based' version (and therefore has to do the same thing as before), if you don't use 'Time Based' you'll not have the ability to move the 'Destination Referential' and make the 3DEntity understand you want it to follow the 'Destination Referential'.<BR>
    - also Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
    (for the 'Progression' output parameter)<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x010d010b, 0x011d010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateMoveToProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("3D Transformations/Movement");
    return od;
}

CKERROR CreateMoveToProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Move To");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Destination Point", CKPGUID_VECTOR);
    proto->DeclareInParameter("Destination Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Duration (ms)", CKPGUID_TIME, "0m 3s 0ms");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");
    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, 0); // stepsRemaining
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);  //"curveLastX"
    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT);  //"curveDeltaX"
    proto->DeclareLocalParameter(NULL, CKPGUID_VECTOR); //"goingVector"
    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MoveToTime);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(MoveToCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR MoveToCallBack(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = FALSE;
        beh->GetLocalParameterValue(4, &time);

        // pin offset from older to newer version
        CKParameterIn *pin = beh->GetInputParameter(1);
        int pindec = (pin->GetGUID() == CKPGUID_3DENTITY) ? 1 : 0;
        pin = beh->GetInputParameter(1 + pindec);

        CKGUID currentGuid = pin->GetGUID();
        CKBOOL isOldVersion = (beh->GetVersion() < 0x00010005);

        // Time based or not ?
        if (time)
        {
            if (isOldVersion && (currentGuid != CKPGUID_TIME))
            {
                beh->SetFunction(MoveToTimeOld);
                pin->SetGUID(CKPGUID_INT, TRUE, "Duration (ms)");
            }
            else
            {
                beh->SetFunction(MoveToTime);
                pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
                if (isOldVersion)
                    beh->SetVersion(0x00010005);
            }
        }
        else
        {
            beh->SetFunction(MoveTo);
            pin->SetGUID(CKPGUID_INT, TRUE, "Number of Frame");
        }
    }
    break;
    }

    return CKBR_OK;
}

/*******************************************************/
/*       Move To (TIME Based new version (float))      */
/*******************************************************/
int MoveToTime(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // getting the destination point
    VxVector despt, pos_init;
    beh->GetInputParameterValue(0, &pos_init);

    // Get the destination referential
    CK3dEntity *destref = NULL;
    CKParameterIn *pin = beh->GetInputParameter(1);
    int pindec = 0;
    if (pin->GetGUID() == CKPGUID_3DENTITY)
    {
        pindec = 1;
        destref = (CK3dEntity *)beh->GetInputParameterObject(1);
    }

    // Transform dest point position to world
    if (destref)
    {
        if (pos_init == VxVector::axis0())
        {
            destref->GetPosition(&despt);
        }
        else
        {
            destref->Transform(&despt, &pos_init);
        }
    }
    else
    {
        despt = pos_init;
    }

    // Get the total time
    float time = 1000;
    beh->GetInputParameterValue(1 + pindec, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(2 + pindec, &curve);

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3 + pindec, &k);
    k = !k;

    // Local Variables
    float elapsedTime, value;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0; // start elapsed time
        beh->SetLocalParameterValue(0, &elapsedTime);

        // memorize initial position
        ent->GetPosition(&pos_init);
        beh->SetLocalParameterValue(3, &pos_init);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);
        beh->GetLocalParameterValue(0, &elapsedTime);
    }

    elapsedTime += behcontext.DeltaTime;

    // getting the initial point
    beh->GetLocalParameterValue(3, &pos_init);

    float progression = elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;
    value = curve->GetY(progression);

    //____________________________ Motion
    despt.x = pos_init.x + (despt.x - pos_init.x) * value;
    despt.y = pos_init.y + (despt.y - pos_init.y) * value;
    despt.z = pos_init.z + (despt.z - pos_init.z) * value;

    ent->SetPosition(&despt, NULL, k);

    //____________________________ End ?
    if (elapsedTime > time)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    //____________________________ or Loop ?
    beh->ActivateOutput(1, TRUE);

    // memorize elapsedTime
    beh->SetLocalParameterValue(0, &elapsedTime);

    // output Progression
    beh->SetOutputParameterValue(0, &progression);

    // output Value
    beh->SetOutputParameterValue(1, &value);

    return CKBR_OK;
}

/*******************************************************/
/*        Move To (TIME Based old version (int))       */
/*******************************************************/
int MoveToTimeOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // getting the destination point
    VxVector despt, pos_init;
    beh->GetInputParameterValue(0, &pos_init);

    // Get the destination referential
    CK3dEntity *destref = NULL;
    CKParameterIn *pin = beh->GetInputParameter(1);
    int pindec = 0;
    if (pin->GetGUID() == CKPGUID_3DENTITY)
    {
        pindec = 1;
        destref = (CK3dEntity *)beh->GetInputParameterObject(1);
    }

    // Transform dest point position to world
    if (destref)
    {
        if (pos_init == VxVector::axis0())
        {
            destref->GetPosition(&despt);
        }
        else
        {
            destref->Transform(&despt, &pos_init);
        }
    }
    else
    {
        despt = pos_init;
    }

    // Get the total time
    int time = 1000;
    beh->GetInputParameterValue(1 + pindec, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(2 + pindec, &curve);

    // hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3 + pindec, &k);
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

        // memorize initial position
        ent->GetPosition(&pos_init);
        beh->SetLocalParameterValue(3, &pos_init);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);
        beh->GetLocalParameterValue(0, &elapsedTime);
    }

    elapsedTime += (int)behcontext.DeltaTime;

    // getting the initial point
    beh->GetLocalParameterValue(3, &pos_init);

    float progression = (float)elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;
    value = curve->GetY(progression);

    //____________________________ Motion
    despt.x = pos_init.x + (despt.x - pos_init.x) * value;
    despt.y = pos_init.y + (despt.y - pos_init.y) * value;
    despt.z = pos_init.z + (despt.z - pos_init.z) * value;

    ent->SetPosition(&despt, NULL, k);

    //____________________________ End ?
    if (elapsedTime > time)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    //____________________________ or Loop ?
    beh->ActivateOutput(1, TRUE);

    // memorize elapsedTime
    beh->SetLocalParameterValue(0, &elapsedTime);

    // output Progression
    beh->SetOutputParameterValue(0, &progression);

    // output Value
    beh->SetOutputParameterValue(1, &value);

    return CKBR_OK;
}

/*******************************************************/
/*              Move To Not Time Based                 */
/*******************************************************/
int MoveTo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // we get the camera entity
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    VxVector goingVector;

    // Get the destination referential
    CK3dEntity *destref = NULL;
    CKParameterIn *pin = beh->GetInputParameter(1);
    int pindec = 0;
    if (pin->GetGUID() == CKPGUID_3DENTITY)
    {
        pindec = 1;
        destref = (CK3dEntity *)beh->GetInputParameterObject(1);
    }

    // we get the steps remaining (local parameter)
    int stepsRemaining = 0;
    beh->GetLocalParameterValue(0, &stepsRemaining);

    // getting the 2d curve
    CK2dCurve *accelerationCurve = NULL;
    beh->GetInputParameterValue(2 + pindec, &accelerationCurve);

    if (beh->IsInputActive(0))
    { // first time in a loop MoveTo is launched

        // we get the object position
        VxVector position;
        beh->GetInputParameterValue(0, &position);

        // we calculate the steps remaining
        beh->GetInputParameterValue(1 + pindec, &stepsRemaining);

        // we get the dest position
        VxVector destPosition;
        ent->GetPosition(&destPosition, destref);

        // we calculate the goingVector
        goingVector = position - destPosition;

        // we calculate the curveDeltaX
        float curveDeltaX = 0;
        float value = accelerationCurve->GetY(0);

        // output Progression
        beh->SetOutputParameterValue(0, &curveDeltaX);

        // output Value
        beh->SetOutputParameterValue(1, &value);

        curveDeltaX = 1.0f / (stepsRemaining);

        ///////////////////////////////
        // writing the local parameters

        // writing the stepsRemaining
        beh->SetLocalParameterValue(0, &stepsRemaining);

        // writing the curveLastX
        float clx = 0.0;
        beh->SetLocalParameterValue(1, &clx);

        // writing the curveDeltaX
        beh->SetLocalParameterValue(2, &curveDeltaX);

        // writing the goingVector
        beh->SetLocalParameterValue(3, &goingVector);

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    else
    { // we are in the loop

        ///////////////////////////////
        // getting the local parameters

        // getting the curveLastX
        float curveLastX;
        beh->GetLocalParameterValue(1, &curveLastX);

        // getting the curveDeltaX
        float curveDeltaX;
        beh->GetLocalParameterValue(2, &curveDeltaX);

        // getting the goingVector
        VxVector goingVector;
        beh->GetLocalParameterValue(3, &goingVector);

        // calculating the deltaY of the curve
        float progression = curveLastX + curveDeltaX;
        float value = accelerationCurve->GetY(progression);
        float deltaY = value - accelerationCurve->GetY(curveLastX);

        // output Progression
        beh->SetOutputParameterValue(0, &progression);

        // output Value
        beh->SetOutputParameterValue(1, &value);

        // hierarchy
        CKBOOL k = TRUE;
        beh->GetInputParameterValue(3 + pindec, &k);
        k = !k;

        // translating the camera
        VxVector trans = goingVector * deltaY;
        ent->Translate(&trans, destref, k);

        ///////////////////////////////
        // writing the local parameters

        // writing the stepsRemaining
        stepsRemaining--;
        beh->SetLocalParameterValue(0, &stepsRemaining);

        // writing the curveLastX
        curveLastX += curveDeltaX;
        beh->SetLocalParameterValue(1, &curveLastX);

        beh->ActivateInput(1, FALSE);

        if (stepsRemaining)
        { // MoveTo not finished
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        else
        { // loop accomplished
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
    }
    return CKBR_OK;
}
