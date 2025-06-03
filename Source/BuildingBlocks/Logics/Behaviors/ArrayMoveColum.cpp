/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Move Column
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayMoveColumnDecl();
CKERROR CreateArrayMoveColumnProto(CKBehaviorPrototype **);
int ArrayMoveColumn(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayMoveColumnDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Move Column");
    od->SetDescription("Moves a column of the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Src Column: </SPAN>index of the column to move.<BR>
    <SPAN CLASS=pin>Dest Column: </SPAN>index of the column to insert before. -1 means insert after all the columns.<BR>
    <BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1ace1319, 0x66b112b1));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayMoveColumnProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayMoveColumnProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Move Column");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Src Column", CKPGUID_INT, "0");
    proto->DeclareInParameter("Dest Column", CKPGUID_INT, "-1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(ArrayMoveColumn);

    *pproto = proto;
    return CK_OK;
}

int ArrayMoveColumn(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int c1 = -1;
    beh->GetInputParameterValue(0, &c1);

    int c2 = -1;
    beh->GetInputParameterValue(1, &c2);

    array->MoveColumn(c1, c2);

    return CKBR_OK;
}