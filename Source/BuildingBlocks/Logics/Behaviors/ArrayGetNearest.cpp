/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Get Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayGetNearestDecl();
CKERROR CreateArrayGetNearestProto(CKBehaviorPrototype **);
int ArrayGetNearest(const CKBehaviorContext &behcontext);
CKERROR ArrayGetNearestCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayGetNearestDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Nearest");
    od->SetDescription("Gets the element of an array, which value in the given column is the nearest from the reference value.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated when the element is found in the array.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated when the element is not found in the array.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>the column in which the Nearest value must be searched.<BR>
    <SPAN CLASS=pin>(Reference Value): </SPAN>input parameter that will be created when choosing the column and editing the building block
    and that will contain the reference value to test.<BR>
    <BR>
    <SPAN CLASS=pout>Row Number: </SPAN>the index of the row in which the element has been found.<BR>
    <SPAN CLASS=pout>(Element Values): </SPAN>all the elements of the row found are written in these subsequent output parameters.<BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode.
    (The output parameters are only created in author mode, when editing the building block.)
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfac60ad, 0x1bd31ac9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayGetNearestProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayGetNearestProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Nearest");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Column", CKPGUID_INT, "1");

    proto->DeclareOutParameter("Row Number", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArrayGetNearest);
    proto->SetBehaviorCallbackFct(ArrayGetNearestCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayGetNearest(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(0, &column);

    CKDWORD key = NULL;

    // we find the key
    int size = GetInputValue(array, beh, column, 1, key);

    // the search itself
    int row = -1;
    array->GetNearest(column, &key, row);

    CKDataRow *it = array->GetRow(row);

    // we write the row number
    beh->SetOutputParameterValue(0, &row);

    if (!it)
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // we write the array data in the output parameters
    WriteOutputParameters(array, it, beh, 1);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayGetNearestCallBack(const CKBehaviorContext &behcontext)
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
        CreateInputParameter(array, beh, column, 1, "Reference Value");

        // we create the output parameters
        CreateOutputParameters(array, beh, 1);
    }
    break;
    }

    return CKBR_OK;
}
