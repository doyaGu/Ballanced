/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Sum
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArraySumDecl();
CKERROR CreateArraySumProto(CKBehaviorPrototype **);
int ArraySum(const CKBehaviorContext &behcontext);
CKERROR ArraySumCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySumDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Column Sum");
    od->SetDescription("Calculates the sum of all the elements of a column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>the column from which the sum must be calculated.<BR>
    <BR>
    <SPAN CLASS=pout>(Sum Value): </SPAN>this output parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    */
    /* warning:
    - This building block can only be used with column containing INTEGER or FLOAT values.
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x49124252, 0x1be909a4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySumProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySumProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Column Sum");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));

    proto->SetFunction(ArraySum);
    proto->SetBehaviorCallbackFct(ArraySumCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArraySum(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(0, &column);

    // the search itself
    CKDWORD sum = array->Sum(column);

    beh->SetOutputParameterValue(0, &sum);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArraySumCallBack(const CKBehaviorContext &behcontext)
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
        int column = 0;
        beh->GetInputParameterValue(0, &column);

        // create the output result parameter
        CreateOutputParameter(array, beh, column, 0);
    }
    break;
    }

    return CKBR_OK;
}
