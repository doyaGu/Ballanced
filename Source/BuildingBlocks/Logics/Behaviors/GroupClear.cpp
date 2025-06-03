/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					GroupClear
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGroupClearDecl();
CKERROR CreateGroupClearProto(CKBehaviorPrototype **);
int GroupClear(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGroupClearDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Group Clear");
    od->SetDescription("Remove all the elements of a group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    See also : Add To Group, Remove From Group.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1d12662e, 0x42687a3c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGroupClearProto);
    od->SetCompatibleClassId(CKCID_GROUP);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateGroupClearProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Group Clear");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(GroupClear);

    *pproto = proto;
    return CK_OK;
}

int GroupClear(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKGroup *group = (CKGroup *)beh->GetTarget();
    if (!group)
        return CKBR_PARAMETERERROR;

    group->Clear();

    return CKBR_OK;
}
