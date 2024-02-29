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

CKObjectDeclaration *FillBehaviorPhysicsCollDetectionDecl()
{
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

CKERROR CreatePhysicsCollDetectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PhysicsCollDetection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

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

#define ENTITY 0
#define SPEED 1
#define COLLISION_NORMAL_WORLD 2
#define POSITION_WORLD 3

class PhysicsCollDetectionListener : public IVP_Listener_Collision
{
public:
    PhysicsCollDetectionListener(float sleepAfterwards, float minSpeed, float maxSpeed,
                                 IVP_Real_Object *obj, CKIpionManager *manager, CKBehavior *beh, int collisionID)
        : IVP_Listener_Collision(IVP_LISTENER_COLLISION_CALLBACK_POST_COLLISION |
                                 IVP_LISTENER_COLLISION_CALLBACK_OBJECT_DELETED),
          m_Time(0),
          m_SleepAfterwards(sleepAfterwards),
          m_MinSpeed(minSpeed),
          m_MaxSpeed(maxSpeed),
          m_RealObject(obj),
          m_IpionManager(manager),
          m_Behavior(beh),
          m_CollisionID(collisionID)
    {
        obj->add_listener_collision(this);
    }

    ~PhysicsCollDetectionListener()
    {
        if (m_RealObject)
            m_RealObject->remove_listener_collision(this);

        PhysicsCollDetectionListener *listener = NULL;
        m_Behavior->SetLocalParameterValue(0, &listener);
    }

    void event_post_collision(IVP_Event_Collision *collision)
    {
        IVP_Contact_Situation *situation = collision->contact_situation;
        IVP_Real_Object *obj = situation->objects[0];
        if (obj == m_RealObject)
            obj = situation->objects[1];

        CK3dEntity *ent = (CK3dEntity *)obj->client_data;

        int collisionID = m_IpionManager->GetCollisionDetectID(ent);

        CKBOOL useCollisionID = FALSE;
        m_Behavior->GetLocalParameterValue(1, &useCollisionID);
        if (useCollisionID && m_CollisionID != collisionID)
            return;

        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent);
        if (!po)
            return;

        if (m_Behavior->IsOutputActive(0) ||
            m_IpionManager->GetSimulationTime() - m_Time < m_SleepAfterwards)
            return;

        double sl = situation->speed.real_length();
        if (sl <= m_MinSpeed)
            return;

        float speed;
        if (sl < 0.0001f)
        {
            speed = 0.0001f;
        }
        else if (m_MaxSpeed < 0.0001f)
        {
            speed = 1.0f;
        }
        else
        {
            speed = (float)sl / m_MaxSpeed;
            if (speed > 1.0f)
                speed = 1.0f;
        }

        m_Behavior->SetOutputParameterObject(ENTITY, ent);
        m_Behavior->SetOutputParameterValue(SPEED, &speed);

        if (situation->objects[0] != m_RealObject)
        {
            situation->surf_normal.mult(-1.0f);
        }

        VxVector collisionNormalWorld(situation->surf_normal.k[0],
                                      situation->surf_normal.k[1],
                                      situation->surf_normal.k[2]);
        m_Behavior->SetOutputParameterValue(COLLISION_NORMAL_WORLD, &collisionNormalWorld);

        VxVector positionWorld((float)situation->contact_point_ws.k[0],
                               (float)situation->contact_point_ws.k[1],
                               (float)situation->contact_point_ws.k[2]);
        m_Behavior->SetOutputParameterValue(POSITION_WORLD, &collisionNormalWorld);

        m_Time = m_IpionManager->GetSimulationTime();

        m_Behavior->ActivateOutput(0, TRUE);
    }

    void event_collision_object_deleted(IVP_Real_Object *obj)
    {
        if (obj == m_RealObject)
        {
            m_RealObject->remove_listener_collision(this);
            m_RealObject = NULL;
        }
    }

    IVP_Time m_Time;
    float m_SleepAfterwards;
    float m_MinSpeed;
    float m_MaxSpeed;
    IVP_Real_Object *m_RealObject;
    CKIpionManager *m_IpionManager;
    CKBehavior *m_Behavior;
    int m_CollisionID;
    int field_2C;
};

class PhysicsCollDetectionCallback : public PhysicsCallback
{
public:
    PhysicsCollDetectionCallback(CKIpionManager *pm, CKBehavior *beh) : PhysicsCallback(pm, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)m_Behavior->GetTarget();
        if (!ent)
            return 1;

        float minSpeed = 0.3f;
        beh->GetInputParameterValue(MIN_SPEED, &minSpeed);

        float maxSpeed = 0.3f;
        beh->GetInputParameterValue(MAX_SPEED, &maxSpeed);

        float sleepAfterwards = 0.5f;
        beh->GetInputParameterValue(SLEEP_AFTERWARDS, &sleepAfterwards);

        int collisionID = 1;
        beh->GetInputParameterValue(COLLISION_ID, &collisionID);

        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent, TRUE);
        if (!po)
            return 0;

        IVP_Real_Object *obj = po->m_RealObject;

        PhysicsCollDetectionListener *listener = new PhysicsCollDetectionListener(sleepAfterwards, minSpeed, maxSpeed,
                                                                                  obj, m_IpionManager, m_Behavior,
                                                                                  collisionID);

        beh->SetLocalParameterValue(0, &listener);

        return 1;
    }
};

int PhysicsCollDetection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    if (beh->IsInputActive(0))
    {
        PhysicsCollDetectionListener *listener = NULL;
        beh->GetLocalParameterValue(0, &listener);
        if (!listener)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            CKIpionManager *man = CKIpionManager::GetManager(context);
            man->GetPhysicsObject(ent, TRUE);

            PhysicsCollDetectionCallback *physicsCall = new PhysicsCollDetectionCallback(man, beh);
            man->m_PreSimulateCallbacks->Process(physicsCall);
        }

        beh->ActivateInput(0, FALSE);
        return CKBR_ACTIVATENEXTFRAME;
    }
    else if (beh->IsInputActive(1))
    {
        PhysicsCollDetectionListener *listener = NULL;
        beh->GetLocalParameterValue(0, &listener);
        if (listener)
        {
            delete listener;
        }

        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR PhysicsCollDetectionCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}
