/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		     Physics Collision Detection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorPhysicsCollDetectionDecl();
CKERROR CreatePhysicsCollDetectionProto(CKBehaviorPrototype **);
int PhysicsCollDetection(const CKBehaviorContext &behcontext);
CKERROR PhysicsCollDetectionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsCollDetectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics CollDetection");
    od->SetDescription("Physics Collision Detection");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7435488D, 0x201D1188));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsCollDetectionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsCollDetectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics CollDetection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Collision");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->DeclareInParameter("Min Speed m/s", CKPGUID_FLOAT, "0.3");
    proto->DeclareInParameter("Max Speed m/s", CKPGUID_FLOAT, "10.0");
    proto->DeclareInParameter("Sleep afterwards", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Collision ID", CKPGUID_INT, "1");
    proto->DeclareInParameter("Entity", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Speed (0-1)", CKPGUID_FLOAT);
    proto->DeclareInParameter("Collision Normal World", CKPGUID_VECTOR);
    proto->DeclareInParameter("Position World", CKPGUID_VECTOR);

    proto->DeclareSetting("Use Collision ID", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsCollDetection);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PhysicsCollDetectionCallBack);

    *pproto = proto;
    return CK_OK;
}

#define MIN_SPEED 0
#define MAX_SPEED 1
#define SLEEP_AFTERWARDS 2
#define COLLISION_ID 3
#define ENTITY 4
#define SPEED 5
#define COLLISION_NORMAL_WORLD 6
#define POSITION_WORLD 7

int PhysicsCollDetection(const CKBehaviorContext &behcontext)
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
        context->OutputToConsoleExBeep("TT_PhysicsCollDetection: pm==NULL.");
        return CKBR_OK;
    }

    return CKBR_OK;
}

CKERROR PhysicsCollDetectionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
