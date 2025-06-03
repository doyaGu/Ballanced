/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array RemoveIf
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayRemoveIfDecl();
CKERROR CreateArrayRemoveIfProto(CKBehaviorPrototype **);
int ArrayRemoveIf(const CKBehaviorContext &behcontext);
CKERROR ArrayRemoveIfCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayRemoveIfDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Row If");
    od->SetDescription("Remove rows matching a value from an array.");
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
    <BR>
    <SPAN CLASS=pout>Number Removed: </SPAN>number of rows removed from the array.<BR>
    <BR>
    This building block will go through the entire array and will remove the rows for which the comparison
    of given column value and reference value will be true. If it finds somes, it exits by the first
    input and write in its output parameter the number of rows it actually removed.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x57865622, 0x662d2fee));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayRemoveIfProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayRemoveIfProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Row If");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("Number Removed", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayRemoveIf);
    proto->SetBehaviorCallbackFct(ArrayRemoveIfCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayRemoveIf(const CKBehaviorContext &behcontext)
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

    int i = 0;
    int count = 0;
    while (i != -1)
    {
        // the getting itself
        i = array->FindRowIndex(column, op, key, size, i);
        if (i != -1)
        {
            array->RemoveRow(i);
            ++count;
            // we test if we remove the last element
            if (i >= array->GetRowCount())
                break;
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

CKERROR ArrayRemoveIfCallBack(const CKBehaviorContext &behcontext)
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
        CreateInputParameter(array, beh, column, 2, "Reference Value");
    }
    break;
    }

    return CKBR_OK;
}
