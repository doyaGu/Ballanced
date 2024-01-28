#ifndef BUILDINGBLOCKS_PHYSICSMANAGER_H
#define BUILDINGBLOCKS_PHYSICSMANAGER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

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

#include "PhysicsCall.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)
#define TT_PHYSICS_MANAGER_GUID CKGUID(0x6BED328B, 0x141F5148)

#define IVP_COMPACT_SURFACE_ID MAKEID('I', 'V', 'P', 'S')

class CKIpionManager;

class PhysicsStruct
{
public:
    IVP_SurfaceManager *m_SurfaceManager;
    IVP_Real_Object *m_PhysicsObject;
    CKDWORD field_8;
    VxVector m_Scale;
    CKDWORD field_18;
    CKDWORD field_1C;
    CKDWORD field_20;
    CKDWORD field_24;
    CKDWORD field_28;
    void *field_2C;
};

typedef XNHashTable<PhysicsStruct, CK_ID> PhysicStructTable;

class PhysicsListenerCollision : IVP_Listener_Object
{
public:

    virtual void event_object_revived(IVP_Event_Object *object) {

    }

    virtual void event_object_frozen(IVP_Event_Object *object) {

    }

    CKIpionManager *m_PhysicsManager;
};

class PhysicsListenerObject : IVP_Listener_Object
{
public:
    CKIpionManager *m_PhysicsManager;
};

class CKPMClass0x54
{
public:
    PhysicsStruct *m_NumberGroupOutput;
    IVP_U_Vector<int> field_4;
    CKIpionManager *m_PhysicsManager;
    CKDWORD m_ContinuousContactID;
};

struct CKPMClass0x110
{
    int field_110;
    int field_114;
    PhysicsStruct m_PhysicsStructs[200];
    XNHashTable<PhysicsStruct, CK_ID> m_Table;
};

class CKIpionManager : public CKBaseManager
{
public:
    CKIpionManager(CKContext *context);
    ~CKIpionManager();

    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKEnd();
    virtual CKERROR OnCKPlay();
    virtual CKERROR OnCKReset();
    virtual CKERROR PostClearAll();
    virtual CKERROR PostProcess();
    virtual CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_OnCKReset;
    }

    int Physicalize(CK3dEntity *target, int convexCount, CKMesh **convexes, int ballCount, int concaveCount,
                    CKMesh **concaves, float ballRadius, CKSTRING collisionSurface, VxVector *shiftMassCenter,
                    BOOL fixed, IVP_Material *material, float mass, CKSTRING collisionGroup,
                    BOOL startFrozen, BOOL enableCollision, BOOL autoCalcMassCenter,
                    float linearSpeedDampening, float rotSpeedDampening);

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

    int CreatePhysicsObjectOnParameters(
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

    IVP_Polygon *CreatePhysicsPolygon(CKSTRING name, float mass, IVP_Material *material,
                                      float linearSpeedDampening, float rotSpeedDampening,
                                      CK3dEntity *target, BOOL startFrozen, BOOL fixed,
                                      char *collisionGroup, BOOL enableCollision,
                                      IVP_SurfaceManager *surman, VxVector *shiftMassCenter);

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

    void SetPhysicsTimeFactor(float factor);
    void SetGravity(const VxVector& gravity);

    IVP_Environment *GetEnvironment() const { return m_IVPEnv; }
    void CreateEnvironment();
    void DestroyEnvironment();

    void DeleteCollisionSurface();

    virtual void Reset();

    void ResetProfiler();

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
    PhysicsCallManager *m_PhysicsCallManager;
    PhysicsCallManager *m_PhysicsCallManager2;
    CKPMClass0x54 *field_54;
    PhysicsListenerObject *m_PhysicsObjectListener;
    int m_CollDetectionID;
    IVP_Collision_Filter_Exclusive_Pair *m_CollisionFilterExclusivePair;
    int field_68;
    int field_6C;
    int field_70;
    int field_74;
    int m_UniversePSI;
    int m_ControllersPSI;
    int m_IntegratorsPSI;
    int m_HullPSI;
    int m_ShortMindistsPSI;
    int m_CriticalMindistsPSI;
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
    LARGE_INTEGER field_FC;
    LARGE_INTEGER field_104;
    LARGE_INTEGER m_ProfilerResetTime;
    CKPMClass0x110 m_PhysicsObjects;
};

#endif // BUILDINGBLOCKS_PHYSICSMANAGER_H