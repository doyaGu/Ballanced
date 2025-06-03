/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Threshold
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorThresholdDecl();
CKERROR CreateThresholdProto(CKBehaviorPrototype **);
int Threshold(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorThresholdDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Threshold");
    od->SetDescription("Binds a variable between two limits (Min and Max), and exit by the corresponding output according to the comparison result.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>X < Min: </SPAN>is activated if x < min.<BR>
    <SPAN CLASS=out>X > Max: </SPAN>is activated if x > max.<BR>
    <SPAN CLASS=out>Min < X < Max: </SPAN>is activated if x > min and x < max.<BR>
    <BR>
    <SPAN CLASS=pin>X: </SPAN>value to bind.<BR>
    <SPAN CLASS=pin>Min: </SPAN>minimum value.<BR>
    <SPAN CLASS=pin>Max: </SPAN>maximum value.<BR>
    <BR>
    <SPAN CLASS=pout>X: </SPAN>bounded value.<BR>
    <BR>
    This building block is useful to bind a variable between two limits (Min and Max).<BR>
    eg:<BR>
    let Min=2 and Max=5, then if X=13 then bounded X value=5, and if X=0 then bounded X value=2.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x655e6af4, 0x08c0596f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateThresholdProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateThresholdProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Threshold");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("X < MIN");
    proto->DeclareOutput("X > MAX");
    proto->DeclareOutput("MIN < X < MAX");

    proto->DeclareInParameter("X", CKPGUID_FLOAT);
    proto->DeclareInParameter("MIN", CKPGUID_FLOAT);
    proto->DeclareInParameter("MAX", CKPGUID_FLOAT);

    proto->DeclareOutParameter("X", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Threshold);

    *pproto = proto;
    return CK_OK;
}

int Threshold(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    float x, m;
    beh->GetInputParameterValue(0, &x); // get x

    beh->GetInputParameterValue(1, &m); // x < min ?
    if (x < m)
    {
        beh->ActivateOutput(0);
        goto COMMON;
    }

    beh->GetInputParameterValue(2, &m); // x > max ?
    if (x > m)
    {
        beh->ActivateOutput(1);
        goto COMMON;
    }

    m = x;
    beh->ActivateOutput(2); // min < x < max

COMMON:
    x = m;
    beh->SetOutputParameterValue(0, &x);
    return CKBR_OK;
}
