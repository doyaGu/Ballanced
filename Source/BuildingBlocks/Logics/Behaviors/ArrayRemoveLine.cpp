/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array RemoveLine
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayRemoveLineDecl();
CKERROR CreateArrayRemoveLineProto(CKBehaviorPrototype **);
int ArrayRemoveLine(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayRemoveLineDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Row");
    od->SetDescription("Remove a row of the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Removed: </SPAN>is activated when the row has been removed from the array.<BR>
    <SPAN CLASS=out>Not Present: </SPAN>is activated when the row doesn't exist.<BR>
    <BR>
    <SPAN CLASS=pin>Row Index: </SPAN>index of the row to remove.<BR>
    <BR>
    This building block removes the indexed row of teh array and then exits by the first output if the row index was valid.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1fa57136, 0x14310857));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayRemoveLineProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayRemoveLineProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Removed");
    proto->DeclareOutput("Not Present");

    proto->DeclareInParameter("Row Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayRemoveLine);

    *pproto = proto;
    return CK_OK;
}

int ArrayRemoveLine(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // we get the line
    int line = 0;
    beh->GetInputParameterValue(0, &line);

    if (line < 0 || line >= array->GetRowCount())
    {
        // Set IO states
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // the removal itself
    array->RemoveRow(line);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
