/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Force
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsForceDecl();
CKERROR CreatePhysicsForceProto(CKBehaviorPrototype **pproto);
int PhysicsForce(const CKBehaviorContext &behcontext);
CKERROR PhysicsForceCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsForceDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Force");
    od->SetDescription("Physics Force");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56e20c57, 0xb926068));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsForceProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsForceProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("SetPhysicsForce");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Pos Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Direction Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Force Value", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsForce);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PhysicsForceCallBack);

    *pproto = proto;
    return CK_OK;
}

#define POSITION 0
#define REFERENTIAL 1
#define DIRECTION 2
#define DIRECTION_REF 3
#define FORCE_VALUE 4

int PhysicsForce(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsForceCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
