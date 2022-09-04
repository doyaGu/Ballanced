////////////////////////////////
////////////////////////////////
//
//        TT_KeySwitch
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorKeySwitchDecl();
CKERROR CreateKeySwitchProto(CKBehaviorPrototype **pproto);
int KeySwitch(const CKBehaviorContext &behcontext);
CKERROR KeySwitchCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeySwitchDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_KeySwitch");
    od->SetDescription("Waits for a key to be pressed.");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4fbb3808, 0x4a99083c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeySwitchProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateKeySwitchProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_KeySwitch");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out 0");
    proto->DeclareOutput("Out 1");

    proto->DeclareInParameter("Key 0", CKPGUID_KEY);
    proto->DeclareInParameter("Key 1", CKPGUID_KEY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeySwitch);

    proto->SetBehaviorFlags(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS);
    proto->SetBehaviorCallbackFct(KeySwitchCallBack);

    *pproto = proto;
    return CK_OK;
}

int KeySwitch(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR KeySwitchCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}