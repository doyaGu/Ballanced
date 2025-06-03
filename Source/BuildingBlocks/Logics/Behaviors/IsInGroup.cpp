/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					Is In Group
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorIsInGroupDecl();
CKERROR CreateIsInGroupProto(CKBehaviorPrototype **);
int IsInGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsInGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Is In Group");
    od->SetDescription("Check if the object is in a specific group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the Behavioral Object is in the given group.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the  Behavioral Object is not in the given group.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to be tested.<BR>
    <SPAN CLASS=pin>Group: </SPAN>the group.<BR>
    <BR>
    This building block checks weither an object is in a group or not.<BR>
    <BR>
    See also : Add To Group, Remove From Group.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x58a1210b, 0x14e5715c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsInGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateIsInGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Is In Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsInGroup);

    *pproto = proto;
    return CK_OK;
}

int IsInGroup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *obj = (CKBeObject *)beh->GetInputParameterObject(0);
    beh->ActivateInput(0, FALSE);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(1);

    if (CKIsChildClassOf(obj, CKCID_BEOBJECT) && obj->IsInGroup(group))
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    beh->ActivateOutput(1);
    return CKBR_OK;
}
