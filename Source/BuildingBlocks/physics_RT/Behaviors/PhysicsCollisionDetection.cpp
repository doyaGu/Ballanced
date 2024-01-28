/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		     Physics Collision Detection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsCollDetectionDecl();

CKERROR CreatePhysicsCollDetectionProto(CKBehaviorPrototype **pproto);

int PhysicsCollDetection(const CKBehaviorContext &behcontext);

CKERROR PhysicsCollDetectionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsCollDetectionDecl() {
    CKObjectDeclaration *od = CreateCKObjectDeclaration("PhysicsCollDetection");
    od->SetDescription("PhysicsCollDetection");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7435488d, 0x201d1188));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsCollDetectionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsCollDetectionProto(CKBehaviorPrototype **pproto) {
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PhysicsCollDetection");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Collision");

    proto->DeclareInParameter("Min Speed m/s", CKPGUID_FLOAT, "0.3");
    proto->DeclareInParameter("Max Speed m/s", CKPGUID_FLOAT, "10.0");
    proto->DeclareInParameter("Sleep afterwards", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Collision ID", CKPGUID_INT, "1");

    proto->DeclareOutParameter("Entity", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Speed (0-1)", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Collision Normal World", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Position World", CKPGUID_VECTOR);

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->DeclareSetting("Use Collision ID", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsCollDetection);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
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

class PhysicsCollDetectionListener : public IVP_Listener_Collision {
public:
    PhysicsCollDetectionListener(float sleepAfterwards, float minSpeed, float maxSpeed,
                                 IVP_Real_Object *obj, CKIpionManager *manager, CKBehavior *beh, int collisionID)
            : IVP_Listener_Collision(IVP_LISTENER_COLLISION_CALLBACK_POST_COLLISION |
                                     IVP_LISTENER_COLLISION_CALLBACK_FRICTION),
                                     m_CurrentTime(0),
                                     m_SleepAfterwards(sleepAfterwards),
                                     m_MinSpeed(minSpeed),
                                     m_MaxSpeed(maxSpeed),
                                     m_PhysicsObject(obj),
                                     m_IpionManager(manager),
                                     m_Behavior(beh),
                                     m_CollisionID(collisionID)
                                     {}

    void event_pre_collision(IVP_Event_Collision *collision) override {

    }

    void event_friction_deleted(IVP_Event_Friction *friction) override {
        IVP_Listener_Collision::event_friction_deleted(friction);
    }

    double m_CurrentTime;
    float m_SleepAfterwards;
    float m_MinSpeed;
    float m_MaxSpeed;
    IVP_Real_Object *m_PhysicsObject;
    CKIpionManager *m_IpionManager;
    CKBehavior *m_Behavior;
    int m_CollisionID;
    int field_2C;
};

class PhysicsCollDetectionCall : public PhysicsCall {
public:
    PhysicsCollDetectionCall(CKIpionManager *pm, CKBehavior *beh) : PhysicsCall(pm, beh, 2) {}

    virtual CKBOOL Execute() {
        CK3dEntity *ent = (CK3dEntity *) m_Behavior->GetTarget();
        if (!ent)
            return TRUE;

        float minSpeed = 0.3f;
        float maxSpeed = 0.3f;
        float sleepAfterwards = 0.5f;
        int collisionID = 1;
        m_Behavior->GetInputParameterValue(MIN_SPEED, &minSpeed);
        m_Behavior->GetInputParameterValue(MAX_SPEED, &maxSpeed);
        m_Behavior->GetInputParameterValue(SLEEP_AFTERWARDS, &sleepAfterwards);
        m_Behavior->GetInputParameterValue(COLLISION_ID, &collisionID);

        PhysicsStruct *ps = m_IpionManager->HasPhysics(ent, TRUE);
        if (ps) {
            IVP_Real_Object *obj = ps->m_PhysicsObject;
            PhysicsCollDetectionListener *listener = new PhysicsCollDetectionListener(sleepAfterwards, minSpeed, maxSpeed,
                                                                                      obj, m_IpionManager, m_Behavior, collisionID);
            obj->add_listener_collision(listener);
            m_Behavior->SetLocalParameterValue(0, &listener);
        }
        return TRUE;
    }
};

int PhysicsCollDetection(const CKBehaviorContext &behcontext) {
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    if (beh->IsInputActive(0)) {
        void *handle = NULL;
        beh->GetLocalParameterValue(0, &handle);
        if (!handle) {
            CK3dEntity *ent = (CK3dEntity *) beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            CKIpionManager *man = CKIpionManager::GetManager(context);
            man->HasPhysics(ent, TRUE);

            PhysicsCollDetectionCall *physicsCall = new PhysicsCollDetectionCall(man, beh);
            man->m_PhysicsCallManager->Process(physicsCall);
        }

        beh->ActivateInput(0, FALSE);
        return CKBR_ACTIVATENEXTFRAME;
    }

    if (!beh->IsInputActive(1))
        return CKBR_ACTIVATENEXTFRAME;

    beh->ActivateInput(1, FALSE);

    void *handle = NULL;
    beh->GetLocalParameterValue(0, &handle);
    if (handle) {

    }

    return CKBR_OK;
}

CKERROR PhysicsCollDetectionCallBack(const CKBehaviorContext &behcontext) {
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET) {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}
