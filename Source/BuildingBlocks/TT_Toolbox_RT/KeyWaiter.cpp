/////////////////////////////////
/////////////////////////////////
//
//        TT_Key Waiter
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTKeyWaiterBehaviorProto(CKBehaviorPrototype **pproto);
int TTKeyWaiter(const CKBehaviorContext& behcontext);
CKERROR TTKeyWaiterCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTKeyWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Key Waiter");
    od->SetDescription("Waits for a key to be pressed.");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2ffb3ef0,0x21807ae3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTKeyWaiterBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTKeyWaiterBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Key Waiter");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Key", CKPGUID_KEY);

    proto->DeclareSetting("Wait For Any Key", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTKeyWaiter);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetBehaviorCallbackFct(TTKeyWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTKeyWaiter(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTKeyWaiterCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}