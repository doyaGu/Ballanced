/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics WakeUp
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsWakeUpDecl();
CKERROR CreatePhysicsWakeUpProto(CKBehaviorPrototype **pproto);
int PhysicsWakeUp(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsWakeUpDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics WakeUp");
    od->SetDescription("Wake Up Object for Simulation");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38b851b5, 0x72ca74ac));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsWakeUpProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsWakeUpProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics WakeUp");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsWakeUp);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

class PhysicsWakeUpCall : public PhysicsCall
{
public:
    PhysicsWakeUpCall(CKIpionManager *pm, CKBehavior *beh) : PhysicsCall(pm, beh, 2) {}

    virtual CKBOOL Execute()
    {
        CK3dEntity *ent = (CK3dEntity *)m_Behavior->GetTarget();
        if (!ent)
            return TRUE;
        PhysicsStruct *ps = m_IpionManager->HasPhysics(ent);
        if (!ps)
            return FALSE;
        ps->m_PhysicsObject->ensure_in_simulation();
    }
};

int PhysicsWakeUp(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsWakeUp: pm==NULL.");
        return CKBR_OK;
    }

    PhysicsWakeUpCall *physicsCall = new PhysicsWakeUpCall(man, beh);
    man->m_PhysicsCallManager->Process(physicsCall);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}