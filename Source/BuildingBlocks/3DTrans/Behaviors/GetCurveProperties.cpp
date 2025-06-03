/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		      Get Curve Properties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetCurvePropertiesDecl();
CKERROR CreateGetCurvePropertiesProto(CKBehaviorPrototype **);
int GetCurveProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetCurvePropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Curve Properties");
    od->SetDescription("Allows to get parameter values of curve");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Open: </SPAN>if TRUE, then the curve is not closed.<BR>
    <SPAN CLASS=pout>Number Of Steps: </SPAN>number of segments which compose the curve.<BR>
    <SPAN CLASS=pout>Fitting Coef: </SPAN>correlation of curvepoints.<BR>
    <SPAN CLASS=pout>Length: </SPAN>length of curve.<BR>
    <BR>
    This behavior got all specified parameters of a curve.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1251adfa, 0xaebec002));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetCurvePropertiesProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    return od;
}

CKERROR CreateGetCurvePropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Curve Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Open", CKPGUID_BOOL);
    proto->DeclareOutParameter("Number Of Steps", CKPGUID_INT);
    proto->DeclareOutParameter("Fitting Coef", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Length", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetCurveProperties);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetCurveProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurve *ent = (CKCurve *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set
    CKBOOL local_open = ent->IsOpen();
    beh->SetOutputParameterValue(0, &local_open);
    int local_stepcount = ent->GetStepCount();
    beh->SetOutputParameterValue(1, &local_stepcount);
    float local_fittingcoeff = ent->GetFittingCoeff();
    beh->SetOutputParameterValue(2, &local_fittingcoeff);
    float local_length = ent->GetLength();
    beh->SetOutputParameterValue(3, &local_length);

    return CKBR_OK;
}
