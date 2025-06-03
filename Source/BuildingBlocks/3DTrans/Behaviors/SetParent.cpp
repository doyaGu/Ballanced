/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetParent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetParentDecl();
CKERROR CreateSetParentProto(CKBehaviorPrototype **pproto);
int SetParent(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetParentDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Parent");
    od->SetDescription("Sets the Parent of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Parent: </SPAN>3D Entity to be set as parent. Use NULL to 'deparent' the entity (which means parenting it to the world).<BR>
    <BR>
    See also: Add Child.<BR>
    */
    /* warning:
    - This building block sets a state on the object. Which means, once it is activated, the state will be kept as it is until a 'SetParent' is called on the child (this can also occur when reseting IC).<BR>
    Therefore, if you want initial hierarchy to be reseted as it was before activating this building block, you'll need to put Initial Conditions (IC) on the child.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9d9d9d98, 0x7e7a7f75));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetParentProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetParentProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Parent");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Parent", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetParent);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetParent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get parent
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(0);

    ent->SetParent(e);

    return CKBR_OK;
}
