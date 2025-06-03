/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ChangeSpriteSlot
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorChangeSpriteSlotDecl();
CKERROR CreateChangeSpriteSlotProto(CKBehaviorPrototype **pproto);
int ChangeSpriteSlot(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChangeSpriteSlotDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Change Sprite Slot");
    od->SetDescription("Changes the current slot of the Sprite.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>activates the behavior.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Slot Index : </SPAN>index of the new Sprite slot.<BR>
    <BR>
    Slots are simply different images contained in a Sprite.<BR>
    See the Setup View for Sprites.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x70f8028c, 0x723215e7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChangeSpriteSlotProto);
    od->SetCompatibleClassId(CKCID_SPRITE);
    od->SetCategory("Visuals/2D");
    return od;
}

CKERROR CreateChangeSpriteSlotProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Change Sprite Slot");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Slot Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ChangeSpriteSlot);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ChangeSpriteSlot(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Core
    CKSprite *cktex = (CKSprite *)beh->GetTarget();
    if (!cktex)
        return CKBR_OWNERERROR;

    // we get a int (input parameter)
    int s = 0;
    beh->GetInputParameterValue(0, &s);

    cktex->SetCurrentSlot(s);

    return CKBR_OK;
}
