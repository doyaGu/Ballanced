/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics WakeUp
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorPhysicsWakeUpDecl();
CKERROR CreatePhysicsWakeUpProto(CKBehaviorPrototype **);
int PhysicsWakeUp(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsWakeUpDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics WakeUp");
    od->SetDescription("Wake Up Object for Simulation");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38B851B5, 0x72CA74AC));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsWakeUpProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsWakeUpProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics WakeUp");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsWakeUp);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int PhysicsWakeUp(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKPhysicsManager *man = CKPhysicsManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsWakeUp: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}