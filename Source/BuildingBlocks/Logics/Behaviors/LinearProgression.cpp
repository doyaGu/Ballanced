/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Linear Progression
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLinearProgressionDecl();
CKERROR CreateLinearProgressionProto(CKBehaviorPrototype **);
int LinearProgression(const CKBehaviorContext &behcontext);
int LinearProgressionTimeBased(const CKBehaviorContext &behcontext);
int LinearProgressionTimeBasedOld(const CKBehaviorContext &behcontext);
CKERROR LinearProgressionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLinearProgressionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Linear Progression");
    od->SetDescription("Increments from 'Start Value' to 'End Value' in 'Number of Steps' steps.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <SPAN CLASS=pin>or Number of Frames: </SPAN>number of steps in which the whole range is covered.<BR>
    <SPAN CLASS=pin>A: </SPAN>First value of the progression.<BR>
    <SPAN CLASS=pin>B: </SPAN>Last value of the progression.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>time elapsed since the start of the progression.<BR>
    <SPAN CLASS=pout>or Current Frame: </SPAN>current step of the progression.<BR>
    <SPAN CLASS=pout>Value: </SPAN>current interpolated value.<BR>
    <SPAN CLASS=pout>Delta: </SPAN>subtraction between current value and the last one.<BR>
    <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.
    Useful if you need to interpolates at the same time (in the same loop) a color vlaue, a vector, a orientation or something like this.<BR>
    <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame
    Rate dependant. Making this building block Time dependant has the advantage that compositions
    will run at the same rate on all computer configurations.<BR>
    Tip: Since the 'Progression' value is given as an output parameter, you can use it in the loop to interpolate the orientation of the object at the same time (with the 'Interpolator Orientation' building block for example).<BR>
    <BR>
    This building block is ideal for linear progressions.<BR>
    The "In" input resets the current step to 0.<BR>
    <BR>
    See also: Bezier Progression.<BR>
    */
    /* warning:
    - If <FONT COLOR=#000077>Number of Steps</FONT>= 5, <FONT COLOR=#a03030>Loop Out </FONT>will be activated 5 time, but as you
    enter by <FONT COLOR=#a03030>In </FONT> at begining, you'll then enter 1 + 5 time into the building block; that's why
    at the end of last loop, current step will be equal to 6.<BR>
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    - also Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
    (for the 'Progression' output parameter)<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfff45680, 0xaa512a39));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateLinearProgressionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLinearProgressionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Linear Progression");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Time", CKPGUID_TIME, "0m 3s 0ms");
    proto->DeclareInParameter("A", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("B", CKPGUID_FLOAT, "1.0");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LinearProgressionTimeBased);
    proto->SetBehaviorCallbackFct(LinearProgressionCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*     Bezier Progression Not Time Based               */
/*******************************************************/
int LinearProgression(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int maxcount = 0;
    int current = 0;

    float A;
    beh->GetInputParameterValue(1, &A);

    float B;
    beh->GetInputParameterValue(2, &B);

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        // we initialize the old value
        beh->SetOutputParameterValue(1, &A);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);
    }

    ++current;
    beh->SetOutputParameterValue(0, &current);

    beh->GetInputParameterValue(0, &maxcount);

    float Pos = B;
    float progression = (float)current / maxcount;

    if (current > maxcount)
        beh->ActivateOutput(0);
    else // not yet finish...
    {
        Pos = A + (B - A) * progression;
        beh->ActivateOutput(1);
    }

    float oldPos;
    beh->GetOutputParameterValue(1, &oldPos);
    oldPos = Pos - oldPos;

    beh->SetOutputParameterValue(1, &Pos);
    beh->SetOutputParameterValue(2, &oldPos);
    beh->SetOutputParameterValue(3, &progression);

    return CKBR_OK;
}

/*******************************************************/
/* Bezier Progression (TIME Based new version (float)) */
/*******************************************************/
int LinearProgressionTimeBased(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float maxcount = 0.0f;
    float current = 0.0f;

    float A;
    beh->GetInputParameterValue(1, &A);

    float B;
    beh->GetInputParameterValue(2, &B);

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        // we initialize the old value
        beh->SetOutputParameterValue(1, &A);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);
    }

    current += behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &current);

    beh->GetInputParameterValue(0, &maxcount);

    float Pos = B;
    float progression = current / maxcount;

    if (current > maxcount)
        beh->ActivateOutput(0);
    else // not yet finish...
    {
        Pos = A + (B - A) * progression;
        beh->ActivateOutput(1);
    }

    float oldPos;
    beh->GetOutputParameterValue(1, &oldPos);
    oldPos = Pos - oldPos;

    beh->SetOutputParameterValue(1, &Pos);
    beh->SetOutputParameterValue(2, &oldPos);
    beh->SetOutputParameterValue(3, &progression);

    return CKBR_OK;
}

/*******************************************************/
/*  Bezier Progression (TIME Based old version (int))  */
/*******************************************************/
int LinearProgressionTimeBasedOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int maxcount = 0;
    int current = 0;

    float A;
    beh->GetInputParameterValue(1, &A);

    float B;
    beh->GetInputParameterValue(2, &B);

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        // we initialize the old value
        beh->SetOutputParameterValue(1, &A);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);
    }

    current += (int)behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &current);

    beh->GetInputParameterValue(0, &maxcount);

    float Pos = B;
    float progression = (float)current / maxcount;

    if (current > maxcount)
        beh->ActivateOutput(0);
    else // not yet finish...
    {
        Pos = A + (B - A) * progression;
        beh->ActivateOutput(1);
    }

    float oldPos;
    beh->GetOutputParameterValue(1, &oldPos);
    oldPos = Pos - oldPos;

    beh->SetOutputParameterValue(1, &Pos);
    beh->SetOutputParameterValue(2, &oldPos);
    beh->SetOutputParameterValue(3, &progression);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR LinearProgressionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = FALSE;
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
                beh->SetFunction(LinearProgressionTimeBasedOld);
                pin->SetGUID(CKPGUID_INT, TRUE, "Duration (ms)");
                if (pout)
                {
                    pout->SetName("Elapsed Time");
                    pout->SetGUID(CKPGUID_INT);
                }
            }
            else
            {
                beh->SetFunction(LinearProgressionTimeBased);
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
            beh->SetFunction(LinearProgression);
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
