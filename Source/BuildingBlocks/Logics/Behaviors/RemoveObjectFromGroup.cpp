/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RemoveObjectFromGroup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRemoveObjectFromGroupDecl();
CKERROR CreateRemoveObjectFromGroupProto(CKBehaviorPrototype **);
int RemoveObjectFromGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRemoveObjectFromGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Remove Object From Group");
    od->SetDescription("Removes the object from a Group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>group from which the object will be removed.<BR>
    <SPAN CLASS=pin>Object: </SPAN>object to remove from the group.<BR>
    <BR>
      */
    /* warning:
    - this building block is OBSOLETE, see 'Remove From Group'.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3c94747a, 0x50882825));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRemoveObjectFromGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateRemoveObjectFromGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Remove Object From Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);
    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(RemoveObjectFromGroup);

    *pproto = proto;
    return CK_OK;
}

int RemoveObjectFromGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBeObject *o = (CKBeObject *)beh->GetInputParameterObject(1);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        return CKBR_PARAMETERERROR;

    group->RemoveObject(o);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
