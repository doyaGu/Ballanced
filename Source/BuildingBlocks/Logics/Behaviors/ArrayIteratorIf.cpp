/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array IteratorIf
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayIteratorIfDecl();
CKERROR CreateArrayIteratorIfProto(CKBehaviorPrototype **);
int ArrayIteratorIf(const CKBehaviorContext &behcontext);
CKERROR ArrayIteratorIfCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayIteratorIfDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Iterator If");
    od->SetDescription("Iterates on each row matching a value and return the values of them.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>column in which your search will be done.<BR>
    <SPAN CLASS=pin>Operator: </SPAN>comparison operator that will be use to compare the column values with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>this output parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    <BR>
    <SPAN CLASS=pout>Row Index: </SPAN>index of the curent parsed row.<BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the current parsed row are written in these subsequent output parameters.<BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode (The output parameters are only created in author mode, when editing the building block).
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6bec4be6, 0x12d64c7c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayIteratorIfProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayIteratorIfProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Iterator If");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operator", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("Index", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArrayIteratorIf);
    proto->SetBehaviorCallbackFct(ArrayIteratorIfCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayIteratorIf(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int index;

    CKParameterOut *pout = beh->GetOutputParameter(0);

    if (beh->IsInputActive(0))
    { // we've entered by reset
        beh->ActivateInput(0, FALSE);
        index = 0;
    }
    else
    { // we've entered by next
        beh->ActivateInput(1, FALSE);
        int *pindex = (int *)pout->GetWriteDataPtr();
        index = (*pindex) + 1;
    }

    if (index >= array->GetRowCount())
    { // we've finished
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    int column = 0;
    beh->GetInputParameterValue(0, &column);

    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(1, &op);

    // we find the key
    CKDWORD key = NULL;
    int size = GetInputValue(array, beh, column, 2, key);

    // the getting itself
    int i = array->FindRowIndex(column, op, key, size, index);

    // we test if a value was found
    if (i < 0)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    // we get the index'th line
    CKDataRow *it = array->GetRow(i);
    // we increment index to the element next to the one found
    index = i;

    // we write the array data in the output param
    WriteOutputParameters(array, it, beh, 1);

    // we write the index in the output parameter
    beh->SetOutputParameterValue(0, &index);

    // We loop
    beh->ActivateOutput(1);

    return CKBR_OK;
}

CKERROR ArrayIteratorIfCallBack(const CKBehaviorContext &behcontext)
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

        // we create the output parameters
        CreateOutputParameters(array, beh, 1);

        int column = 0;
        beh->GetInputParameterValue(0, &column);

        // we create the input parameter
        CreateInputParameter(array, beh, column, 2, "Reference Value");
    }
    break;
    }

    return CKBR_OK;
}
