/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		    Get Control Point Properties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetCurvePointPropertiesDecl();
CKERROR CreateGetCurvePointPropertiesProto(CKBehaviorPrototype **);
int GetCurvePointProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurvePointPropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Control Point Properties");
    od->SetDescription("Allows to get parameter values of a control point");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Bias: </SPAN>inflex of the tangent of curve at the control point.<BR>
    <SPAN CLASS=pout>Tension: </SPAN>absolute value of the tangent vector.<BR>
    <SPAN CLASS=pin>Continuity: </SPAN>angle between right tangent vector and left one.<BR>
    <SPAN CLASS=pin>Linear: </SPAN>if TRUE, then curve at this point is not splined.<BR>
    <BR>
    This behavior got all specified parameters of a curvepoint.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1251effa, 0xaebec112));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurvePointPropertiesProto);
    od->SetCompatibleClassId(CKCID_CURVEPOINT);
    return od;
}

CKERROR CreateGetCurvePointPropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Control Point Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Bias", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Tension", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Continuity", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Linear", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurvePointProperties);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetCurvePointProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurvePoint *ent = (CKCurvePoint *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set
    float local_bias = ent->GetBias();
    beh->SetOutputParameterValue(0, &local_bias);
    float local_tension = ent->GetTension();
    beh->SetOutputParameterValue(1, &local_tension);
    float local_continuity = ent->GetContinuity();
    beh->SetOutputParameterValue(2, &local_continuity);
    CKBOOL local_linear = ent->IsLinear();
    beh->SetOutputParameterValue(3, &local_linear);

    return CKBR_OK;
}
