#include "CKIpionManager.h"

#include "CKTimeManager.h"
#include "CKMesh.h"
#include "CK3dEntity.h"

#include "ivp_performancecounter.hxx"

int ConvertConvexToLedge(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale)
{
    if (!builder)
    {
        return 0;
    }

    VxVector s = (scale) ? VxVector(1.0, 1.0, 1.0) : *scale;

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
            VxVector *&v = vertices[p];
            pts[p] = CKIpionManager::Cast(*v * s);

            points.add(&pts[p]);
        }
    }

    if (vertices)
    {
        delete[] vertices;
    }

    IVP_Compact_Ledge *ledge = IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge(&points);
    int ret = 0;
    if (ledge)
    {
        builder->insert_ledge(ledge);
        ret = 1;
    }

    if (pts)
    {
        delete[] pts;
    }
    return ret;
}

void ConvertConcaveToLedge(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *concave, VxVector *scale)
{
    if (!builder)
    {
        return;
    }

    VxVector s = (scale) ? VxVector(1.0, 1.0, 1.0) : *scale;

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
        point1 = CKIpionManager::Cast(vertex1 * s);
        point2 = CKIpionManager::Cast(vertex2 * s);
        point3 = CKIpionManager::Cast(vertex3 * s);
        IVP_Compact_Ledge *ledge = IVP_SurfaceBuilder_Pointsoup::convert_pointsoup_to_compact_ledge(&points);
        if (ledge)
        {
            builder->insert_ledge(ledge);
        }
    }
}

CKIpionManager::CKIpionManager(CKContext *context)
    : CKBaseManager(context, TT_PHYSICS_MANAGER_GUID, "TT Physics Manager")
{
    context->RegisterNewManager(this);
}

CKIpionManager::~CKIpionManager()
{
}

CKERROR CKIpionManager::OnCKInit()
{
    m_SurfaceManagers = new IVP_U_String_Hash(4);
    m_IVPEnv = NULL;
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
    m_IVPEnv = NULL;
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
}

CKERROR CKIpionManager::PostProcess()
{
    CKTimeManager *tm = m_Context->GetTimeManager();
    float time = (float)((tm->GetLastDeltaTime() + m_CurrentTime * 3.0) * 0.25);
    m_CurrentTime = time;
    m_PhysicsTimeFactor = time * m_Factor;
    if (m_IVPEnv)
    {
        if (m_PhysicsCallManager->m_HasPhysicsCalls)
        {
            m_PhysicsCallManager->Process();
        }

        m_IVPEnv->simulate_dtime(m_PhysicsTimeFactor);

        IVP_PerformanceCounter_Simple *pc = (IVP_PerformanceCounter_Simple *)m_IVPEnv->get_performancecounter();
        m_UniversePSI += pc->counter[IVP_PE_PSI_UNIVERSE][IVP_PE_PSI_START];
        m_ControllersPSI = pc->counter[IVP_PE_PSI_CONTROLLERS][IVP_PE_PSI_START] + m_ControllersPSI;
        m_IntegratorsPSI = pc->counter[IVP_PE_PSI_INTEGRATORS][IVP_PE_PSI_START] + m_IntegratorsPSI;
        m_HullPSI = pc->counter[IVP_PE_PSI_HULL][IVP_PE_PSI_START] + m_HullPSI;
        m_ShortMindistsPSI = pc->counter[IVP_PE_PSI_SHORT_MINDISTS][IVP_PE_PSI_START]
                             + m_ShortMindistsPSI;
        m_CriticalMindistsPSI = pc->counter[IVP_PE_PSI_CRITICAL_MINDISTS][IVP_PE_PSI_START]
                                      + m_CriticalMindistsPSI;
    }
}

int CKIpionManager::Physicalize(CK3dEntity *target, int convexCount, CKMesh **convexes, int ballCount, int concaveCount, CKMesh **concaves, float ballRadius, CKSTRING collisionSurface, VxVector *shiftMassCenter, BOOL fixed, IVP_Material *material, float mass, CKSTRING collisionGroup, BOOL startFrozen, BOOL enableCollision, BOOL autoCalcMassCenter, float linearSpeedDampening, float rotSpeedDampening)
{
    VxVector scale;
    target->GetScale(&scale);

    IVP_Real_Object *obj = NULL;

    if (!collisionSurface || strcmp(collisionSurface, "") == 0 || ballCount)
    {
        if (!collisionSurface || strcmp(collisionSurface, "") == 0)
        {
            enableCollision = FALSE;
        }
        obj = CreatePhysicsBall(target->GetName(), mass, ballRadius, material, linearSpeedDampening, rotSpeedDampening, target, startFrozen, fixed, collisionGroup, enableCollision, shiftMassCenter);
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
                        ledgeCount += ConvertConvexToLedge(&builder, convexes[i], &scale);
                    }
                }
            }

            if (concaveCount > 0)
            {
                for (int i = 0; i < concaveCount; i++)
                {
                    if (concaves[i])
                    {
                        ConvertConcaveToLedge(&builder, concaves[i], &scale);
                        ++ledgeCount;
                    }
                }
            }

            if (ledgeCount > 0)
            {
                IVP_Compact_Surface *compactSurface = builder.compile();
                IVP_SurfaceManager *surman = new IVP_SurfaceManager_Polygon(compactSurface);
                AddSurfaceManager(collisionSurface, surman);
                obj = CreatePhysicsPolygon(target->GetName(), mass, material, linearSpeedDampening, rotSpeedDampening, target, startFrozen, fixed, collisionGroup, enableCollision, surman, shiftMassCenter);
            }
        }
    }
    obj->client_data = target;

    struct C110
    {
        IVP_SurfaceManager *m_SurfaceManager;
        IVP_Real_Object *m_PhysicsObject;
        DWORD field_8;
        VxVector m_Scale;
        DWORD field_18;
        DWORD field_1C;
        DWORD field_20;
        DWORD field_24;
        DWORD field_28;
        DWORD field_2C;
    };
    m_Context->OutputToConsoleEx("Error: incorrect mesh for %s !\n", target->GetName());
}

IVP_Ball *CKIpionManager::CreatePhysicsBall(CKSTRING name, float mass, float ballRadius, IVP_Material *material, float linearSpeedDampening, float rotSpeedDampening, CK3dEntity *target, BOOL startFrozen, BOOL fixed, char *collisionGroup, BOOL enableCollision, VxVector *shiftMassCenter)
{
}

IVP_Polygon *CKIpionManager::CreatePhysicsPolygon(CKSTRING name, float mass, IVP_Material *material, float linearSpeedDampening, float rotSpeedDampening, CK3dEntity *target, BOOL startFrozen, BOOL fixed, char *collisionGroup, BOOL enableCollision, IVP_SurfaceManager *surman, VxVector *shiftMassCenter)
{
}

void CKIpionManager::AddSurfaceManager(const CKSTRING collisionSurface, IVP_SurfaceManager *surman)
{
    m_SurfaceManagers->add(collisionSurface, surman);
}

IVP_SurfaceManager *CKIpionManager::GetSurfaceManager(CKSTRING collisionSurface) const
{
    return (IVP_SurfaceManager *)m_SurfaceManagers->find(collisionSurface);
}

void CKIpionManager::CreateEnvironment() {
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


}

void CKIpionManager::SetPhysicsTimeFactor(float factor)
{
    m_PhysicsTimeFactor = factor * 0.001f;
}

void CKIpionManager::SetGravity(const VxVector &gravity)
{
    IVP_U_Point gravityPoint(gravity.x, gravity.y, gravity.z);
    m_IVPEnv->set_gravity(&gravityPoint);
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

