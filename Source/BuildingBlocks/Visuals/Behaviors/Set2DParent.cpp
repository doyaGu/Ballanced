/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set2DParent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSet2DParentDecl();
CKERROR CreateSet2DParentProto(CKBehaviorPrototype **pproto);
int Set2DParent(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DParentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set 2D Parent");
    od->SetDescription("Sets the Parent of a 2D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Parent: </SPAN>2D Entity to be set as parent. Use NULL to 'deparent' the entity (which means parenting it to the world).<BR>
    <BR>
    */
    /* warning:
    - This building block sets a state on the object. Which means, once it is activated, the state will be kept as it is until a 'Set2DParent' is called on the child (this can also occur when reseting IC).<BR>
    Therefore, if you want initial hierarchy to be reseted as it was before activating this building block, you'll need to put Initial Conditions (IC) on the child.<BR>
    */
    od->SetCategory("Visuals/2D");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x14c70f07, 0x4f2a100a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DParentProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DParentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set 2D Parent");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Parent", CKPGUID_2DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DParent);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Set2DParent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get parent
    CK2dEntity *e = (CK2dEntity *)beh->GetInputParameterObject(0);

    ent->SetParent(e);

    return CKBR_OK;
}
