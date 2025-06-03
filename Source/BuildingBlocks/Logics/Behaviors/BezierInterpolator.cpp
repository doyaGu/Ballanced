/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//				Bezier Interpolator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBezierInterpolatorDecl();
CKERROR CreateBezierInterpolatorProto(CKBehaviorPrototype **);
int BezierInterpolator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBezierInterpolatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Bezier Interpolator");
    od->SetDescription("Interpolates a float according to a 2D Bezier curve in the [Min,Max] range, in a given number of steps.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Number of steps: </SPAN>number of steps in which the whole range will be covered.<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression scheme of the output variable.<BR>
    <SPAN CLASS=pin>Min: </SPAN>lower end of the range covered by the Bezier-interpolated value.<BR>
    <SPAN CLASS=pin>Max: </SPAN>higher end of the range covered by the Bezier-interpolated value.<BR>
    <BR>
    <SPAN CLASS=pout>Value: </SPAN>current Bezier-interpolated value.<BR>
    <SPAN CLASS=pout>Delta: </SPAN>subtraction between current Bezier-interpolated value and the previous one.<BR>
    <BR>
    This building block can be used to transform a linear progression to a Bezier curve progression in the [Min,Max] range.<BR>
    <BR>
    See also : IBCQ, Time Bezier Interpolator.<BR>
    */
    /* warning:
    - This building block is obsolete : use Bezier Progression instead.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x71570e39, 0x5b37428a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBezierInterpolatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Interpolator");
    return od;
}

CKERROR CreateBezierInterpolatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Bezier Interpolator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");
    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Number of steps", CKPGUID_INT, "100");
    proto->DeclareInParameter("Progression Curve", CKPGUID_2DCURVE);
    proto->DeclareInParameter("Min", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Max", CKPGUID_FLOAT, "1.0");

    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT, "0.0");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); //"Previous"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(BezierInterpolator);

    *pproto = proto;
    return CK_OK;
}

int BezierInterpolator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get the number of steps
    int frames;
    beh->GetInputParameterValue(0, &frames);

    // Get the progression curve
    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    // Get the range values
    float valueMin, valueMax;
    beh->GetInputParameterValue(2, &valueMin);
    beh->GetInputParameterValue(3, &valueMax);

    // Local Variables
    int previous = 0;
    float lastValue, value;

    if (beh->IsInputActive(0))
    {
        // Enter by "In"
        beh->ActivateInput(0, FALSE);

        beh->SetLocalParameterValue(0, &previous);

        lastValue = curve->GetY(0.0f);
        lastValue = valueMin + (valueMax - valueMin) * lastValue;
        beh->SetOutputParameterValue(0, &lastValue);
    }
    else
    {
        // Enter by "Loop In"
        beh->ActivateInput(1, FALSE);
    }

    beh->GetLocalParameterValue(0, &previous);

    value = curve->GetY((float)previous / (frames - 1));
    value = valueMin + (valueMax - valueMin) * value;

    beh->GetOutputParameterValue(0, &lastValue);
    lastValue = value - lastValue;

    beh->SetOutputParameterValue(0, &value);
    beh->SetOutputParameterValue(1, &lastValue);

    previous++;
    if (previous >= frames)
        beh->ActivateOutput(0);
    else
    {
        beh->SetLocalParameterValue(0, &previous);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
