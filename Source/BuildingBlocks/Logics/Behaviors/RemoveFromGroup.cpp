/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RemoveFromGroup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveFromGroupDecl();
CKERROR CreateRemoveFromGroupProto(CKBehaviorPrototype **);
int RemoveFromGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveFromGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove From Group");
    od->SetDescription("Removes the object from a Group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>group from which the object is to be removed.<BR>
    <BR>
    */
    od->SetCategory("Logics/Groups");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd0147412, 0x0258520d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveFromGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRemoveFromGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove From Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RemoveFromGroup);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RemoveFromGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *o = beh->GetTarget();

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        return CKBR_PARAMETERERROR;

    group->RemoveObject(o);

    return CKBR_OK;
}
