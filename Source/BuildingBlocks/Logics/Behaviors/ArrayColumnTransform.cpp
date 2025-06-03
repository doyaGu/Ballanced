/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array ColumnTransform
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayColumnTransformDecl();
CKERROR CreateArrayColumnTransformProto(CKBehaviorPrototype **);
int ArrayColumnTransform(const CKBehaviorContext &behcontext);
CKERROR ArrayColumnTransformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayColumnTransformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Column Transform");
    od->SetDescription("Performs an operation between one column value and a operand and store the result in the column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Column: </SPAN>index of the column to be transformed by the equation.<BR>
    <SPAN CLASS=pin>Operation: </SPAN>operation to perform between the column and the operand.<BR>
    <SPAN CLASS=pin>(Operand): </SPAN>this input parameter will be created when choosing the column and editing the building block.
    The type of this parameter is choosen according to the type of the given column.<BR>
    <BR>
    */
    /* warning:
    - This building block can only be used with column containing integer or float basic types, not the parameter versions.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f8f0dfe, 0x7c517321));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayColumnTransformProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayColumnTransformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Column Transform");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Operation", CKPGUID_BINARYOPERATOR, "+");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    proto->SetFunction(ArrayColumnTransform);
    proto->SetBehaviorCallbackFct(ArrayColumnTransformCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArrayColumnTransform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int column = 0;
    beh->GetInputParameterValue(0, &column);

    CK_BINARYOPERATOR op = CKADD;
    beh->GetInputParameterValue(1, &op);

    // we find the key
    CKDWORD key = NULL;
    int size = GetInputValue(array, beh, column, 2, key);

    // the transformation itself
    array->ColumnTransform(column, op, key);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}

CKERROR ArrayColumnTransformCallBack(const CKBehaviorContext &behcontext)
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
        CreateInputParameter(array, beh, column, 2, "Operand");
    }
    break;
    }

    return CKBR_OK;
}
