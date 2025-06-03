/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Remove Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorRemoveArrayDecl();
CKERROR CreateRemoveArrayProto(CKBehaviorPrototype **);
int RemoveArray(const CKBehaviorContext &behcontext);
CKERROR RemoveArrayCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Key Row");
    od->SetDescription("Remove elements from an array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
  <SPAN CLASS=out>Out: </SPAN>is activated when elements are found and remove from group.<BR>
    <SPAN CLASS=out>No Elements Found: </SPAN>is activated when no element has been found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>Key: </SPAN>Input parameter that will be removed.<BR>
    <BR>
    This building block, using the input parameter as a research key in the key column,
    remove the first (and so unique) found row, if any.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf8334ea, 0x279a40cd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveArrayProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateRemoveArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Key Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(RemoveArray);
    proto->SetBehaviorCallbackFct(RemoveArrayCallBack);

    *pproto = proto;
    return CK_OK;
}

int RemoveArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the array
    CKParameterIn *pin = beh->GetInputParameter(0);
    if (!pin)
        return CKBR_PARAMETERERROR;

    // we get the criterion column
    int column = array->GetKeyColumn();
    if (column < 0)
    {
        behcontext.Context->OutputToConsole("No column criterion defined");
        return CKBR_PARAMETERERROR;
    }

    // we find the key
    CKDWORD key = 0;
    int size = GetInputValue(array, beh, column, 0, key);

    // the insertion itself
    int index = array->FindRowIndex(column, CKEQUAL, key, size);
    if (index < 0)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // the removal itself
    array->RemoveRow(index);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR RemoveArrayCallBack(const CKBehaviorContext &behcontext)
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

        int column = array->GetKeyColumn();
        if (column < 0)
        {
            behcontext.Context->OutputToConsole("No column criterion defined");
            return CKBR_PARAMETERERROR;
        }

        // we create the input parameter
        CreateInputParameter(array, beh, column, 0, "Reference Value");
    }
    break;
    }

    return CKBR_OK;
}
