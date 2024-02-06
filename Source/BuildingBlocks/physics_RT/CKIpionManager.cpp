#include "CKIpionManager.h"

#include "CKTimeManager.h"
#include "CKMesh.h"
#include "CK3dEntity.h"
#include "VxMatrix.h"

#include "ivp_templates.hxx"
#include "ivp_performancecounter.hxx"

CKIpionManager::CKIpionManager(CKContext *context)
    : CKBaseManager(context, TT_PHYSICS_MANAGER_GUID, "TT Physics Manager")
{
    m_HasPhysicsCalls = 0;
    m_PhysicalizeCalls = 0;
    m_DePhysicalizeCalls = 0;

    LARGE_INTEGER frequency;
    LARGE_INTEGER performanceCount;
    ::QueryPerformanceFrequency(&frequency);
    ::QueryPerformanceCounter(&performanceCount);

    m_PerformanceCount = performanceCount.LowPart;
    m_CollisionFilterExclusivePair = NULL;
    m_PhysicsCallManager = NULL;
    m_PhysicsCallManager2 = NULL;
    m_CollisionListener = NULL;
    m_PhysicsObjectListener = NULL;
    field_30 = 0;
    field_C8 = 0;
    m_CurrentTime = 0.0;

    if (context->RegisterNewManager(this) == CKERR_MANAGERALREADYEXISTS)
        ::OutputDebugStringA("Manager already exists");

    field_90 = 0;
    field_98 = 0;
    field_A8 = 0;
    m_StringHash1 = NULL;
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
    m_CollDetectionID = -1;
}

CKIpionManager::~CKIpionManager()
{
}

CKERROR CKIpionManager::OnCKInit()
{
    m_SurfaceManagers = new IVP_U_String_Hash(4);
    m_Environment = NULL;
    m_Factor = 0.001;
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
    //    CKAttributeManager *am = m_Context->GetAttributeManager();
    DestroyEnvironment();
    m_Environment = NULL;
    m_Factor = 0.001;
    field_68 = 0;
    m_PhysicsObjects.m_Table.Clear();

    return CK_OK;
}

CKERROR CKIpionManager::PostClearAll()
{
    DeleteCollisionSurface();
    m_Vector4.clear();

    if (m_SurfaceManagers)
        delete m_SurfaceManagers;
    m_SurfaceManagers = new IVP_U_String_Hash(64);

    return CK_OK;
}

CKERROR CKIpionManager::PostProcess()
{
    CKTimeManager *tm = m_Context->GetTimeManager();
    float time = (float)((tm->GetLastDeltaTime() + m_CurrentTime * 3.0) * 0.25);
    m_CurrentTime = time;
    m_TimeFactor = time * m_Factor;
    if (m_Environment)
    {
        if (m_PhysicsCallManager->m_HasPhysicsCalls)
            m_PhysicsCallManager->Process();

        m_Environment->simulate_dtime(m_TimeFactor);

        IVP_PerformanceCounter_Simple *pc = (IVP_PerformanceCounter_Simple *)m_Environment->get_performancecounter();
        m_UniversePSI += pc->counter[IVP_PE_PSI_UNIVERSE][IVP_PE_PSI_START];
        m_ControllersPSI = pc->counter[IVP_PE_PSI_CONTROLLERS][IVP_PE_PSI_START] + m_ControllersPSI;
        m_IntegratorsPSI = pc->counter[IVP_PE_PSI_INTEGRATORS][IVP_PE_PSI_START] + m_IntegratorsPSI;
        m_HullPSI = pc->counter[IVP_PE_PSI_HULL][IVP_PE_PSI_START] + m_HullPSI;
        m_ShortMindistsPSI = pc->counter[IVP_PE_PSI_SHORT_MINDISTS][IVP_PE_PSI_START] + m_ShortMindistsPSI;
        m_CriticalMindistsPSI = pc->counter[IVP_PE_PSI_CRITICAL_MINDISTS][IVP_PE_PSI_START] + m_CriticalMindistsPSI;
    }

    return CK_OK;
}

PhysicsStruct *CKIpionManager::HasPhysics(CK3dEntity *entity, CKBOOL logging)
{
    LARGE_INTEGER begin;
    ::QueryPerformanceCounter(&begin);

    PhysicsStruct *result = NULL;

    ++m_HasPhysicsCalls;
    PhysicStructTable::Iterator it = m_PhysicsObjects.m_Table.Find(entity->GetID());
    if (it != m_PhysicsObjects.m_Table.End())
    {
        result = &(*it);
    }
    else
    {
        if (logging)
            m_Context->OutputToConsoleEx("You must Physicalize %s ...", entity->GetName());
    }

    LARGE_INTEGER end;
    ::QueryPerformanceCounter(&end);

    m_DePhysicalizeTime.QuadPart = end.QuadPart - begin.QuadPart;
    if (m_DePhysicalizeTime.QuadPart > m_HasPhysicsTime.QuadPart)
        m_HasPhysicsTime.QuadPart = m_DePhysicalizeTime.QuadPart;

    return result;
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

    PhysicsStruct ps;
    ps.m_PhysicsObject = obj;
    m_PhysicsObjects.m_Table.Insert(target->GetID(), ps);

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

IVP_SurfaceManager *CKIpionManager::GetSurfaceManager(CKSTRING collisionSurface) const
{
    return (IVP_SurfaceManager *)m_SurfaceManagers->find(collisionSurface);
}

void CKIpionManager::AddSurfaceManager(const CKSTRING collisionSurface, IVP_SurfaceManager *surman)
{
    m_SurfaceManagers->add(collisionSurface, surman);
}

void CKIpionManager::DeleteCollisionSurface()
{
    delete m_SurfaceManagers;
    m_SurfaceManagers = NULL;
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
    IVP_Environment *env = envManager->create_environment(&appEnv, "NeMo", 0x7EFAD621);

    IVP_U_Point gravity = IVP_U_Point(0.0, -9.81, 0.0);
    env->set_gravity(&gravity);

    m_PhysicsCallManager = new PhysicsCallManager(this);
    m_PhysicsCallManager2 = new PhysicsCallManager(this);

    m_CollisionListener = new PhysicsListenerCollision(this);
    m_PhysicsObjectListener = new PhysicsListenerObject(this);

    field_54 = new CKPMClass0x54(this);
    field_54->GetContactID();
    m_Environment = env;
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

    if (m_PhysicsCallManager)
    {
        delete m_PhysicsCallManager;
        m_PhysicsCallManager = NULL;
    }

    if (m_PhysicsCallManager2)
    {
        delete m_PhysicsCallManager2;
        m_PhysicsCallManager2 = NULL;
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

    for (int i = m_Entities.len(); i >= 0; --i)
    {
        CK3dEntity *entity = m_Entities.element_at(i);
        m_Context->DestroyObject(entity);
    }
    m_Entities.clear();

    for (int i = m_Materials.len(); i >= 0; --i)
    {
        IVP_Material *material = m_Materials.element_at(i);
        delete material;
    }
    m_Materials.clear();
}

void CKIpionManager::SetTimeFactor(float factor)
{
    m_TimeFactor = factor * 0.001f;
}

void CKIpionManager::SetGravity(const VxVector &gravity)
{
    IVP_U_Point gravityPoint(gravity.x, gravity.y, gravity.z);
    m_Environment->set_gravity(&gravityPoint);
}

void CKIpionManager::ResetProfiler()
{
    QueryPerformanceFrequency(&m_ProfilerResetTime);
    memset(&m_HasPhysicsTime, 0, sizeof(LARGE_INTEGER));
    memset(&m_DePhysicalizeTime, 0, sizeof(LARGE_INTEGER));
    memset(&field_FC, 0, sizeof(LARGE_INTEGER));
    memset(&field_104, 0, sizeof(LARGE_INTEGER));
    m_HasPhysicsCalls = 0;
    m_PhysicalizeCalls = 0;
    m_DePhysicalizeCalls = 0;
}

void CKIpionManager::Reset()
{
    DestroyEnvironment();

    m_Factor = 0.001f;
    field_68 = 0;
    m_PhysicsObjects.m_Table.Clear();

    CreateEnvironment();
}

void CKIpionManager::FillTemplateInfo(IVP_Template_Real_Object *templ, IVP_U_Point *position, IVP_U_Quat *orientation,
                                      CKSTRING name, float mass, IVP_Material *material, float linearSpeedDampening,
                                      float rotSpeedDampening, CK3dEntity *target, CKBOOL fixed,
                                      CKSTRING collisionGroup, IVP_U_Matrix *massCenterMatrix,
                                      VxVector *shiftMassCenter)
{
    templ->mass = mass;
    templ->material = material;
    templ->physical_unmoveable = static_cast<IVP_BOOL>(fixed);
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

    int vCount = convex->GetModifierVertexCount();
    CKDWORD stride;
    VxVector *vPtr = (VxVector *)convex->GetModifierVertices(&stride);

    int vIndex = 0;
    VxVector **vertices = new VxVector *[vCount];
    if (vCount > 0)
    {
        VxVector **vit = vertices;
        for (int i = 0; i < vCount; ++i, vPtr += stride)
        {
            int j;
            for (j = 0; j < vIndex; ++j)
            {
                if (*vPtr == *vertices[j])
                {
                    break;
                }
            }
            if (j == vIndex)
            {
                *vit++ = vPtr;
                ++vIndex;
            }
        }
    }

    IVP_U_Vector<IVP_U_Point> points(vIndex);
    IVP_U_Point *pts = new IVP_U_Point[vIndex];
    if (vIndex > 0)
    {
        for (int p = 0; p < vIndex; ++p)
        {
            VxVector *v = vertices[p];
            VxVector t = *v * s;
            pts[p].set(t.x, t.y, t.z);

            points.add(&pts[p]);
        }
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

    int fCount = concave->GetFaceCount();

    VxVector vertex1, vertex2, vertex3;
    IVP_U_Vector<IVP_U_Point> points(3);
    IVP_U_Point point1, point2, point3;
    points.add(&point1);
    points.add(&point2);
    points.add(&point3);
    for (int i = 0; i < fCount; ++i)
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

void PhysicsListenerCollision::event_friction_created(IVP_Event_Friction *friction)
{
    if (!friction)
        return;

    CK3dEntity *entity1 = (CK3dEntity *)friction->contact_situation->objects[0]->client_data;
    if (!entity1)
        return;

    PhysicsStruct *ps1 = m_PhysicsManager->HasPhysics(entity1);
    if (!ps1)
        return;

    if (ps1->m_FrictionCount == 0)
        ps1->m_CurrentTime = friction->environment->get_current_time();
    ++ps1->m_FrictionCount;

    CK3dEntity *entity2 = (CK3dEntity *)friction->contact_situation->objects[1]->client_data;
    if (!entity2)
        return;

    PhysicsStruct *ps2 = m_PhysicsManager->HasPhysics(entity2);
    if (!ps2)
        return;

    if (ps2->m_FrictionCount == 0)
        ps2->m_CurrentTime = friction->environment->get_current_time();
    ++ps2->m_FrictionCount;
}

void PhysicsListenerCollision::event_friction_deleted(IVP_Event_Friction *friction)
{
    if (friction)
        return;

    CK3dEntity *entity1 = (CK3dEntity *)friction->contact_situation->objects[0]->client_data;
    if (!entity1)
        return;

    PhysicsStruct *ps1 = m_PhysicsManager->HasPhysics(entity1);
    if (!ps1)
        return;

    --ps1->m_FrictionCount;

    CK3dEntity *entity2 = (CK3dEntity *)friction->contact_situation->objects[1]->client_data;
    if (!entity2)
        return;

    PhysicsStruct *ps2 = m_PhysicsManager->HasPhysics(entity2);
    if (!ps2)
        return;

    --ps2->m_FrictionCount;
}

void PhysicsListenerObject::event_object_deleted(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;

    IVP_Core *core = obj->get_core();
    if (!core->physical_unmoveable)
        m_PhysicsManager->m_RealObjects.remove(obj);

    CK3dEntity *entity = (CK3dEntity *)obj->client_data;
    if (!entity)
        return;

    PhysicsStruct *ps = m_PhysicsManager->HasPhysics(entity);
    if (ps && ps->m_ContactData)
    {
        delete ps->m_ContactData;
        ps->m_ContactData = NULL;
    }
}

void PhysicsListenerObject::event_object_created(IVP_Event_Object *object) {}

void PhysicsListenerObject::event_object_revived(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_PhysicsManager->m_RealObjects.add(obj);
}

void PhysicsListenerObject::event_object_frozen(IVP_Event_Object *object)
{
    IVP_Real_Object *obj = object->real_object;
    m_PhysicsManager->m_RealObjects.remove(obj);
}

void CKPMClass0x54::GetContactID()
{
    CKAttributeManager *am = m_PhysicsManager->m_Context->GetAttributeManager();
    m_ContinuousContactID = am->GetAttributeTypeByName("Continuous Contact ID");
}
