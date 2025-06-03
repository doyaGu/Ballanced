/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AddChild
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddChildDecl();
CKERROR CreateAddChildProto(CKBehaviorPrototype **);
int AddChild(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddChildDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Child");
    od->SetDescription("Adds a child to the 3D Entity's hierarchy");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Child: </SPAN>3D Entity to be attached as child.<BR>
    <BR>
    Adds a Child to the 3D Entity's hierarchy.<BR>
    See also: Set Parent.<BR>
    */
    /* warning:
    - This building block sets a state on the object. Which means, once it is activated, the state will be kept as it is until a 'SetParent' is called on the child (this can also occur when reseting IC).<BR>
    Therefore, if you want initial hierarchy to be reseted as it was before activating this building block, you'll need to put Initial Conditions (IC) on the child.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45986587, 0x12654556));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddChildProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateAddChildProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Child");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Child", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AddChild);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddChild(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *Parent = (CK3dEntity *)beh->GetTarget();
    if (!Parent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Input Parameter
    CK3dEntity *Child = (CK3dEntity *)beh->GetInputParameterObject(0);

    Parent->AddChild(Child);

    return CKBR_OK;
}
