/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Get Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorGetArrayDecl();
CKERROR CreateGetArrayProto(CKBehaviorPrototype **);
int GetArray(const CKBehaviorContext &behcontext);
CKERROR GetArrayCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetArrayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Key Row");
    od->SetDescription("Gets elements from an array, using the key column as a search criterion on the given key value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when the element is found in the array.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated when the element is not found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>(Key Value): </SPAN>input parameter that will be created when choosing the column and editing the building block
    and that will contain the value to search in the key column.<BR>
    <BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the found row are written in these subsequent output parameters.<BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode.
    (The output parameters are only created in author mode, when editing the building block.)
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x49064205, 0x10e72f7a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetArrayProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateGetArrayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Key Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(GetArray);
    proto->SetBehaviorCallbackFct(GetArrayCallBack);

    *pproto = proto;
    return CK_OK;
}

int GetArray(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    CKDWORD key = NULL;
    int column = array->GetKeyColumn();
    if (column < 0)
    {
        behcontext.Context->OutputToConsole("No column criterion defined");
        return CKBR_PARAMETERERROR;
    }

    // we find the key
    int size = GetInputValue(array, beh, column, 0, key);

    // the getting itself
    CKDataRow *it = array->FindRow(column, CKEQUAL, key, size);

    if (!it)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // we write the array data in the output param
    WriteOutputParameters(array, it, beh, 0);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR GetArrayCallBack(const CKBehaviorContext &behcontext)
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
            ///				return CKBR_PARAMETERERROR;
        }

        // we create the input parameter
        CreateInputParameter(array, beh, column, 0, "Key Value");

        // we create the output parameters
        CreateOutputParameters(array, beh, 0);
    }
    break;
    }

    return CKBR_OK;
}
