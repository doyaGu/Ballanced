/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ChangeTextureSlot
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorChangeTextureSlotDecl();
CKERROR CreateChangeTextureSlotProto(CKBehaviorPrototype **);
int ChangeTextureSlot(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChangeTextureSlotDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Current Slot");
    od->SetDescription("Changes the current slot of the texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Slot Index: </SPAN>index of the new current texture slot.<BR>
    <BR>
    Texture slots are simply different images contained in a texture.<BR>
    See the Setup View for textures.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xaaaa213a, 0xeaa8d52a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChangeTextureSlotProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateChangeTextureSlotProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Set Current Slot");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Slot Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ChangeTextureSlot);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ChangeTextureSlot(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Core
    CKTexture *cktex = (CKTexture *)beh->GetTarget();
    if (!cktex)
        return CKBR_OWNERERROR;

    // we get a int (input parameter)
    int s = 0;
    beh->GetInputParameterValue(0, &s);

    cktex->SetCurrentSlot(s);

    return CKBR_OK;
}
