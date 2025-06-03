/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AddObjectToGroup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddObjectToGroupDecl();
CKERROR CreateAddObjectToGroupProto(CKBehaviorPrototype **);
int AddObjectToGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddObjectToGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Object To Group");
    od->SetDescription("Adds the Object to a Group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>group to which the object will be added.<BR>
    <SPAN CLASS=pin>Object: </SPAN>object to add to the group.<BR>
    <BR>
    See also : Add To Group.<BR>
    */
    /* warning:
    - this building block is obsolete : use Add To Group instead.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x69aa320b, 0x23829c7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddObjectToGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateAddObjectToGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Object To Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);
    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(AddObjectToGroup);

    *pproto = proto;
    return CK_OK;
}

int AddObjectToGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBeObject *o = (CKBeObject *)beh->GetInputParameterObject(1);

    // we get the group
    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        return CKBR_PARAMETERERROR;

    group->AddObject(o);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
