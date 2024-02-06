#ifndef BUILDINGBLOCKS_PHYSICSMANAGER_H
#define BUILDINGBLOCKS_PHYSICSMANAGER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include "CKBaseManager.h"
#include "CKAttributeManager.h"
#include "CKContext.h"
#include "XNHashTable.h"

#include "ivp_physics.hxx"
#include "ivp_real_object.hxx"
#include "ivp_ball.hxx"
#include "ivp_polygon.hxx"
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

struct PhysicsContactData;

struct PhysicsStruct
{
    CKBehavior *m_Behavior;
    IVP_Real_Object *m_PhysicsObject;
    PhysicsStruct *m_Struct;
    VxVector m_Scale;
    DWORD m_FrictionCount;
    DWORD field_1C;
    IVP_Time m_CurrentTime;
    DWORD field_28;
    PhysicsContactData *m_ContactData;
    PhysicsStruct *m_Next;
    PhysicsContactData *m_ContactData2;
};

class CKPMClass0x54;

struct PhysicsContactData
{
    float m_TimeDelayStart;
    float m_TimeDelayEnd;
    CKPMClass0x54 *m_Owner;
    CKBehavior *m_Behavior;
    void *m_GroupOutputs;
    IVP_Listener_Collision *m_Listener;
};

typedef XNHashTable<PhysicsStruct, CK_ID> PhysicStructTable;

class PhysicsListenerCollision : IVP_Listener_Collision
{
public:
    explicit PhysicsListenerCollision(CKIpionManager *manager) : IVP_Listener_Collision(5), m_PhysicsManager(manager) {}

    virtual void event_friction_created(IVP_Event_Friction *friction);
    virtual void event_friction_deleted(IVP_Event_Friction *friction);

private:
    CKIpionManager *m_PhysicsManager;
};

class PhysicsListenerObject : IVP_Listener_Object
{
public:
    explicit PhysicsListenerObject(CKIpionManager *manager) : m_PhysicsManager(manager) {}

    virtual void event_object_deleted(IVP_Event_Object *object);
    virtual void event_object_created(IVP_Event_Object *object);
    virtual void event_object_revived(IVP_Event_Object *object);
    virtual void event_object_frozen(IVP_Event_Object *object);

private:
    CKIpionManager *m_PhysicsManager;
};

class CKPMClass0x54
{
public:
    explicit CKPMClass0x54(CKIpionManager *manager)
    {
        m_PhysicsManager = manager;
        m_NumberGroupOutput = 50;
        m_ContinuousContactID = 0;
    }

    void GetContactID();

    int m_NumberGroupOutput;
    IVP_U_Vector<PhysicsContactData> m_Data;
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
    explicit CKIpionManager(CKContext *context);
    virtual ~CKIpionManager();

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

    PhysicsStruct *HasPhysics(CK3dEntity *entity, CKBOOL logging = FALSE);

    int CreatePhysicsObjectOnParameters(CK3dEntity *target, int convexCount, CKMesh **convexes,
                                        int ballCount, int concaveCount, CKMesh **concaves, float ballRadius,
                                        CKSTRING collisionSurface, VxVector *shiftMassCenter, CKBOOL fixed,
                                        IVP_Material *material, float mass, CKSTRING collisionGroup,
                                        CKBOOL startFrozen, CKBOOL enableCollision, CKBOOL autoCalcMassCenter,
                                        float linearSpeedDampening, float rotSpeedDampening);

    IVP_Ball *CreatePhysicsBall(CKSTRING name, float mass, float ballRadius, IVP_Material *material,
                                float linearSpeedDampening, float rotSpeedDampening, CK3dEntity *entity,
                                CKBOOL startFrozen, CKBOOL fixed, CKSTRING collisionGroup,
                                CKBOOL enableCollision, VxVector *shiftMassCenter);

    IVP_Polygon *CreatePhysicsPolygon(CKSTRING name, float mass, IVP_Material *material,
                                      float linearSpeedDampening, float rotSpeedDampening,
                                      CK3dEntity *target, BOOL startFrozen, BOOL fixed,
                                      CKSTRING collisionGroup, BOOL enableCollision,
                                      IVP_SurfaceManager *surman, VxVector *shiftMassCenter);

    IVP_SurfaceManager *GetSurfaceManager(CKSTRING collisionSurface) const;
    void AddSurfaceManager(CKSTRING collisionSurface, IVP_SurfaceManager *surfaceManager);
    void DeleteCollisionSurface();

    IVP_Environment *GetEnvironment() const { return m_Environment; }
    void CreateEnvironment();
    void DeleteEnvironment();
    void DestroyEnvironment();

    void SetTimeFactor(float factor);
    void SetGravity(const VxVector &gravity);

    void ResetProfiler();

    virtual void Reset();

    static void FillTemplateInfo(IVP_Template_Real_Object *templ, IVP_U_Point *position, IVP_U_Quat *orientation,
                                 CKSTRING name, float mass, IVP_Material *material,
                                 float linearSpeedDampening, float rotSpeedDampening,
                                 CK3dEntity *target, CKBOOL fixed, CKSTRING collisionGroup,
                                 IVP_U_Matrix *massCenterMatrix, VxVector *shiftMassCenter);

    static int AddConvexSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale);
    static void AddConcaveSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *concave, VxVector *scale);

    static CKIpionManager *GetManager(CKContext *context)
    {
        return (CKIpionManager *)context->GetManagerByGuid(TT_PHYSICS_MANAGER_GUID);
    }

    IVP_U_Vector<IVP_Real_Object> m_RealObjects;
    int field_30;
    IVP_U_Vector<CK3dEntity> m_Entities;
    IVP_U_Vector<IVP_Material> m_Materials;
    IVP_U_Vector<int> m_Vector4;
    PhysicsCallManager *m_PhysicsCallManager;
    PhysicsCallManager *m_PhysicsCallManager2;
    CKPMClass0x54 *field_54;
    PhysicsListenerCollision *m_CollisionListener;
    PhysicsListenerObject *m_PhysicsObjectListener;
    int m_CollDetectionID;
    IVP_Collision_Filter_Exclusive_Pair *m_CollisionFilterExclusivePair;
    int field_68;
    int field_6C;
    int field_70;
    int m_PerformanceCount;
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
    IVP_Environment *m_Environment;
    int field_C8;
    float m_CurrentTime;
    float m_TimeFactor;
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