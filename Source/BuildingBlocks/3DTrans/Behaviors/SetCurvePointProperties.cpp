/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		      Set Curve Point Properties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetCurvePointPropertiesDecl();
CKERROR CreateSetCurvePointPropertiesProto(CKBehaviorPrototype **);
int SetCurvePointProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetCurvePointPropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Control Point Properties");
    od->SetDescription("Allows to set parameter values of a control point");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Bias: </SPAN>inflexes the tangent of curve at the control point.<BR>
    <SPAN CLASS=pin>Tension: </SPAN>modifies the absolute value of the tangent vector.<BR>
    <SPAN CLASS=pin>Continuity: </SPAN>defines the angle between right tangent vector and left one.<BR>
    <SPAN CLASS=pin>Linear: </SPAN>if TRUE, then curve at this point is not splined.<BR>
    <BR>
    This behavior set all specified parameters of a curvepoint.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1254effa, 0xfebec112));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetCurvePointPropertiesProto);
    od->SetCompatibleClassId(CKCID_CURVEPOINT);
    return od;
}

CKERROR CreateSetCurvePointPropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Control Point Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Bias", CKPGUID_FLOAT);
    proto->DeclareInParameter("Tension", CKPGUID_FLOAT);
    proto->DeclareInParameter("Continuity", CKPGUID_FLOAT);
    proto->DeclareInParameter("Linear", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCurvePointProperties);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetCurvePointProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurvePoint *ent = (CKCurvePoint *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get
    float local_bias = 0;
    beh->GetInputParameterValue(0, &local_bias);
    float local_tension = 0;
    beh->GetInputParameterValue(1, &local_tension);
    float local_continuity = 0;
    beh->GetInputParameterValue(2, &local_continuity);
    CKBOOL local_linear = FALSE;
    beh->GetInputParameterValue(3, &local_linear);

    // Behavior processing

    ent->SetBias(local_bias);
    ent->SetTension(local_tension);
    ent->SetContinuity(local_continuity);
    ent->SetLinear(local_linear);

    return CKBR_OK;
}
