/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Add Array
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "ArrayGeneralFunctions.h"

CKObjectDeclaration *FillBehaviorArrayRemoveColumnDecl();
CKERROR CreateArrayRemoveColumnProto(CKBehaviorPrototype **);
int ArrayRemoveColumn(const CKBehaviorContext &behcontext);
CKERROR ArrayRemoveColumnCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayRemoveColumnDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Column");
    od->SetDescription("Removes a column of the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Index: </SPAN>index of the column to remove.<BR>
    */
    /* warning:
    - The "Insert Column" isn't supposed to be used in combination with BB such as Iterator BB.
    Because the Iterator BB (as other BB) dynamically change its outputs/inputs when they are edited (right-click + Edit Building Block),
    to match the column numbers and types.<BR>
    If you add a column to your array after having the "Iterator" BB edited, the "Iterator" BB won't be aware of this change, and its outputs/inputs won't be update.<BR>
    Thus the "Insert Column" BB should be used instead with a "GetElement" and "Set Element" BB.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3f377888, 0x128a7767));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayRemoveColumnProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayRemoveColumnProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Column");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Index", CKPGUID_INT, "-1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(ArrayRemoveColumn);

    *pproto = proto;
    return CK_OK;
}

int ArrayRemoveColumn(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    int c = -1;
    beh->GetInputParameterValue(0, &c);

    array->RemoveColumn(c);

    return CKBR_OK;
}