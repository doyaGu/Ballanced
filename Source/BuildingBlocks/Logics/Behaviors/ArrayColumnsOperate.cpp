/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array ColumnsOperate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayColumnsOperateDecl();
CKERROR CreateArrayColumnsOperateProto(CKBehaviorPrototype **);
int ArrayColumnsOperate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayColumnsOperateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Column Operation");
    od->SetDescription("Performs an operation between two columns and put the result in a third one.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Column 1: </SPAN>index of the first source column.<BR>
    <SPAN CLASS=pin>Operation: </SPAN>operation to perform between the two columns.<BR>
    <SPAN CLASS=pin>Column 2: </SPAN>index of the second source column.<BR>
    <SPAN CLASS=pin>Result Column: </SPAN>index of the column receiving the results (It can be one of the two source column) of the equation.<BR>
    <BR>
    This building block performs the operation array[rc][i] = array[c1][i] (op) array[c2][i]
    for every i going from 0 to (number of rows-1) in the array. The operation will
    only work on FLOAT and INTEGER values.<BR>
    */
    /* warning:
    - The operation will only work on integer or float basic types, not the parameter versions.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32351c65, 0x4f6916be));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayColumnsOperateProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayColumnsOperateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Column Operation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column 1", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_BINARYOPERATOR, "+");
    proto->DeclareInParameter("Column 2", CKPGUID_INT, "1");
    proto->DeclareInParameter("Result Column", CKPGUID_INT, "2");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayColumnsOperate);

    *pproto = proto;
    return CK_OK;
}

int ArrayColumnsOperate(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column1 = 0;
    beh->GetInputParameterValue(0, &column1);

    CK_BINARYOPERATOR op = CKADD;
    beh->GetInputParameterValue(1, &op);

    int column2 = 1;
    beh->GetInputParameterValue(2, &column2);

    int columnr = 2;
    beh->GetInputParameterValue(3, &columnr);

    // the transformation itself
    array->ColumnsOperate(column1, op, column2, columnr);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}