#ifndef BUILDINGBLOCKS_PHYSICSMANAGER_H
#define BUILDINGBLOCKS_PHYSICSMANAGER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CKBaseManager.h"
#include "CKContext.h"
#include "XNHashTable.h"

#include "ivp_physics.hxx"
#include "ivp_material.hxx"
#include "ivp_collision_filter.hxx"
#include "ivp_listener_collision.hxx"
#include "ivp_listener_object.hxx"
#include "ivp_compact_surface.hxx"
#include "ivp_surbuild_pointsoup.hxx"
#include "ivp_surbuild_ledge_soup.hxx"
#include "ivp_surman_polygon.hxx"
#include "ivu_string_hash.hxx"

#define TT_PHYSICS_MANAGER_GUID CKGUID(0x6BED328B, 0x141F5148)

#define IVP_COMPACT_SURFACE_ID MAKEID('I', 'V', 'P', 'S')

class CKIpionManager;

class PhysicsStruct
{
public:
    IVP_SurfaceManager *m_SurfaceManager;
    IVP_Real_Object *m_PhysicsObject;
    DWORD field_8;
    VxVector m_Scale;
    DWORD field_18;
    DWORD field_1C;
    DWORD field_20;
    DWORD field_24;
    DWORD field_28;
    void *field_2C;
};

typedef XNHashTable<PhysicsStruct, CK_ID> PhysicStructTable;

class PhysicsCall
{
public:
    PhysicsCall(CKIpionManager *pm, CKBehavior *beh) : m_PhysicsManager(pm), m_Value(2), m_Behavior(beh) {}
    virtual CKBOOL Call() = 0;
    virtual ~PhysicsCall();

    CKIpionManager *m_PhysicsManager;
    int m_Value;
    CKBehavior *m_Behavior;
};

class PhysicsForceCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsWakeUpCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsResetCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsBallJointCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsHingeCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsSliderCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

class PhysicsSpringCall : public PhysicsCall
{
    CKBOOL Call()
    {
    }
};

struct PhysicsCallManager
{
    CKIpionManager *m_PhysicsManager;
    BOOL m_HasPhysicsCalls;
    IVP_U_Vector<PhysicsCall> m_PhysicsCalls[3];
};

class CKIpionManager : public CKBaseManager
{
public:
    CKIpionManager(CKContext *context);
    ~CKIpionManager();

    CKERROR OnCKInit() { return CK_OK; }
    CKERROR OnCKEnd() { return CK_OK; }
    CKERROR OnCKPlay() { return CK_OK; }
    CKERROR OnCKPause() { return CK_OK; }
    CKERROR OnCKReset() { return CK_OK; }
    CKERROR PostClearAll() { return CK_OK; }
    CKERROR PreProcess() { return CK_OK; }
    CKERROR PostProcess() { return CK_OK; }
    CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PreProcess |
               CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_OnCKReset;
    }

    void AddSurfaceManager(CKSTRING collisionSurface, IVP_SurfaceManager *surfaceManager);
    IVP_SurfaceManager *GetSurfaceManager(CKSTRING collisionSurface) const;

    int AddConvexSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale);
    void AddConcaveSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *concave, VxVector *scale);

    void FillTemplateInfo(
        IVP_Template_Real_Object *templ,
        IVP_U_Point *position,
        IVP_U_Quat *orientation,
        CKSTRING name,
        float mass,
        IVP_Material *material,
        float linearSpeedDampening,
        float rotSpeedDampening,
        CK3dEntity *target,
        CKBOOL fixed,
        CKSTRING collisionGroup,
        IVP_U_Matrix *massCenterMatrix,
        VxVector *shiftMassCenter);

    int CreatePhysicObjectOnParameters(
        CK3dEntity *entity,
        int convexCount,
        CKMesh **convexes,
        int ballCount,
        int concaveCount,
        CKMesh **concaves,
        float ballRadius,
        CKSTRING collisionSurface,
        VxVector *shiftMassCenter,
        CKBOOL fixed,
        IVP_Material *material,
        float mass,
        CKSTRING collisionGroup,
        CKBOOL startFrozen,
        CKBOOL enableCollision,
        CKBOOL autoCalcMassCenter,
        float linearSpeedDampening,
        float rotSpeedDampening);

    IVP_Ball *CreatePhysicsBall(
        CKSTRING name,
        float mass,
        float ballRadius,
        IVP_Material *material,
        float linearSpeedDampening,
        float rotSpeedDampening,
        CK3dEntity *entity,
        CKBOOL startFrozen,
        CKBOOL fixed,
        CKSTRING collisionGroup,
        CKBOOL enableCollision,
        VxVector *shiftMassCenter);

    IVP_Polygon *CreatePhysicObject(
        CKSTRING name,
        float mass,
        IVP_Material *material,
        float linearSpeedDampening,
        float rotSpeedDampening,
        CK3dEntity *target,
        CKBOOL startFrozen,
        CKBOOL fixed,
        char *collisionGroup,
        CKBOOL enableCollision,
        IVP_SurfaceManager *surfaceManager,
        VxVector *shiftMassCenter);

    PhysicsStruct *HasPhysics(CK3dEntity *entity, CKBOOL logging = FALSE);

    void CreateEnvironment();
    void DeleteEnvironment();

    static CKIpionManager *GetManager(CKContext *context)
    {
        return (CKIpionManager *)context->GetManagerByGuid(TT_PHYSICS_MANAGER_GUID);
    }

    static IVP_U_Point Cast(const VxVector &v)
    {
        return IVP_U_Point(v.x, v.y, v.z);
    }

    static VxVector Cast(const IVP_U_Point &p)
    {
        return VxVector(p.k[0], p.k[1], p.k[2]);
    }

    IVP_U_Vector<int> m_Vector1;
    int field_30;
    IVP_U_Vector<int> m_Vector2;
    IVP_U_Vector<IVP_Material> m_Materials;
    IVP_U_Vector<int> m_Vector4;
    PhysicsCallManager m_PhysicsCallManager;
    IVP_Listener_Collision *m_CollisionListener;
    int field_5C;
    int m_CollDetectionID;
    IVP_Collision_Filter_Exclusive_Pair *m_CollisionFilterExclusivePair;
    int field_68;
    LARGE_INTEGER *field_6C;
    int field_70;
    LARGE_INTEGER field_78;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
    int field_90;
    int field_94;
    int field_98;
    int field_9C;
    int field_A0;
    int field_A4;
    int field_A8;
    int field_AC;
    int field_B0;
    int field_B4;
    IVP_U_String_Hash *m_StringHash1;
    IVP_U_String_Hash *m_SurfaceManagers;
    IVP_Environment *m_IVPEnv;
    int field_C8;
    float m_CurrentTime;
    float m_PhysicsTimeFactor;
    float m_Factor;
    int field_D8;
    int field_DC;
    int m_HasPhysicsCalls;
    int m_PhysicalizeCalls;
    int m_DePhysicalizeCalls;
    LARGE_INTEGER m_HasPhysicsTime;
    LARGE_INTEGER m_DePhysicalizeTime;
    int field_FC;
    int field_100;
    int field_104;
    LARGE_INTEGER field_108;
    int field_110;
    int field_114;
    void *field_118[200];
    PhysicStructTable m_PhysicStructTable;
};

#endif // BUILDINGBLOCKS_PHYSICSMANAGER_H