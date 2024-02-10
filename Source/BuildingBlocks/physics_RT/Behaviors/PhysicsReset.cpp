/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Reset
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsResetDecl();
CKERROR CreatePhysicsResetProto(CKBehaviorPrototype **pproto);
int PhysicsReset(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsResetDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Reset");
    od->SetDescription("Reset all Physics");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5714345e, 0x792262ae));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsResetProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreatePhysicsResetProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics Reset");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsReset);

    *pproto = proto;
    return CK_OK;
}

class PhysicsResetCall : public PhysicsCallback
{
public:
    PhysicsResetCall(CKIpionManager *pm, CKBehavior *beh) : PhysicsCallback(pm, beh, 2) {}

    virtual int Execute()
    {
        m_IpionManager->Reset();
        return 1;
    }
};

int PhysicsReset(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    PhysicsResetCall *physicsCall = new PhysicsResetCall(man, beh);
    man->m_PreSimulateCallbacks->Process(physicsCall);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}