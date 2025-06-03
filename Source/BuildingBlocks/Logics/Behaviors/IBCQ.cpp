/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//				Incremental Bezier Curve Query
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorIBCQDecl();
CKERROR CreateIBCQProto(CKBehaviorPrototype **);
int QueryCurveFct(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIBCQDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("IBCQ");
    od->SetDescription("Incremental Bezier Curve Query: Interpolates a float according to a 2D Bezier curve in the [0,1] range.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Delta_In: </SPAN>linear progression step in the [0%, 100%] range. (if Delta=1%, there will be 100 steps in the progression).<BR>
    <SPAN CLASS=pin>Progression Curve: </SPAN>2D Curve representing the progression of the 3D Entity along its movement.<BR>
    <BR>
    <SPAN CLASS=pout>Delta_Out: </SPAN>current Bezier-interpolated Delta value (PERCENTAGE).<BR>
    <SPAN CLASS=pout>Value: </SPAN>current Bezier-interpolated value (PERCENTAGE).<BR>
    <SPAN CLASS=pout>Progression: </SPAN>percentage between 0% and 100% which defines the progression of the process. start=0%, middle time=50%, end=100%.
    Useful if you need to interpolates at the same time (in the same loop) a color vlaue, a vector, a orientation or something like this.<BR>
    <BR>
    This building block can be used to transform a linear progression to a Bezier curve progression. Useful to simulate acceleration on a movement.<BR>
    <BR>
    See also: Bezier Interpolator, Time Bezier Interpolator, Bezier Interpolator.<BR>
    */
    /* warning:
    - Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf5f22514, 0x123fffee));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIBCQProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIBCQProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("IBCQ");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Delta_In", CKPGUID_PERCENTAGE, "0.1");
    proto->DeclareInParameter("Curve", CKPGUID_2DCURVE);

    proto->DeclareOutParameter("Delta_Out", CKPGUID_PERCENTAGE, "0.1");
    proto->DeclareOutParameter("Value", CKPGUID_PERCENTAGE, "0");
    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");

    proto->DeclareLocalParameter(NULL, CKPGUID_FLOAT); // "Prev_In"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(QueryCurveFct);

    *pproto = proto;
    return CK_OK;
}

int QueryCurveFct(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dCurve *curve = NULL;
    beh->GetInputParameterValue(1, &curve);

    if (beh->IsInputActive(0)) // IN
    {
        beh->ActivateInput(0, FALSE);

        // Initialisation of the Locals and Output Parameters
        float prev_in_init = 0.0f;
        beh->SetLocalParameterValue(0, &prev_in_init);

        float prev_out_init = curve->GetY(prev_in_init);
        beh->SetOutputParameterValue(1, &prev_out_init);
    }
    else // LOOP IN
    {
        beh->ActivateInput(1, FALSE);
    }

    float prev_in, delta_in = 0.1f;
    float prev_out, delta_out;
    beh->GetLocalParameterValue(0, &prev_in);
    beh->GetInputParameterValue(0, &delta_in);
    beh->GetOutputParameterValue(1, &prev_out);

    prev_in += delta_in;
    if (prev_in > 1.0f)
    {
        float tmp = 1.0f;
        beh->SetOutputParameterValue(1, &tmp);
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    // Process Maths ...
    float tmp = curve->GetY(prev_in);
    delta_out = tmp - prev_out;
    prev_out = tmp;

    // refresh the parameters....
    beh->SetLocalParameterValue(0, &prev_in);    // locals
    beh->SetOutputParameterValue(0, &delta_out); // output
    beh->SetOutputParameterValue(1, &prev_out);

    beh->SetOutputParameterValue(2, &prev_in); // progression

    beh->ActivateOutput(1);
    return CKBR_OK;
}
