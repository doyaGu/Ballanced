/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array SetElement
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArraySetElementDecl();
CKERROR CreateArraySetElementProto(CKBehaviorPrototype **);
int ArraySetElement(const CKBehaviorContext &behcontext);
CKERROR ArraySetElementCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySetElementDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Cell");
    od->SetDescription("Sets the element of a chosen row and column.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Found: </SPAN>is activated if the given cell exists.<BR>
    <SPAN CLASS=out>Not Found: </SPAN>is activated if the given cell doesn't exist.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>the index of the row from which the element must be returned.<BR>
    <SPAN CLASS=pin>Column Index: </SPAN>the index of the column from which the element must be returned.<BR>
    <BR>
    <SPAN CLASS=pin>(Cell Value): </SPAN>the cell of the row found.<BR>
    */
    /* warning:
    - If you change the target array dynamically, make sure it has the same format as the one given in author mode
    (the output parameter are only created in author mode, when editing the building block .)
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30ed1c6d, 0x4a3b7067));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySetElementProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySetElementProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Cell");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Found");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");
    proto->DeclareInParameter("Column Index", CKPGUID_INT, "0");
    proto->DeclareInParameter("Value", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_VARIABLEPARAMETERINPUTS));

    proto->SetFunction(ArraySetElement);
    proto->SetBehaviorCallbackFct(ArraySetElementCallBack);

    *pproto = proto;
    return CK_OK;
}

int ArraySetElement(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the row
    int line = 0;
    beh->GetInputParameterValue(0, &line);

    // we get the column
    int column = 0;
    beh->GetInputParameterValue(1, &column);

    CKParameterIn *pin = beh->GetInputParameter(2);
    CKParameter *pout = pin->GetRealSource();

    // the set itself
    if (!array->SetElementValueFromParameter(line, column, pout))
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

CKERROR ArraySetElementCallBack(const CKBehaviorContext &behcontext)
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

        // we remove
        while (beh->GetInputParameterCount() > 3)
        {
            beh->RemoveInputParameter(3);
        }
    }
    break;
    }

    return CKBR_OK;
}
