/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Clear
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorArrayClearDecl();
CKERROR CreateArrayClearProto(CKBehaviorPrototype **);
int ArrayClear(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayClearDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Clear Array");
    od->SetDescription("Removes all the rows of the array.");
    /* rem:
      <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
      <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    This building block removes all the rows of the given array.<BR>
    <BR>
    */
    /* warning:
    - The columns arn't removed. There's simply no more rows.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x35c9352f, 0x7b1a193b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayClearProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayClearProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Clear Array");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayClear);

    *pproto = proto;
    return CK_OK;
}

int ArrayClear(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    array->Clear();

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
