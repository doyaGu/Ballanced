/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Swap Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKDataArray.h"

CKObjectDeclaration *FillBehaviorArraySwapDecl();
CKERROR CreateArraySwapProto(CKBehaviorPrototype **);
int ArraySwap(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArraySwapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Swap Rows");
    od->SetDescription("Swaps two rows of an array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Row 1: </SPAN>index of the row to swap with row 2.<BR>
    <SPAN CLASS=pin>Row 2: </SPAN>index of the row to swap with row 1.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2ed72740, 0x406b5027));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArraySwapProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArraySwapProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Swap Rows");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Row 1", CKPGUID_INT, "0");
    proto->DeclareInParameter("Row 2", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArraySwap);

    *pproto = proto;
    return CK_OK;
}

int ArraySwap(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OK;

    int l1 = 0;
    beh->GetInputParameterValue(0, &l1);

    int l2 = 1;
    beh->GetInputParameterValue(1, &l2);

    // the swap itself
    array->SwapRows(l1, l2);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
