#include "CKIpionManager.h"

#include "CKTimeManager.h"
#include "CKMesh.h"
#include "CK3dEntity.h"
#include "CK3dObject.h"
#include "CKBehavior.h"
#include "CKParameterOut.h"
#include "VxMatrix.h"

CKIpionManager::CKIpionManager(CKContext *context)
    : CKBaseManager(context, TT_PHYSICS_MANAGER_GUID, "TT Physics Manager")
{
    m_PhysicsObjectAccessTimes = 0;
    m_PhysicalizeTimes = 0;
    m_DePhysicalizeTimes = 0;

    LARGE_INTEGER frequency;
    LARGE_INTEGER performanceCount;
    ::QueryPerformanceFrequency(&frequency);
    ::QueryPerformanceCounter(&performanceCount);

    m_PerformanceCount = performanceCount.LowPart;
    m_CollisionFilterExclusivePair = NULL;
    m_PreSimulateCallbacks = NULL;
    m_PostSimulateCallbacks = NULL;
    m_CollisionListener = NULL;
    m_PhysicsObjectListener = NULL;
    field_30 = 0;
    field_C8 = 0;
    m_DeltaTime = 0.0;

    if (context->RegisterNewManager(this) == CKERR_MANAGERALREADYEXISTS)
        ::OutputDebugStringA("Manager already exists");

    field_90 = 0;
    field_98 = 0;
    field_A8 = 0;
    m_StringHash1 = NULL;
    m_SurfaceManagers = NULL;
    m_Environment = NULL;
    m_PerformanceCount = 0;
    m_UniversePSI = 0;
    m_ControllersPSI = 0;
    m_IntegratorsPSI = 0;
    m_HullPSI = 0;
    m_ShortMindistsPSI = 0;
    m_CriticalMindistsPSI = 0;
    field_94 = 0;
    field_9C = 0;
    field_A0 = 0;
    field_AC = 0;
    field_B4 = 0;
    field_B0 = 0;
    m_CollisionDetectionID = -1;
}

CKIpionManager::~CKIpionManager() {}

CKERROR CKIpionManager::OnCKInit()
{
    m_SurfaceManagers = new IVP_U_String_Hash(4);
    m_Environment = NULL;
    m_PhysicsTimeFactor = 0.001f;
    field_68 = 0;
    return CK_OK;
}

CKERROR CKIpionManager::OnCKEnd()
{
    DestroyEnvironment();
    DeleteCollisionSurface();

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
    field_68 = 0;
    m_PhysicsObjectContainer.ClearObjects();

    return CK_OK;
}

CKERROR CKIpionManager::PostClearAll()
{
    DeleteCollisionSurface();

    const int len = m_Surfaces.len();
    for (int i = len - 1; i >= 0; --i)
    {
        IVP_Liquid_Surface_Descriptor_Simple *surface = m_Surfaces.element_at(i);
        m_Surfaces.remove_at(i);
        delete surface;
    }

    if (m_SurfaceManagers)
        delete m_SurfaceManagers;
    m_SurfaceManagers = new IVP_U_String_Hash(64);

    return CK_OK;
}

CKERROR CKIpionManager::PostProcess()
{
    float delta = m_Context->GetTimeManager()->GetLastDeltaTime();
    float time = (delta + m_DeltaTime * 3.0f) * 0.25f;
    m_DeltaTime = time;
    m_PhysicsDeltaTime = time * m_PhysicsTimeFactor;
    if (m_Environment)
    {
        if (m_PreSimulateCallbacks->m_HasPhysicsCallback)
            m_PreSimulateCallbacks->Process();

        m_Environment->simulate_dtime(m_PhysicsDeltaTime);

        m_ContactManager->Process(m_Environment->get_current_time());

        if (m_PostSimulateCallbacks->m_HasPhysicsCallback)
            m_PostSimulateCallbacks->Process();

        IVP_PerformanceCounter_Simple *pc = (IVP_PerformanceCounter_Simple *)m_Environment->get_performancecounter();
        m_UniversePSI += pc->counter[IVP_PE_PSI_UNIVERSE][IVP_PE_PSI_START];
        m_ControllersPSI = pc->counter[IVP_PE_PSI_CONTROLLERS][IVP_PE_PSI_START] + m_ControllersPSI;
        m_IntegratorsPSI = pc->counter[IVP_PE_PSI_INTEGRATORS][IVP_PE_PSI_START] + m_IntegratorsPSI;
        m_HullPSI = pc->counter[IVP_PE_PSI_HULL][IVP_PE_PSI_START] + m_HullPSI;
        m_ShortMindistsPSI = pc->counter[IVP_PE_PSI_SHORT_MINDISTS][IVP_PE_PSI_START] + m_ShortMindistsPSI;
        m_CriticalMindistsPSI = pc->counter[IVP_PE_PSI_CRITICAL_MINDISTS][IVP_PE_PSI_START] + m_CriticalMindistsPSI;

        pc->reset_and_print_performance_counters(m_Environment->get_current_time());

        const int len = m_RealObjects.len();
        for (int i = len - 1; i >= 0; --i)
        {
            IVP_Real_Object *obj = m_RealObjects.element_at(i);
            UpdateObjectWorldMatrix(obj);
        }
    }

    return CK_OK;
}

void CKIpionManager::Reset()
{
    DestroyEnvironment();

    m_PhysicsTimeFactor = 0.001f;
    field_68 = 0;
    m_PhysicsObjectContainer.ClearObjects();

    CreateEnvironment();
}

PhysicsObject *CKIpionManager::GetPhysicsObject(CK3dEntity *entity, CKBOOL logging)
{
    LARGE_INTEGER begin;
    ::QueryPerformanceCounter(&begin);

    ++m_PhysicsObjectAccessTimes;
    PhysicsObject *obj = m_PhysicsObjectContainer.GetPhysicsObject(entity->GetID());
    if (!obj && logging)
        m_Context->OutputToConsoleEx("You must Physicalize %s ...", entity->GetName());

    LARGE_INTEGER end;
    ::QueryPerformanceCounter(&end);

    m_DePhysicalizeTime.QuadPart = end.QuadPart - begin.QuadPart;
    if (m_DePhysicalizeTime.QuadPart > m_HasPhysicsTime.QuadPart)
        m_HasPhysicsTime.QuadPart = m_DePhysicalizeTime.QuadPart;

    return obj;
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
    m_PhysicsObjectContainer.AddObject(target->GetID(), po);

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

    m_PhysicsObjectListener = new PhysicsListenerObject(this);
    m_Environment->add_listener_object_global(m_PhysicsObjectListener);

    m_ContactManager = new PhysicsContactManager(this);
    m_ContactManager->SetupContactID();
    SetupCollisionDetectID();
}

void CKIpionManager::DeleteEnvironment()
{
    if (m_Environment)
    {
        delete m_Environment;
        m_Environment = NULL;
    }

    m_CollisionFilterExclusivePair = NULL;
}

void CKIpionManager::DestroyEnvironment()
{
    if (m_Environment)
    {
        delete m_Environment;
        m_Environment = NULL;
    }

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

    if (m_CollisionListener)
    {
        delete m_CollisionListener;
        m_CollisionListener = NULL;
    }

    if (m_PhysicsObjectListener)
    {
        delete m_PhysicsObjectListener;
        m_PhysicsObjectListener = NULL;
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

    CKAttributeManager *am = m_Context->GetAttributeManager();
    const XObjectPointerArray &array = m_Context->GetObjectListByType(CKCID_3DOBJECT, FALSE);
    for (XObjectPointerArray::Iterator it = array.Begin(); it != array.End(); ++it)
    {
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
            }
        }
    }
}

void CKIpionManager::DeleteCollisionSurface()
{
    delete m_SurfaceManagers;
    m_SurfaceManagers = NULL;
}

void CKIpionManager::ResetProfiler()
{
    QueryPerformanceFrequency(&m_ProfilerCounter);
    memset(&m_HasPhysicsTime, 0, sizeof(LARGE_INTEGER));
    memset(&m_DePhysicalizeTime, 0, sizeof(LARGE_INTEGER));
    memset(&field_FC, 0, sizeof(LARGE_INTEGER));
    memset(&field_104, 0, sizeof(LARGE_INTEGER));
    m_PhysicsObjectAccessTimes = 0;
    m_PhysicalizeTimes = 0;
    m_DePhysicalizeTimes = 0;
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
    templ->rot_speed_damp_factor.k[0] = rotSpeedDampening;
    templ->rot_speed_damp_factor.k[1] = rotSpeedDampening;
    templ->rot_speed_damp_factor.k[2] = rotSpeedDampening;
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
    float m20 = mat[2][0];
    mat[1][0] = mat[0][1];
    float m21 = mat[2][1];
    mat[0][1] = m10;
    mat[2][0] = mat[0][2];
    mat[0][2] = m20;
    mat[2][1] = mat[1][2];
    mat[1][2] = m21;

    VxQuaternion quat;
    quat.FromMatrix(mat);
    position->k[0] = mat[3][0];
    position->k[1] = mat[3][1];
    position->k[2] = mat[3][2];
    orientation->x = quat.x;
    orientation->y = quat.y;
    orientation->z = quat.z;
    orientation->w = quat.w;
}

int CKIpionManager::AddConvexSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale)
{
    if (!builder)
        return 0;

    VxVector s = (scale) ? *scale : VxVector(1.0, 1.0, 1.0);

    const int vertexCount = convex->GetVertexCount();
    CKDWORD stride;
    VxVector *pos = (VxVector *)convex->GetPositionsPtr(&stride);

    VxVector **vertices = new VxVector *[vertexCount];

    int count = 0;
    VxVector **ptr = vertices;
    for (int j = 0; j < vertexCount; ++j, pos = (VxVector *)((CKBYTE *)pos + stride))
    {
        int i = 0;
        if (count > 0)
        {
            for (; i < count; ++i)
            {
                if (*pos == *vertices[i])
                    break;
            }
        }

        if (i == count)
        {
            *ptr = pos;
            ++count;
            ++ptr;
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

    VxVector s = (scale) ? *scale : VxVector(1.0, 1.0, 1.0);

    const int faceCount = concave->GetFaceCount();

    VxVector vertex1, vertex2, vertex3;
    IVP_U_Vector<IVP_U_Point> points(3);
    IVP_U_Point point1, point2, point3;
    points.add(&point1);
    points.add(&point2);
    points.add(&point3);

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
        po1->m_CurrentTime = friction->environment->get_current_time();
    ++po1->m_FrictionCount;

    CK3dEntity *entity2 = (CK3dEntity *)situation->objects[1]->client_data;
    if (!entity2)
        return;

    PhysicsObject *po2 = m_IpionManager->GetPhysicsObject(entity2);
    if (!po2)
        return;

    if (po2->m_FrictionCount == 0)
        po2->m_CurrentTime = friction->environment->get_current_time();
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

PhysicsListenerObject::PhysicsListenerObject(CKIpionManager *man) : m_IpionManager(man) {}

void PhysicsListenerObject::event_object_deleted(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;

    IVP_Core *core = obj->get_core();
    if (!core->physical_unmoveable)
    {
        int i = m_IpionManager->m_RealObjects.index_of(obj);
        if (i != -1)
            m_IpionManager->m_RealObjects.remove_at(i);
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

void PhysicsListenerObject::event_object_created(IVP_Event_Object *object) {}

void PhysicsListenerObject::event_object_revived(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_IpionManager->m_RealObjects.add(obj);
}

void PhysicsListenerObject::event_object_frozen(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_IpionManager->m_RealObjects.remove(obj);
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

void PhysicsContactManager::SetupContactID()
{
    CKAttributeManager *am = m_IpionManager->m_Context->GetAttributeManager();
    m_ContactID = am->GetAttributeTypeByName("Continuous Contact ID");
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
    for (int i = 0, pos = 1; i < count; ++i, pos += 2)
    {
        GroupOutput *output = &m_GroupOutputs[i];
        if (output->active)
        {
            output->active = FALSE;
            beh->ActivateOutput(pos, TRUE);
        }
    }

    delete[] m_GroupOutputs;
}