/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Set
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArraySetDecl();
CKERROR CreateArraySetProto(CKBehaviorPrototype **);
int ArraySet(const CKBehaviorContext &behcontext);
CKERROR ArraySetCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySetDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Key Row");
    od->SetDescription("Sets the elements of a row, using the key column as a search criterion.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when the element is found in the array.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated when the element is not found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>(Element Values): </SPAN>all the elements of the row to set are read from
    these input parameters. The value you give at the input representing the
    key column is used to find the row in the array. If more than one row
    containing this value in the key column are present, Set Key Row will only
    change the values of the first one found, from the start of the array.
    If you don't link some input parameters, the values will be unchanged.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2fff4c63, 0x4fcb1b94));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySetProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySetProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Key Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArraySet);
    proto->SetBehaviorCallbackFct(ArraySetCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArraySet(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column = array->GetKeyColumn();
    if (column < 0)
    {
        behcontext.Context->OutputToConsole("ArraySet Error : No column criterion defined");
        return CKBR_PARAMETERERROR;
    }

    // the insertion itself
    CKDWORD key;
    int size = GetInputValue(array, beh, column, column, key);
    int index = array->FindRowIndex(column, CKEQUAL, key, size);
    if (index < 0)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    CKDataRow *it = array->GetRow(index);

    // we put the input parameters value in the array
    ReadInputParameters(array, it, beh, 0);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArraySetCallBack(const CKBehaviorContext &behcontext)
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

        // we create the input parameters
        CreateInputParameters(array, beh, 0);
    }
    break;
    }

    return CKBR_OK;
}
