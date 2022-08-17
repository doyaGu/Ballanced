/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		     Physics Continuous Contact
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsContinuousContactDecl();
CKERROR CreatePhysicsContinuousContactProto(CKBehaviorPrototype **);
int PhysicsContinuousContact(const CKBehaviorContext &behcontext);
CKERROR PhysicsContinuousContactCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsContinuousContactDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("PhysicsContinuousContact");
    od->SetDescription("PhysicsContinuousContact");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x199E4CF1, 0x545A78FE));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsContinuousContactProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsContinuousContactProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PhysicsContinuousContact");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Stop");

    proto->DeclareInParameter("Time Delay Start", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Time Delay End", CKPGUID_FLOAT, "0.1");

    proto->DeclareSetting("Number Group Output", CKPGUID_INT, "5");
    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER, "NULL");

    char buffer[248];
    for (int i = 1; i <= 5; ++i)
    {
        sprintf(buffer, "contact on %d", i);
        proto->DeclareOutput(buffer);
        sprintf(buffer, "contact off %d", i);
        proto->DeclareOutput(buffer);
    }

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsContinuousContact);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(PhysicsContinuousContactCallBack);

    *pproto = proto;
    return CK_OK;
}

#define TIME_DELAY_START 0
#define TIME_DELAY_END 1

int PhysicsContinuousContact(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsContinuousContact: pm==NULL.");
        return CKBR_OK;
    }

    return CKBR_OK;
}

CKERROR PhysicsContinuousContactCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
