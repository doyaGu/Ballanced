/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Search
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArraySearchDecl();
CKERROR CreateArraySearchProto(CKBehaviorPrototype **);
int ArraySearch(const CKBehaviorContext &behcontext);
CKERROR ArraySearchCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySearchDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Row Search");
    od->SetDescription("Get the first row index found in an array, where the value in the chosen column match the comparison with the reference value given.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when the element is found in the array.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated when the element is not found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>column in which your search will be done.<BR>
    <SPAN CLASS=pin>First Search Row: </SPAN>row index at which the searching process will start.<BR>
    <SPAN CLASS=pin>Operation: </SPAN>romparison operator that will be used to compare the array value with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>rnput parameter that will be created when choosing the column and editing the building block and that will contain the reference value to test.<BR>
    <BR>
    <SPAN CLASS=pout>First Row Found: </SPAN>row index of the first found element.<BR>
    <BR>
    This building block will go through the entire array and will stop at the first row for which the comparison
    of given column value and reference value will be true. If it finds ones, it exits by the first
    input and write in its output parameter the index of the first matching row. You can then reuse this index+1
    in the second input parameter to continue searching the array.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78863443, 0x45af59b6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySearchProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySearchProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Row Search");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("First Search Row", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("First Row Found", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArraySearch);
    proto->SetBehaviorCallbackFct(ArraySearchCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArraySearch(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column = 0;
    beh->GetInputParameterValue(0, &column);

    int firstline = 0;
    beh->GetInputParameterValue(1, &firstline);

    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(2, &op);

    // we find the key
    CKDWORD key = NULL;
    int size = GetInputValue(array, beh, column, 3, key);

    // the getting itself
    int i = array->FindRowIndex(column, op, key, size, firstline);

    if (i < 0)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // we write the index
    beh->SetOutputParameterValue(0, &i);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArraySearchCallBack(const CKBehaviorContext &behcontext)
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
        beh->GetInputParameterValue(0, &column);

        // we create the input parameter
        CreateInputParameter(array, beh, column, 3, "Reference Value");
    }
    break;
    }

    return CKBR_OK;
}
