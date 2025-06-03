/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MiniCalculator
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMiniCalculatorDecl();
CKERROR CreateMiniCalculatorProto(CKBehaviorPrototype **);
int MiniCalculator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMiniCalculatorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mini Calculator");
    od->SetDescription("Processes a simple operation (+,-,*,/) between 'a' and 'b'");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Operator: </SPAN>operator string (among '+', '-', '*', '/').<BR>
    <SPAN CLASS=pin>A: </SPAN>first value.<BR>
    <SPAN CLASS=pin>B: </SPAN>second value.<BR>
    <BR>
    <SPAN CLASS=pout>X : </SPAN>result of the operation.<BR>
    <BR>
    The supported operations (given as a string of characters) are '+' '-' '*' '/'.<BR>
    See also, enhanced version with "Calculator".
    */
    /* warning:
    - The only supproted type is FLOAT (and all the derived type like ANGLE and PERCENTAGE).<BR>
    - It is now easy and more natural to use the "Op" building block.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x55bc3115, 0x1dfe1e40));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMiniCalculatorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateMiniCalculatorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mini Calculator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Operator", CKPGUID_BINARYOPERATOR, "+");
    proto->DeclareInParameter("a", CKPGUID_FLOAT);
    proto->DeclareInParameter("b", CKPGUID_FLOAT);

    proto->DeclareOutParameter("x", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MiniCalculator);

    *pproto = proto;
    return CK_OK;
}

int MiniCalculator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float a, b;
    beh->GetInputParameterValue(1, &a);
    beh->GetInputParameterValue(2, &b);

    float x;

    CKParameterIn *pin = beh->GetInputParameter(0);

    if (pin->GetGUID() == CKPGUID_STRING)
    {
        char *op = (char *)beh->GetInputParameterReadDataPtr(0);

        if (*op == '+')
            x = a + b;
        else if (*op == '-')
            x = a - b;
        else if (*op == '*')
            x = a * b;
        else if (*op == '/')
            x = a / b;
    }
    else
    {
        int op = CKADD;
        beh->GetInputParameterValue(0, &op);
        if (op == CKADD)
            x = a + b;
        else if (op == CKSUB)
            x = a - b;
        else if (op == CKMUL)
            x = a * b;
        else if (op == CKDIV)
            x = a / b;
    }

    beh->SetOutputParameterValue(0, &x);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
