/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Array Reverse
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorArrayReverseDecl();
CKERROR CreateArrayReverseProto(CKBehaviorPrototype **);
int ArrayReverse(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorArrayReverseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Reverse Order");
    od->SetDescription("Reverse the rows order of the array.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
      Reverse the rows order of the array.<BR>
    */
    od->SetCategory("Logics/Array");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3b7d3db7, 0x519f015b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateArrayReverseProto);
    od->SetCompatibleClassId(CKCID_DATAARRAY);
    return od;
}

CKERROR CreateArrayReverseProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Reverse Order");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetFunction(ArrayReverse);

    *pproto = proto;
    return CK_OK;
}

int ArrayReverse(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKDataArray *array = (CKDataArray *)beh->GetTarget();
    if (!array)
        return CKBR_OWNERERROR;

    // the search itself
    array->Reverse();

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
