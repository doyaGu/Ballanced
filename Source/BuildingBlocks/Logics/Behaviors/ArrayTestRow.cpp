/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array TestRow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayTestRowDecl();
CKERROR CreateArrayTestRowProto(CKBehaviorPrototype **);
int ArrayTestRow(const CKBehaviorContext &behcontext);
CKERROR ArrayTestRowCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayTestRowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Test Cell");
    od->SetDescription("Compare an array element (cell) with a given reference value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated when the test succeed for the given row.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated when the test failed for the given row.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>index of the row to test.<BR>
    <SPAN CLASS=pin>Column: </SPAN>index of the column in which your search will be done.<BR>
    <SPAN CLASS=pin>Operator: </SPAN>comparison operator that will be used to compare the array value with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>this output parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4e6c6da8, 0x636904fc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayTestRowProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayTestRowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Test Cell");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");
    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_COMPOPERATOR, "Equal");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayTestRow);
    proto->SetBehaviorCallbackFct(ArrayTestRowCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayTestRow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int index = 0;
    beh->GetInputParameterValue(0, &index);

    int column = 0;
    beh->GetInputParameterValue(1, &column);

    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(2, &op);

    // we find the key
    CKDWORD key = NULL;
    int size = GetInputValue(array, beh, column, 3, key);

    // the test itself
    if (array->TestRow(index, column, op, key, size))
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_OK;
}

CKERROR ArrayTestRowCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIOREDITED:
    {
        CKDataArray *array = (CKDataArray *)beh->GetTarget();
        if (!array)
            return CKBR_OK;

        int column = 0;
        beh->GetInputParameterValue(1, &column);

        // we create the input parameter
        CreateInputParameter(array, beh, column, 3, "Reference Value");
    }
    break;
    }

    return CKBR_OK;
}
