/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//					Is In Group
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGroupIteratorDecl();
CKERROR CreateGroupIteratorProto(CKBehaviorPrototype **);
int GroupIterator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGroupIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Group Iterator");
    od->SetDescription("Retrieves each element of a group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Group: </SPAN>the group to parse.<BR>
    <BR>
    <SPAN CLASS=pout>Element: </SPAN>current parsed element of the group.<BR>
    <SPAN CLASS=pout>Index: </SPAN>index of the current element.<BR>
    <BR>
    This building block is useful in many situations. Suppose you want to change the diffuse color of several materials.
    As the "Set Diffuse" building block is targetable, you just have to put all the materials you want the color to be changed in a group,
    and parse all the elements (materials) of this group, and link the "Loop Out" output to the "Set Diffuse" building block.
    The retrieved "Element" will be the current parsed material, and have to be used (linked) as the Target of the "Set Diffuse" building block.<BR>
    But this building block is also useful to find a specifique information amoung a group. For instance, which material use the texture T ? Or which
    is the strongest ennemy amoung them all ? (supposing they have a Ennemy attribut, for example).<BR>
    <BR>
    eg: <BR>
    <IMG SRC="GroupIt.gif"><BR><P>
    See also : Add To Group, Remove From Group.<BR>
    */
    /* warning:
    - Beware, each if you explicitly put a link delay of 1 for looping link,
    this will mean you want to wait 1 behavioral frame each time you parse a material.
    In most case, a link delay of 0 for the looping link must be used.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6050252f, 0x3aa82d40));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGroupIteratorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Groups");
    return od;
}

CKERROR CreateGroupIteratorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Group Iterator");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->DeclareOutParameter("Element", CKPGUID_BEOBJECT);
    proto->DeclareOutParameter("Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GroupIterator);

    *pproto = proto;
    return CK_OK;
}

int GroupIterator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group) //--- I keep this here for backcompatibility...
    {
        beh->ActivateOutput(0);
        return CKBR_PARAMETERERROR;
    } //---

    int index = 0;
    if (beh->GetOutputParameterValue(1, &index))
        beh->GetLocalParameterValue(0, &index);

    if (beh->IsInputActive(0)) // IN
    {
        beh->ActivateInput(0, FALSE);
        index = 0;
    }
    else // LOOP IN
    {
        beh->ActivateInput(1, FALSE);
        ++index;
    }

    if (index >= group->GetObjectCount()) // no more object in group
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    CKBeObject *beo = (CKBeObject *)group->GetObject(index);
    beh->SetOutputParameterObject(0, beo);
    if (beh->SetOutputParameterValue(1, &index))
        beh->SetLocalParameterValue(0, &index);

    beh->ActivateOutput(1);
    return CKBR_OK;
}
