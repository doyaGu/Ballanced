#include "CKIpionManager.h"

#include "CKTimeManager.h"
#include "CKMesh.h"
#include "CK3dEntity.h"
#include "CK3dObject.h"
#include "CKBehavior.h"
#include "CKParameterOut.h"
#include "VxMatrix.h"

#include "ivp_time.hxx"
#include "ivp_surman_polygon.hxx"
#include "ivp_compact_surface.hxx"

PhysicsCollisionListener::PhysicsCollisionListener(CKIpionManager *man)
    : IVP_Listener_Collision(IVP_LISTENER_COLLISION_CALLBACK_POST_COLLISION |
                             IVP_LISTENER_COLLISION_CALLBACK_FRICTION),
      m_IpionManager(man) {}

void PhysicsCollisionListener::event_friction_created(IVP_Event_Friction *friction)
{
    if (!friction)
        return;

    IVP_Contact_Situation *situation = friction->contact_situation;

    CK3dEntity *entity1 = (CK3dEntity *)situation->objects[0]->client_data;
    if (!entity1)
        return;

    PhysicsObject *po1 = m_IpionManager->GetPhysicsObject(entity1);
    if (!po1)
        return;

    if (po1->m_FrictionCount == 0)
        po1->m_FrictionTime = friction->environment->get_current_time();
    ++po1->m_FrictionCount;

    CK3dEntity *entity2 = (CK3dEntity *)situation->objects[1]->client_data;
    if (!entity2)
        return;

    PhysicsObject *po2 = m_IpionManager->GetPhysicsObject(entity2);
    if (!po2)
        return;

    if (po2->m_FrictionCount == 0)
        po2->m_FrictionTime = friction->environment->get_current_time();
    ++po2->m_FrictionCount;
}

void PhysicsCollisionListener::event_friction_deleted(IVP_Event_Friction *friction)
{
    if (!friction)
        return;

    IVP_Contact_Situation *situation = friction->contact_situation;

    CK3dEntity *entity1 = (CK3dEntity *)situation->objects[0]->client_data;
    if (!entity1)
        return;

    PhysicsObject *po1 = m_IpionManager->GetPhysicsObject(entity1);
    if (!po1)
        return;

    --po1->m_FrictionCount;

    CK3dEntity *entity2 = (CK3dEntity *)situation->objects[1]->client_data;
    if (!entity2)
        return;

    PhysicsObject *po2 = m_IpionManager->GetPhysicsObject(entity2);
    if (!po2)
        return;

    --po2->m_FrictionCount;
}

PhysicsObjectListener::PhysicsObjectListener(CKIpionManager *man) : m_IpionManager(man) {}

void PhysicsObjectListener::event_object_deleted(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;

    IVP_Core *core = obj->get_core();
    if (!core->physical_unmoveable)
    {
        int i = m_IpionManager->m_MovableObjects.index_of(obj);
        if (i != -1)
            m_IpionManager->m_MovableObjects.remove_at(i);
    }

    CK3dEntity *entity = (CK3dEntity *)obj->client_data;
    if (!entity)
        return;

    PhysicsObject *po = m_IpionManager->GetPhysicsObject(entity);
    if (po && po->m_ContactData)
    {
        delete po->m_ContactData;
        po->m_ContactData = NULL;
    }
}

void PhysicsObjectListener::event_object_created(IVP_Event_Object *object) {}

void PhysicsObjectListener::event_object_revived(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_IpionManager->m_MovableObjects.add(obj);
}

void PhysicsObjectListener::event_object_frozen(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_IpionManager->m_MovableObjects.remove(obj);
}

void PhysicsContactManager::Setup()
{
    CKAttributeManager *am = m_IpionManager->m_Context->GetAttributeManager();
    m_ContactIDAttributeType = am->GetAttributeTypeByName("Continuous Contact ID");
}

void PhysicsContactManager::Process(IVP_Time time)
{
    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        PhysicsObject *po = record->m_PhysicsObject;
        PhysicsContactData *data = po->m_ContactData;

        double delta = time - record->m_Time;
        int id = record->m_ID;
        PhysicsContactData::GroupOutput *output = data->m_GroupOutputs;

        if (output[id].active)
        {
            if (output[id].number == 0 && delta > data->m_TimeDelayEnd)
            {
                output[id].active = FALSE;
                data->m_Behavior->ActivateOutput(2 * id + 1, TRUE);
            }

            m_Records.remove_at(i);
            delete record;
        }
        else
        {
            if (output[id].number == 0 && data->m_TimeDelayStart < delta * 0.5)
            {
                m_Records.remove_at(i);
                delete record;
            }
            else if (data->m_TimeDelayStart < delta)
            {
                output[id].active = TRUE;
                data->m_Behavior->ActivateOutput(2 * id, TRUE);
                m_Records.remove_at(i);
                delete record;
            }
        }
    }
}

int PhysicsContactManager::GetRecordCount() const
{
    return m_Records.len();
}

void PhysicsContactManager::AddRecord(PhysicsObject *obj, int id, IVP_Time time)
{
    bool found = false;
    const int len = m_Records.len();
    for (int i = 0; i < len; ++i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj && record->m_ID == id)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        PhysicsContactRecord *record = new PhysicsContactRecord;
        record->m_PhysicsObject = obj;
        record->m_ID = id;
        record->m_Time = time;
        m_Records.add(record);
    }
}

void PhysicsContactManager::RemoveRecord(PhysicsObject *obj)
{
    if (!obj)
        return;

    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj)
        {
            m_Records.remove_at(i);
            delete record;
        }
    }
}

void PhysicsContactManager::RemoveRecord(PhysicsObject *obj, int id)
{
    if (!obj)
        return;

    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj && record->m_ID == id)
        {
            m_Records.remove_at(i);
            delete record;
        }
    }
}

PhysicsContactData::PhysicsContactData(float timeDelayStart, float timeDelayEnd,
                                       PhysicsContactManager *man, CKBehavior *beh)
    : m_TimeDelayStart(timeDelayStart), m_TimeDelayEnd(timeDelayEnd), m_Manager(man), m_Behavior(beh)
{
    m_GroupOutputs = new GroupOutput[m_Manager->m_NumberGroupOutput];
    m_Listener = NULL;
}

PhysicsContactData::~PhysicsContactData()
{
    CKBehavior *beh = m_Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return;

    PhysicsObject *po = m_Manager->m_IpionManager->GetPhysicsObject(ent);
    m_Manager->RemoveRecord(po);

    PhysicsContactData *data = NULL;
    beh->SetLocalParameterValue(1, &data);

    const int count = m_Manager->m_NumberGroupOutput;
    for (int i = 0; i < count; ++i)
    {
        GroupOutput *output = &m_GroupOutputs[i];
        if (output->active == TRUE)
        {
            output->active = FALSE;
            beh->ActivateOutput(2 * i + 1, TRUE);
        }
    }

    delete[] m_GroupOutputs;
}

CKIpionManager::CKIpionManager(CKContext *context)
    : CKBaseManager(context, TT_PHYSICS_MANAGER_GUID, "TT Physics Manager")
{
    m_HasPhysicsCalls = 0;
    m_PhysicalizeCalls = 0;
    m_DePhysicalizeCalls = 0;

    m_PerformanceCounter.pcount(IVP_PE_PSI_START);

    m_CollisionFilterExclusivePair = NULL;
    m_PreSimulateCallbacks = NULL;
    m_PostSimulateCallbacks = NULL;
    m_CollisionListener = NULL;
    m_ObjectListener = NULL;
    field_30 = 0;
    m_Environment = NULL;
    m_TimeManager = NULL;
    m_DeltaTime = 0.0f;
    m_PhysicsDeltaTime = 0.0f;

    if (context->RegisterNewManager(this) == CKERR_MANAGERALREADYEXISTS)
        ::OutputDebugStringA("Manager already exists");

    m_SurfaceManagers = NULL;
    m_CollisionDetectionID = -1;
}

CKIpionManager::~CKIpionManager() {}

CKERROR CKIpionManager::OnCKInit()
{
    m_SurfaceManagers = new IVP_U_String_Hash(4);

    m_TimeManager = m_Context->GetTimeManager();
    m_PhysicsTimeFactor = 0.001f;
    m_PerformanceCounter.start_pcount();

    return CK_OK;
}

CKERROR CKIpionManager::OnCKEnd()
{
    DestroyEnvironment();
    DeleteCollisionSurfaces();

    m_TimeManager = NULL;

    return CK_OK;
}

CKERROR CKIpionManager::OnCKPlay()
{
    if (m_Context->IsReseted())
        CreateEnvironment();

    return CK_OK;
}

CKERROR CKIpionManager::OnCKReset()
{
    DestroyEnvironment();

    m_PhysicsTimeFactor = 0.001f;
    m_PerformanceCounter.start_pcount();
    m_PhysicsObjects.Clear();

    return CK_OK;
}

CKERROR CKIpionManager::PostClearAll()
{
    DeleteCollisionSurfaces();

    const int len = m_LiquidSurfaces.len();
    for (int i = len - 1; i >= 0; --i)
    {
        IVP_Liquid_Surface_Descriptor_Simple *surface = m_LiquidSurfaces.element_at(i);
        m_LiquidSurfaces.remove_at(i);
        delete surface;
    }

    if (m_SurfaceManagers)
        delete m_SurfaceManagers;
    m_SurfaceManagers = new IVP_U_String_Hash(64);

    return CK_OK;
}

CKERROR CKIpionManager::PostProcess()
{
    float delta = m_TimeManager->GetLastDeltaTime();
    SetDeltaTime(delta);

    if (m_Environment)
    {
        if (m_PreSimulateCallbacks->m_HasCallbacks)
            m_PreSimulateCallbacks->Process();

        m_Environment->simulate_dtime(m_PhysicsDeltaTime);

        m_ContactManager->Process(m_Environment->get_current_time());

        if (m_PostSimulateCallbacks->m_HasCallbacks)
            m_PostSimulateCallbacks->Process();

        IVP_PerformanceCounter_Simple *pc = (IVP_PerformanceCounter_Simple *)m_Environment->get_performancecounter();
        m_PerformanceCounter.counter[IVP_PE_PSI_UNIVERSE][0] += pc->counter[IVP_PE_PSI_UNIVERSE][0];
        m_PerformanceCounter.counter[IVP_PE_PSI_CONTROLLERS][0] += pc->counter[IVP_PE_PSI_CONTROLLERS][0];
        m_PerformanceCounter.counter[IVP_PE_PSI_INTEGRATORS][0] += pc->counter[IVP_PE_PSI_INTEGRATORS][0];
        m_PerformanceCounter.counter[IVP_PE_PSI_HULL][0] += pc->counter[IVP_PE_PSI_HULL][0];
        m_PerformanceCounter.counter[IVP_PE_PSI_SHORT_MINDISTS][0] += pc->counter[IVP_PE_PSI_SHORT_MINDISTS][0];
        m_PerformanceCounter.counter[IVP_PE_PSI_CRITICAL_MINDISTS][0] += pc->counter[IVP_PE_PSI_CRITICAL_MINDISTS][0];

        pc->reset_and_print_performance_counters(m_Environment->get_current_time());

        const int len = m_MovableObjects.len();
        for (int i = len - 1; i >= 0; --i)
        {
            IVP_Real_Object *obj = m_MovableObjects.element_at(i);
            UpdateObjectWorldMatrix(obj);
        }
    }

    return CK_OK;
}

void CKIpionManager::Reset()
{
    DestroyEnvironment();

    m_PhysicsTimeFactor = 0.001f;
    m_PerformanceCounter.start_pcount();
    m_PhysicsObjects.Clear();

    CreateEnvironment();
}

int CKIpionManager::GetPhysicsObjectCount() const
{
    return m_PhysicsObjects.Size();
}

PhysicsObject *CKIpionManager::GetPhysicsObject(CK3dEntity *entity, CKBOOL logging)
{
    if (!entity)
        return NULL;

    LARGE_INTEGER begin;
    ::QueryPerformanceCounter(&begin);

    ++m_HasPhysicsCalls;

    PhysicsObject *obj = NULL;
    PhysicsObjectTable::Iterator it = m_PhysicsObjects.Find(entity->GetID());
    if (it == m_PhysicsObjects.End())
    {
        if (logging)
            m_Context->OutputToConsoleEx("You must Physicalize %s ...", entity->GetName());
    }
    else
    {
        obj = &*it;
    }

    LARGE_INTEGER end;
    ::QueryPerformanceCounter(&end);

    m_DePhysicalizeTime.QuadPart = end.QuadPart - begin.QuadPart;
    if (m_DePhysicalizeTime.QuadPart > m_HasPhysicsTime.QuadPart)
        m_HasPhysicsTime.QuadPart = m_DePhysicalizeTime.QuadPart;

    return obj;
}

void CKIpionManager::RemovePhysicsObject(CK3dEntity *entity)
{
    if (entity)
        m_PhysicsObjects.Remove(entity->GetID());
}

int CKIpionManager::CreatePhysicsObjectOnParameters(CK3dEntity *target, int convexCount, CKMesh **convexes,
                                                    int ballCount, int concaveCount, CKMesh **concaves,
                                                    float ballRadius, CKSTRING collisionSurface,
                                                    VxVector *shiftMassCenter, CKBOOL fixed, IVP_Material *material,
                                                    float mass, CKSTRING collisionGroup, CKBOOL startFrozen,
                                                    CKBOOL enableCollision, CKBOOL autoCalcMassCenter,
                                                    float linearSpeedDampening, float rotSpeedDampening)
{
    VxVector scale;
    target->GetScale(&scale);

    IVP_Real_Object *obj = NULL;

    if (ballCount != 0 || !collisionSurface || collisionSurface[0] == '\0')
    {
        if (!collisionSurface || collisionSurface[0] == '\0')
            enableCollision = FALSE;

        obj = CreatePhysicsBall(target->GetName(), mass, ballRadius, material, linearSpeedDampening, rotSpeedDampening,
                                target, startFrozen, fixed, collisionGroup, enableCollision, shiftMassCenter);
    }
    else
    {
        IVP_SurfaceManager *surman = GetSurfaceManager(collisionSurface);
        if (!surman)
        {
            IVP_SurfaceBuilder_Ledge_Soup builder;
            int ledgeCount = 0;

            if (convexCount > 0)
            {
                for (int i = 0; i < convexCount; i++)
                {
                    if (convexes[i])
                    {
                        ledgeCount += AddConvexSurface(&builder, convexes[i], &scale);
                    }
                }
            }

            if (concaveCount > 0)
            {
                for (int i = 0; i < concaveCount; i++)
                {
                    if (concaves[i])
                    {
                        AddConcaveSurface(&builder, concaves[i], &scale);
                        ++ledgeCount;
                    }
                }
            }

            if (ledgeCount == 0)
            {
                m_Context->OutputToConsoleEx("Error: incorrect mesh for %s !\n", target->GetName());
            }
            else
            {
                IVP_Compact_Surface *compactSurface = builder.compile();
                surman = new IVP_SurfaceManager_Polygon(compactSurface);
                AddSurfaceManager(collisionSurface, surman);
            }
        }

        obj = CreatePhysicsPolygon(target->GetName(), mass, material, linearSpeedDampening, rotSpeedDampening, target,
                                   startFrozen, fixed, collisionGroup, enableCollision, surman, shiftMassCenter);
    }

    obj->client_data = target;

    PhysicsObject po;
    po.m_RealObject = obj;
    m_PhysicsObjects.Insert(target->GetID(), po);

    return CK_OK;
}

IVP_Ball *CKIpionManager::CreatePhysicsBall(CKSTRING name, float mass, float ballRadius, IVP_Material *material,
                                            float linearSpeedDampening, float rotSpeedDampening,
                                            CK3dEntity *target, CKBOOL startFrozen, CKBOOL fixed,
                                            CKSTRING collisionGroup, CKBOOL enableCollision, VxVector *shiftMassCenter)
{
    IVP_Template_Real_Object objectTemplate;
    IVP_U_Point position;
    IVP_U_Quat orientation;
    IVP_U_Matrix massCenter;

    FillTemplateInfo(&objectTemplate, &position, &orientation, name, mass, material, linearSpeedDampening, rotSpeedDampening,
                     target, fixed, collisionGroup, &massCenter, shiftMassCenter);

    IVP_Template_Ball ballTemplate;
    ballTemplate.radius = ballRadius;

    IVP_Ball *ball = m_Environment->create_ball(&ballTemplate, &objectTemplate, &orientation, &position);
    if (ball)
    {
        if (!startFrozen)
            ball->ensure_in_simulation();
        if (enableCollision)
            ball->enable_collision_detection();
    }

    return ball;
}

IVP_Polygon *CKIpionManager::CreatePhysicsPolygon(CKSTRING name, float mass, IVP_Material *material,
                                                  float linearSpeedDampening, float rotSpeedDampening,
                                                  CK3dEntity *target, CKBOOL startFrozen, CKBOOL fixed,
                                                  CKSTRING collisionGroup, CKBOOL enableCollision,
                                                  IVP_SurfaceManager *surman, VxVector *shiftMassCenter)
{
    IVP_Template_Real_Object objectTemplate;
    IVP_U_Point position;
    IVP_U_Quat orientation;
    IVP_U_Matrix massCenter;

    FillTemplateInfo(&objectTemplate, &position, &orientation, name, mass, material, linearSpeedDampening, rotSpeedDampening,
                     target, fixed, collisionGroup, &massCenter, shiftMassCenter);

    IVP_Polygon *polygon = m_Environment->create_polygon(surman, &objectTemplate, &orientation, &position);
    if (polygon)
    {
        if (!startFrozen)
            polygon->ensure_in_simulation();
        if (enableCollision)
            polygon->enable_collision_detection();
    }

    return polygon;
}

void CKIpionManager::CreateEnvironment()
{
    IVP_Application_Environment appEnv;
    appEnv.material_manager = new IVP_Material_Manager(IVP_TRUE);
    appEnv.env_active_float_manager = new IVP_U_Active_Value_Manager(IVP_TRUE);
    appEnv.performancecounter = new IVP_PerformanceCounter_Simple();
    appEnv.env_active_float_manager = new IVP_U_Active_Value_Manager(IVP_TRUE);

    IVP_Collision_Filter_Coll_Group_Ident *groupCollisionFilter = new IVP_Collision_Filter_Coll_Group_Ident(IVP_TRUE);
    m_CollisionFilterExclusivePair = new IVP_Collision_Filter_Exclusive_Pair;

    IVP_Meta_Collision_Filter *collisionFilter = new IVP_Meta_Collision_Filter(IVP_TRUE);
    collisionFilter->add_collision_filter(m_CollisionFilterExclusivePair);
    collisionFilter->add_collision_filter(groupCollisionFilter);
    appEnv.collision_filter = collisionFilter;

    IVP_Environment_Manager *envManager = IVP_Environment_Manager::get_environment_manager();
    m_Environment = envManager->create_environment(&appEnv, "NeMo", 0x7EFAD621);

    IVP_U_Point gravity = IVP_U_Point(0.0, -9.81, 0.0);
    m_Environment->set_gravity(&gravity);

    m_PreSimulateCallbacks = new PhysicsCallbackContainer(this);
    m_PostSimulateCallbacks = new PhysicsCallbackContainer(this);

    m_CollisionListener = new PhysicsCollisionListener(this);
    m_Environment->add_listener_collision_global(m_CollisionListener);

    m_ObjectListener = new PhysicsObjectListener(this);
    m_Environment->add_listener_object_global(m_ObjectListener);

    m_ContactManager = new PhysicsContactManager(this);
    m_ContactManager->Setup();
    SetupCollisionDetectID();
}

void CKIpionManager::DestroyEnvironment()
{
    m_CollisionFilterExclusivePair = NULL;

    if (m_PreSimulateCallbacks)
    {
        delete m_PreSimulateCallbacks;
        m_PreSimulateCallbacks = NULL;
    }

    if (m_PostSimulateCallbacks)
    {
        delete m_PostSimulateCallbacks;
        m_PostSimulateCallbacks = NULL;
    }

    if (m_ObjectListener)
    {
        m_Environment->remove_listener_object_global(m_ObjectListener);

        delete m_ObjectListener;
        m_ObjectListener = NULL;
    }

    if (m_CollisionListener)
    {
        m_Environment->remove_listener_collision_global(m_CollisionListener);

        delete m_CollisionListener;
        m_CollisionListener = NULL;
    }

    for (int i = m_Entities.len() - 1; i >= 0; --i)
    {
        CK3dEntity *entity = m_Entities.element_at(i);
        m_Context->DestroyObject(entity);
    }
    m_Entities.clear();

    for (int i = m_Materials.len() - 1; i >= 0; --i)
    {
        IVP_Material *material = m_Materials.element_at(i);
        delete material;
    }
    m_Materials.clear();

    if (m_Environment)
    {
        delete m_Environment;
        m_Environment = NULL;
    }
}

IVP_Time CKIpionManager::GetSimulationTime()
{
    return m_Environment->get_current_time();
}

float CKIpionManager::GetSimulationTimeStep()
{
    return m_Environment->get_delta_PSI_time();
}

void CKIpionManager::SetSimulationTimeStep(float step)
{
    m_Environment->set_delta_PSI_time(step);
}

void CKIpionManager::SetDeltaTime(float delta)
{
    float time = (m_DeltaTime * 3.0f + delta) / 4;
    m_DeltaTime = time;
    m_PhysicsDeltaTime = time * m_PhysicsTimeFactor;
}

void CKIpionManager::SetTimeFactor(float factor)
{
    m_PhysicsTimeFactor = factor * 0.001f;
}

void CKIpionManager::SetGravity(const VxVector &gravity)
{
    IVP_U_Point gravityPoint(gravity.x, gravity.y, gravity.z);
    m_Environment->set_gravity(&gravityPoint);
}

IVP_SurfaceManager *CKIpionManager::GetSurfaceManager(CKSTRING collisionSurface) const
{
    return (IVP_SurfaceManager *)m_SurfaceManagers->find(collisionSurface);
}

void CKIpionManager::AddSurfaceManager(const CKSTRING collisionSurface, IVP_SurfaceManager *surman)
{
    m_SurfaceManagers->add(collisionSurface, surman);
}

void CKIpionManager::SetupCollisionDetectID()
{
    m_CollisionDetectionID = -1;

    bool found = false;
    CKAttributeManager *am = m_Context->GetAttributeManager();
    const XObjectPointerArray &array = m_Context->GetObjectListByType(CKCID_3DOBJECT, FALSE);
    for (XObjectPointerArray::Iterator it = array.Begin(); it != array.End(); ++it)
    {
        if (found)
            break;

        CK3dObject *obj = (CK3dObject *)*it;
        const int count = obj->GetAttributeCount();
        for (int i = 0; i < count; ++i)
        {
            int type = obj->GetAttributeType(i);
            CKSTRING typeName = am->GetAttributeNameByType(type);
            if (strcmp(typeName, "Coll Detection ID") == 0)
            {
                CKParameterOut *pa = obj->GetAttributeParameter(type);
                pa->GetValue(&m_CollisionDetectionID);
                found = true;
                break;
            }
        }
    }
}

void CKIpionManager::DeleteCollisionSurfaces()
{
    delete m_SurfaceManagers;
    m_SurfaceManagers = NULL;
}

void CKIpionManager::ClearCollisionSurfaces()
{
    DeleteCollisionSurfaces();
    m_SurfaceManagers = new IVP_U_String_Hash(64);
}

void CKIpionManager::ResetProfiler()
{
    QueryPerformanceFrequency(&m_ProfilerCounter);
    memset(&m_HasPhysicsTime, 0, sizeof(LARGE_INTEGER));
    memset(&m_DePhysicalizeTime, 0, sizeof(LARGE_INTEGER));
    memset(&field_FC, 0, sizeof(LARGE_INTEGER));
    memset(&field_104, 0, sizeof(LARGE_INTEGER));
    m_HasPhysicsCalls = 0;
    m_PhysicalizeCalls = 0;
    m_DePhysicalizeCalls = 0;
}

void CKIpionManager::FillTemplateInfo(IVP_Template_Real_Object *templ, IVP_U_Point *position, IVP_U_Quat *orientation,
                                      CKSTRING name, float mass, IVP_Material *material, float linearSpeedDampening,
                                      float rotSpeedDampening, CK3dEntity *target, CKBOOL fixed,
                                      CKSTRING collisionGroup, IVP_U_Matrix *massCenterMatrix,
                                      VxVector *shiftMassCenter)
{
    templ->mass = mass;
    templ->material = material;
    templ->physical_unmoveable = fixed ? IVP_TRUE : IVP_FALSE;
    templ->set_name(name);
    templ->rot_inertia_is_factor = IVP_TRUE;
    templ->rot_speed_damp_factor.set(rotSpeedDampening, rotSpeedDampening, rotSpeedDampening);
    templ->speed_damp_factor = linearSpeedDampening;
    templ->set_nocoll_group_ident(collisionGroup);
    if (shiftMassCenter)
    {
        IVP_U_Point offset(shiftMassCenter->x, shiftMassCenter->y, shiftMassCenter->z);
        massCenterMatrix->init();
        massCenterMatrix->shift_os(&offset);
        templ->mass_center_override = massCenterMatrix;
    }

    VxMatrix mat = target->GetWorldMatrix();
    float m10 = mat[1][0];
    mat[1][0] = mat[0][1];
    mat[0][1] = m10;
    float m20 = mat[2][0];
    mat[2][0] = mat[0][2];
    mat[0][2] = m20;
    float m21 = mat[2][1];
    mat[2][1] = mat[1][2];
    mat[1][2] = m21;

    position->k[0] = mat[3][0];
    position->k[1] = mat[3][1];
    position->k[2] = mat[3][2];

    VxQuaternion quat;
    quat.FromMatrix(mat);

    orientation->x = quat.x;
    orientation->y = quat.y;
    orientation->z = quat.z;
    orientation->w = quat.w;
}

int CKIpionManager::AddConvexSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale)
{
    if (!builder)
        return 0;

    VxVector s = (scale) ? *scale : VxVector(1.0f, 1.0f, 1.0f);

    const int vertexCount = convex->GetVertexCount();
    CKDWORD stride;
    VxVector *pos = (VxVector *)convex->GetPositionsPtr(&stride);

    VxVector **vertices = new VxVector *[vertexCount];

    int count = 0;
    VxVector **ptr = vertices;
    for (int j = 0; j < vertexCount; ++j, pos = (VxVector *)((CKBYTE *)pos + stride))
    {
        // Filter out duplicate vertices
        int i;
        for (i = 0; i < count; ++i)
        {
            if (*pos == *vertices[i])
                break;
        }
        if (i == count)
        {
            *ptr++ = pos;
            ++count;
        }
    }

    IVP_U_Vector<IVP_U_Point> points(count);
    IVP_U_Point *pts = new IVP_U_Point[count];
    for (int p = 0; p < count; ++p)
    {
        VxVector t = *vertices[p] * s;
        pts[p].set(t.x, t.y, t.z);

        points.add(&pts[p]);
    }

    delete[] vertices;

    IVP_Compact_Ledge *ledge = IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge(&points);
    int ret = 0;
    if (ledge)
    {
        builder->insert_ledge(ledge);
        ret = 1;
    }

    delete[] pts;
    return ret;
}

void CKIpionManager::AddConcaveSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *concave, VxVector *scale)
{
    if (!builder)
        return;

    VxVector s = (scale) ? *scale : VxVector(1.0f, 1.0f, 1.0f);

    VxVector vertex1, vertex2, vertex3;
    IVP_U_Vector<IVP_U_Point> points(3);
    IVP_U_Point point1, point2, point3;
    points.add(&point1);
    points.add(&point2);
    points.add(&point3);

    const int faceCount = concave->GetFaceCount();
    for (int i = 0; i < faceCount; ++i)
    {
        int vi1, vi2, vi3;
        concave->GetFaceVertexIndex(i, vi1, vi2, vi3);
        concave->GetVertexPosition(vi1, &vertex1);
        concave->GetVertexPosition(vi2, &vertex2);
        concave->GetVertexPosition(vi3, &vertex3);
        VxVector t1 = vertex1 * s;
        VxVector t2 = vertex2 * s;
        VxVector t3 = vertex3 * s;
        point1.set(t1.x, t1.y, t1.z);
        point2.set(t2.x, t2.y, t2.z);
        point3.set(t3.x, t3.y, t3.z);

        IVP_Compact_Ledge *ledge = IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge(&points);
        if (ledge)
            builder->insert_ledge(ledge);
    }
}

void CKIpionManager::UpdateObjectWorldMatrix(IVP_Real_Object *obj)
{
    if (!obj)
        return;

    CK3dEntity *ent = (CK3dEntity *)obj->client_data;

    IVP_U_Matrix mat;
    obj->get_m_world_f_object_AT(&mat);

    VxMatrix m;
    for (int i = 3; i >= 0; --i)
        for (int j = 3; j >= 0; --j)
            m[j][i] = (float)mat.get_elem(i, j);
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
    m[3][0] = (float)mat.vv.k[0];
    m[3][1] = (float)mat.vv.k[1];
    m[3][2] = (float)mat.vv.k[2];

    ent->SetWorldMatrix(m);
}