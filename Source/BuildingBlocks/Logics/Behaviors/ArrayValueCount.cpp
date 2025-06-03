/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Value Count
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayValueCountDecl();
CKERROR CreateArrayValueCountProto(CKBehaviorPrototype **);
int ArrayValueCount(const CKBehaviorContext &behcontext);
CKERROR ArrayValueCountCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayValueCountDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Value Count");
    od->SetDescription("Returns the number of elements in a column matching the given value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>index of the column in which your search will be done.<BR>
    <SPAN CLASS=pin>Operator: </SPAN>comparison operator that will be used to compare the array value with the reference value.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>this output parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    <BR>
    <SPAN CLASS=pout>Count: </SPAN>number of elements in the given column matching the comparison.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x534377de, 0x75fd478a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayValueCountProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayValueCountProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Value Count");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column", CKPGUID_INT, "1");
    proto->DeclareInParameter("Operator", CKPGUID_COMPOPERATOR, "Equal");

    proto->DeclareOutParameter("Count", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayValueCount);
    proto->SetBehaviorCallbackFct(ArrayValueCountCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayValueCount(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 1;
    beh->GetInputParameterValue(0, &column);

    CKDWORD key = NULL;

    // we find the key
    int size = GetInputValue(array, beh, column, 2, key);

    // we find the operator
    CK_COMPOPERATOR op = CKEQUAL;
    beh->GetInputParameterValue(1, &op);

    // the search itself
    int count = array->GetCount(column, op, key, size);

    // we write the count in the output parameter
    beh->SetOutputParameterValue(0, &count);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayValueCountCallBack(const CKBehaviorContext &behcontext)
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

        // we get the column
        int column = 1;
        beh->GetInputParameterValue(0, &column);

        // we create the input parameter
        CreateInputParameter(array, beh, column, 2);
    }
    break;
    }

    return CKBR_OK;
}
