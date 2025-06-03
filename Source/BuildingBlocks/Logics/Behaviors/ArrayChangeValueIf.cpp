/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array ChangeValueIf
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayChangeValueIfDecl();
CKERROR CreateArrayChangeValueIfProto(CKBehaviorPrototype **);
int ArrayChangeValueIf(const CKBehaviorContext &behcontext);
CKERROR ArrayChangeValueIfCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayChangeValueIfDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Change Value If");
    od->SetDescription("Get elements from an array, using the input parameter key and then replace it by the 4th input parameter's value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when matching elements are found in specified column.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated if there's no matching elements in the column.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>index of the column in which your search will be done.<BR>
    <SPAN CLASS=pin>Operation: </SPAN>comparison operator used to compare the parsed values with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>this input parameter is created created when choosing the column and editing the building block.<BR>
    It must contains the reference value for the test.<BR>
    <SPAN CLASS=pin>(New Value) : </SPAN>Input parameter that will be created when choosing the column and editing the building block
    and that will contain the value to set in replacement of the matched one.<BR>
    <BR>
    <SPAN CLASS=pout>Number Changed: </SPAN>number of rows changed in the array.<BR>
    <BR>
    This building block will go through the entire array and will change the values in the rows
    for which the comparison of given column value and reference value will be true. If it finds somes,
    it exits by the first input and write in its output parameter the number of rows it actually changed.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7253edb, 0x4d1237ed));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayChangeValueIfProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayChangeValueIfProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Change Value If");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("Number Changed", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayChangeValueIf);
    proto->SetBehaviorCallbackFct(ArrayChangeValueIfCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayChangeValueIf(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column = 0;
    beh->GetInputParameterValue(0, &column);

    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(1, &op);

    // we find the key
    CKDWORD key = NULL;
    int size = GetInputValue(array, beh, column, 2, key);

    // we find the key
    CKParameterIn *pin = beh->GetInputParameter(3);
    CKParameter *pout = pin ? pin->GetRealSource() : NULL;

    int i = 0;
    int count = 0;
    while (i != -1)
    {
        // the getting itself
        i = array->FindRowIndex(column, op, key, size, i);
        if (i != -1)
        {
            // we set the new value
            array->SetElementValueFromParameter(i, column, pout);
            ++i;
            ++count;
        }
    }

    // we write the index
    beh->SetOutputParameterValue(0, &count);

    if (count == 0)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayChangeValueIfCallBack(const CKBehaviorContext &behcontext)
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

        // we create the value to compare with
        CreateInputParameter(array, beh, column, 2, "Reference Value");
        // we create the new param
        CreateInputParameter(array, beh, column, 3, "New Value");
    }
    break;
    }

    return CKBR_OK;
}
