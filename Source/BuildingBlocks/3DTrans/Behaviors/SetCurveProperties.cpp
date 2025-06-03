/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Set Curve Properties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetCurvePropertiesDecl();
CKERROR CreateSetCurvePropertiesProto(CKBehaviorPrototype **);
int SetCurveProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetCurvePropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Curve Properties");
    od->SetDescription("Allows to set parameter values of curve");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Open: </SPAN>if TRUE, then the curve is not closed.<BR>
    <SPAN CLASS=pin>Number Of Steps: </SPAN>defines the number of segments which compose the curve.<BR>
    <SPAN CLASS=pin>Fitting Coef: </SPAN>defines the correlation of curvepoints.<BR>
    <BR>
    This behavior set all specified parameters of a curvepoint.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1257effa, 0xf7bec112));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetCurvePropertiesProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    return od;
}

CKERROR CreateSetCurvePropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Curve Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Open", CKPGUID_BOOL);
    proto->DeclareInParameter("Number Of Steps", CKPGUID_INT);
    proto->DeclareInParameter("Fitting Coef", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCurveProperties);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetCurveProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurve *ent = (CKCurve *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get
    CKBOOL local_isopen = TRUE;
    beh->GetInputParameterValue(0, &local_isopen);
    int local_stepcount = 100;
    beh->GetInputParameterValue(1, &local_stepcount);
    float local_fittingcoeff = 0;
    beh->GetInputParameterValue(2, &local_fittingcoeff);

    // Behavior processing

    if (local_isopen)
        ent->Open();
    else
        ent->Close();
    ent->SetStepCount(local_stepcount);
    ent->SetFittingCoeff(local_fittingcoeff);

    return CKBR_OK;
}
