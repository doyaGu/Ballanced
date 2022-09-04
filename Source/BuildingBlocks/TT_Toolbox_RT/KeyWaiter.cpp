/////////////////////////////////
/////////////////////////////////
//
//        TT_Key Waiter
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorKeyWaiterDecl();
CKERROR CreateKeyWaiterProto(CKBehaviorPrototype **pproto);
int KeyWaiter(const CKBehaviorContext &behcontext);
CKERROR KeyWaiterCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeyWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Key Waiter");
    od->SetDescription("Waits for a key to be pressed.");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2ffb3ef0, 0x21807ae3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeyWaiterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateKeyWaiterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Key Waiter");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Key", CKPGUID_KEY);

    proto->DeclareSetting("Wait For Any Key", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeyWaiter);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetBehaviorCallbackFct(KeyWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int KeyWaiter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR KeyWaiterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}