/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//				Time Bezier Interpolator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBezierProgressionDecl();
CKERROR CreateBezierProgressionProto(CKBehaviorPrototype **);
int BezierProgression(const CKBehaviorContext &behcontext);
int BezierProgressionTimeBased(const CKBehaviorContext &behcontext);
int BezierProgressionTimeBasedOld(const CKBehaviorContext &behcontext); // fpr back compatibility
CKERROR BezierProgressionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBezierProgressionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Bezier Progression");
    od->SetDescription("Interpolates a float according to a 2D Bezier curve in the [Min,Max] range, in a given number of milliseconds.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>or Number of Frames: </SPAN>number of steps in which the whole range is covered.<BR>
    <SPAN CLASS=pin>A: </SPAN>lower end of the range covered by the Bezier-interpolated value.<BR>
    <SPAN CLASS=pin>B: </SPAN>higher end of the range covered by the Bezier-interpolated value.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the 3D Entity along its movement.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>time elapsed since the start of the progression.<BR>
    <SPAN CLASS=pout>or Current Frame: </SPAN>current step of the progression.<BR>
    <SPAN CLASS=pout>Value: </SPAN>current Bezier-interpolated value.<BR>
    <SPAN CLASS=pout>Delta: </SPAN>subtraction between current Bezier-interpolated value and the previous one.<BR>
    <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.
  Useful if you need to interpolates at the same time (in the same loop) a color vlaue, a vector, a orientation or something like this.<BR>
    <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame
    Rate dependant. Making this building block Time dependant has the advantage that compositions
    will run at the same rate on all computer configurations.<BR>
  Tip: Since the 'Progression' value is given as an output parameter, you can use it in the loop to interpolate the orientation of the object at the same time (with the 'Interpolator Orientation' building block for example).<BR>
    <BR>
    This building block can be used to transform a linear progression to a Bezier curve progression in the [A,B] range.<BR>
    <BR>
    See also: Linear Progression.<BR>
    */
    /* warning:
  - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
  The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
  This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
  - also Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
  (for the 'Progression' output parameter)<BR>
  */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6bb8699a, 0x29fb6a4b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateBezierProgressionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Loops");
    return od;
}

CKERROR CreateBezierProgressionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Bezier Progression");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 3s 0ms");
    proto->DeclareInParameter("A", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("B", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BezierProgressionTimeBased);
    proto->SetBehaviorCallbackFct(BezierProgressionCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/* Bezier Progression (TIME Based new version (float)) */
/*******************************************************/
int BezierProgressionTimeBased(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get the total time
    float time;
    beh->GetInputParameterValue(0, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(3, &curve);
    if (!curve)
        return CKBR_OK;

    // Get the range values
    float valueMin, valueMax;
    beh->GetInputParameterValue(1, &valueMin);
    beh->GetInputParameterValue(2, &valueMax);

    // Local Variables
    float elapsedTime;
    float value, lastValue;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0;
        beh->SetOutputParameterValue(0, &elapsedTime);

        lastValue = curve->GetY(0.0f);
        lastValue = valueMin + (valueMax - valueMin) * lastValue;
        beh->SetOutputParameterValue(1, &lastValue);
        float delta = 0;
        beh->SetOutputParameterValue(2, &delta);
        beh->SetOutputParameterValue(3, &delta);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);

        beh->GetOutputParameterValue(0, &elapsedTime);
    }

    elapsedTime += behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &elapsedTime);

    float progression = elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;

    value = curve->GetY(progression);
    value = valueMin + (valueMax - valueMin) * value;

    beh->GetOutputParameterValue(1, &lastValue);
    lastValue = value - lastValue;

    beh->SetOutputParameterValue(1, &value);
    beh->SetOutputParameterValue(2, &lastValue);
    beh->SetOutputParameterValue(3, &progression);

    if (elapsedTime > time)
    {
        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    beh->ActivateOutput(1, TRUE);

    return CKBR_OK;
}

/*******************************************************/
/*  Bezier Progression (TIME Based old version (int))  */
/*******************************************************/
int BezierProgressionTimeBasedOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get the total time
    int time;
    beh->GetInputParameterValue(0, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(3, &curve);
    if (!curve)
        return CKBR_OK;

    // Get the range values
    float valueMin, valueMax;
    beh->GetInputParameterValue(1, &valueMin);
    beh->GetInputParameterValue(2, &valueMax);

    // Local Variables
    int elapsedTime;
    float value, lastValue;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0;
        beh->SetOutputParameterValue(0, &elapsedTime);

        lastValue = curve->GetY(0.0f);
        lastValue = valueMin + (valueMax - valueMin) * lastValue;
        beh->SetOutputParameterValue(1, &lastValue);
        float delta = 0;
        beh->SetOutputParameterValue(2, &delta);
        beh->SetOutputParameterValue(3, &delta);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);

        beh->GetOutputParameterValue(0, &elapsedTime);
    }

    elapsedTime += (int)behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &elapsedTime);

    float progression = (float)elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;

    value = curve->GetY(progression);
    value = valueMin + (valueMax - valueMin) * value;

    beh->GetOutputParameterValue(1, &lastValue);
    lastValue = value - lastValue;

    beh->SetOutputParameterValue(1, &value);
    beh->SetOutputParameterValue(2, &lastValue);
    beh->SetOutputParameterValue(3, &progression);

    if (elapsedTime > time)
    {
        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    beh->ActivateOutput(1, TRUE);

    return CKBR_OK;
}

/*******************************************************/
/*     Bezier Progression Not Time Based               */
/*******************************************************/
int BezierProgression(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Get the total time
    int time;
    beh->GetInputParameterValue(0, &time);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(3, &curve);
    if (!curve)
        return CKBR_OK;

    // Get the range values
    float valueMin, valueMax;
    beh->GetInputParameterValue(1, &valueMin);
    beh->GetInputParameterValue(2, &valueMax);

    // Local Variables
    int elapsedTime;
    float value, lastValue;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        elapsedTime = 0;
        beh->SetOutputParameterValue(0, &elapsedTime);

        lastValue = curve->GetY(0.0f);
        lastValue = valueMin + (valueMax - valueMin) * lastValue;
        beh->SetOutputParameterValue(1, &lastValue);
        float delta = 0.0f;
        beh->SetOutputParameterValue(2, &delta);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);

        beh->GetOutputParameterValue(0, &elapsedTime);
    }

    ++elapsedTime;
    beh->SetOutputParameterValue(0, &elapsedTime);

    float progression = (float)elapsedTime / time;
    if (progression > 1.0f)
        progression = 1.0f;

    value = curve->GetY(progression);
    value = valueMin + (valueMax - valueMin) * value;

    beh->GetOutputParameterValue(1, &lastValue);
    lastValue = value - lastValue;

    beh->SetOutputParameterValue(1, &value);
    beh->SetOutputParameterValue(2, &lastValue);
    beh->SetOutputParameterValue(3, &progression);

    if ((float)elapsedTime > time)
    {
        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    beh->ActivateOutput(1, TRUE);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR BezierProgressionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = TRUE;
        beh->GetLocalParameterValue(0, &time);

        CKParameterIn *pin = beh->GetInputParameter(0);
        CKParameterOut *pout = beh->GetOutputParameter(0);

        CKGUID currentGuid = pin->GetGUID();
        CKBOOL isOldVersion = (beh->GetVersion() < 0x00010005);

        // Time based or not ?
        if (time)
        {
            if (isOldVersion && (currentGuid != CKPGUID_TIME))
            {
                beh->SetFunction(BezierProgressionTimeBasedOld);
                pin->SetGUID(CKPGUID_INT, TRUE, "Duration (ms)");
                if (pout)
                {
                    pout->SetName("Elapsed Time");
                    pout->SetGUID(CKPGUID_INT);
                }
            }
            else
            {
                beh->SetFunction(BezierProgressionTimeBased);
                pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
                if (pout)
                {
                    pout->SetName("Elapsed Time");
                    pout->SetGUID(CKPGUID_TIME);
                }
                if (isOldVersion)
                    beh->SetVersion(0x00010005);
            }
        }
        else
        {
            beh->SetFunction(BezierProgression);
            pin->SetGUID(CKPGUID_INT, TRUE, "Number of Frame");
            if (pout)
            {
                pout->SetName("Current Frame");
                pout->SetGUID(CKPGUID_INT);
            }
        }
    }
    }

    return CKBR_OK;
}
