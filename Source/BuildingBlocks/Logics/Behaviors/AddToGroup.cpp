/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AddToGroup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddToGroupDecl();
CKERROR CreateAddToGroupProto(CKBehaviorPrototype **);
int AddToGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddToGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add To Group");
    od->SetDescription("Adds the object to a Group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>group to which the object will be added.<BR>
    <BR>
    See also: Is In Group, Remove From Group.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00024125, 0x785420ab));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddToGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateAddToGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add To Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AddToGroup);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddToGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBeObject *o = beh->GetTarget();

    // we get the group
    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        return CKBR_PARAMETERERROR;

    group->AddObject(o);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
