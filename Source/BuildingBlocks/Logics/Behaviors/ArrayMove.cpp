/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Move Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKDataArray.h"

CKObjectDeclaration *FillBehaviorArrayMoveDecl();
CKERROR CreateArrayMoveProto(CKBehaviorPrototype **);
int ArrayMove(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayMoveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Move Row");
    od->SetDescription("Moves a row of the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Src Row: </SPAN>index of the row to move.<BR>
    <SPAN CLASS=pin>Dest Row: </SPAN>index of the row before which to insert the source row. -1 means Src Row will be put at end.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x101a62f4, 0x6e2d25f6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayMoveProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayMoveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Move Row");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Src Row", CKPGUID_INT, "0");
    proto->DeclareInParameter("Dest Row", CKPGUID_INT, "-1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayMove);

    *pproto = proto;
    return CK_OK;
}

int ArrayMove(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OK;

    int l1 = 0;
    beh->GetInputParameterValue(0, &l1);

    int l2 = 1;
    beh->GetInputParameterValue(1, &l2);

    // the Move itself
    array->MoveRow(l1, l2);

    return CKBR_OK;
}
