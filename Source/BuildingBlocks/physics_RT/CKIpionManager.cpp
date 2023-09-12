#include "physics_RT.h"

#include "CKIpionManager.h"

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

IVP_SurfaceManager *CKIpionManager::GetSurfaceManager(const CKSTRING collisionSurface)
{
    return (IVP_SurfaceManager *)m_SurfaceManagers->find(collisionSurface);
}

void CKIpionManager::AddSurfaceManager(const CKSTRING collisionSurface, IVP_SurfaceManager *surman)
{
    m_SurfaceManagers->add(collisionSurface, surman);
}
