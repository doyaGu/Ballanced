/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//				Time Bezier Interpolator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTimeBezierInterpolatorDecl();
CKERROR CreateTimeBezierInterpolatorProto(CKBehaviorPrototype **);
int TimeBezierInterpolator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTimeBezierInterpolatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Time Bezier Interpolator");
    od->SetDescription("Interpolates a float according to a 2D Bezier curve in the [Min,Max] range, in a given number of milliseconds.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Time (ms): </SPAN>time in which the whole range will be covered.<BR>
    <SPAN CLASS=pin>Curve : </SPAN>2D Bezier curve.<BR>
    <SPAN CLASS=pin>Min : </SPAN>lower end of the range covered by the Bezier-interpolated value.<BR>
    <SPAN CLASS=pin>Max : </SPAN>higher end of the range covered by the Bezier-interpolated value.<BR>
    <BR>
    <SPAN CLASS=pout>Value : </SPAN>current Bezier-interpolated value.<BR>
    <SPAN CLASS=pout>Delta : </SPAN>subtraction between current Bezier-interpolated value and the last one.<BR>
    <BR>
    This behavior can be used to transform a linear progression to a Bezier curve progression in the [Min,Max] range.
    It is frame rate independent.<BR>
    <BR>
    See also : IBCQ, Bezier Interpolator
    */
    /* warning:
      - Obsolete building block, because didn't work with Activation and Deactivation of scripts (use Bezier Progression Instead).<BR>
      */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x34b2e0e, 0xcbf75e9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTimeBezierInterpolatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateTimeBezierInterpolatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Time Bezier Interpolator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Time (ms)", CKPGUID_INT, "3000");
    proto->DeclareInParameter("Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Min", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Max", CKPGUID_FLOAT, "1.0");

    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT, "0.0");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // "StartingTime"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(TimeBezierInterpolator);

    *pproto = proto;
    return CK_OK;
}

int TimeBezierInterpolator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTimeManager *tm = behcontext.TimeManager;

    // Get the total time
    int time = 3000;

    // patch for intermediary version
    CKParameterIn *pin = beh->GetInputParameter(0);
    if (pin && (pin->GetGUID() != CKPGUID_INT))
    {
        float time_float = 3000.0f;
        pin->GetValue(&time_float);
        time = (int)time_float;
    }
    else
    {
        beh->GetInputParameterValue(0, &time);
    }

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    // Get the range values
    float valueMin, valueMax;
    beh->GetInputParameterValue(2, &valueMin);
    beh->GetInputParameterValue(3, &valueMax);

    // Local Variables
    int startingTime, elapsedTime;
    float value, lastValue;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        startingTime = (int)tm->GetAbsoluteTime();
        beh->SetLocalParameterValue(0, &startingTime);

        lastValue = curve->GetY(0.0f);
        lastValue = valueMin + (valueMax - valueMin) * lastValue;
        beh->SetOutputParameterValue(0, &lastValue);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);

        beh->GetLocalParameterValue(0, &startingTime);
    }

    if (startingTime != -1)
    {
        elapsedTime = (int)tm->GetAbsoluteTime() - startingTime;

        if (elapsedTime > time)
        {
            value = curve->GetY(1.0f);

            startingTime = -1;
            beh->SetLocalParameterValue(0, &startingTime);
        }
        else
        {
            value = curve->GetY((float)elapsedTime / time);
        }

        value = valueMin + (valueMax - valueMin) * value;

        beh->GetOutputParameterValue(0, &lastValue);
        lastValue = value - lastValue;

        beh->SetOutputParameterValue(0, &value);
        beh->SetOutputParameterValue(1, &lastValue);

        beh->ActivateOutput(0, FALSE);
        beh->ActivateOutput(1, TRUE);
    }
    else
    {
        beh->ActivateOutput(0, TRUE);
        beh->ActivateOutput(1, FALSE);
    }

    return CKBR_OK;
}
