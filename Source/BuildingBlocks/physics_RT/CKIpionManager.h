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

#include "ivu_string_hash.hxx"
#include "ivp_physics.hxx"
#include "ivp_real_object.hxx"
#include "ivp_ball.hxx"
#include "ivp_polygon.hxx"
#include "ivp_material.hxx"
#include "ivp_constraint.hxx"
#include "ivp_templates.hxx"
#include "ivp_template_constraint.hxx"
#include "ivp_phantom.hxx"
#include "ivp_controller_buoyancy.hxx"
#include "ivp_liquid_surface_descript.hxx"
#include "ivp_listener_object.hxx"
#include "ivp_listener_collision.hxx"
#include "ivp_collision_filter.hxx"
#include "ivp_compact_surface.hxx"
#include "ivp_surbuild_pointsoup.hxx"
#include "ivp_surbuild_ledge_soup.hxx"
#include "ivp_surman_polygon.hxx"
#include "ivp_performancecounter.hxx"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)
#define TT_PHYSICS_MANAGER_GUID CKGUID(0x6BED328B, 0x141F5148)

class CKIpionManager;

class PhysicsCallback
{
public:
    PhysicsCallback() : m_IpionManager(NULL), m_Type(0), m_Behavior(NULL) {}
    PhysicsCallback(CKIpionManager *pm, CKBehavior *beh, int type) : m_IpionManager(pm), m_Type(type), m_Behavior(beh) {}
    virtual int Execute() = 0;
    virtual ~PhysicsCallback(){};

    CKIpionManager *m_IpionManager;
    int m_Type;
    CKBehavior *m_Behavior;
};

class PhysicsCallbackContainer
{
public:
    explicit PhysicsCallbackContainer(CKIpionManager *manager) : m_IpionManager(manager), m_HasCallbacks(FALSE) {}

    void Process()
    {
        for (int i = 0; i < 3; ++i)
        {
            IVP_U_Vector<PhysicsCallback> &cbs = m_Callbacks[i];
            for (int j = cbs.len() - 1; j >= 0; --j)
            {
                PhysicsCallback *pc = cbs.element_at(j);
                if (pc && pc->m_Behavior && pc->Execute() != 0)
                {
                    delete pc;
                    cbs.remove_at(j);
                }
            }
        }
    }

    void Process(PhysicsCallback *pc)
    {
        if (pc->m_Behavior)
        {
            if (pc->Execute() != 0)
            {
                delete pc;
            }
            else if (0 <= pc->m_Type && pc->m_Type < 3)
            {
                m_Callbacks[pc->m_Type].add(pc);
                m_HasCallbacks = TRUE;
            }
        }
    }

    CKIpionManager *m_IpionManager;
    CKBOOL m_HasCallbacks;
    IVP_U_Vector<PhysicsCallback> m_Callbacks[3];
};

class PhysicsCollisionListener : public IVP_Listener_Collision
{
public:
    explicit PhysicsCollisionListener(CKIpionManager *man);

    virtual void event_friction_created(IVP_Event_Friction *friction);
    virtual void event_friction_deleted(IVP_Event_Friction *friction);

private:
    CKIpionManager *m_IpionManager;
};

class PhysicsObjectListener : public IVP_Listener_Object
{
public:
    explicit PhysicsObjectListener(CKIpionManager *man);

    virtual void event_object_deleted(IVP_Event_Object *object);
    virtual void event_object_created(IVP_Event_Object *object);
    virtual void event_object_revived(IVP_Event_Object *object);
    virtual void event_object_frozen(IVP_Event_Object *object);

private:
    CKIpionManager *m_IpionManager;
};

class PhysicsObject;
class PhysicsContactManager;

class PhysicsContactData
{
public:
    struct GroupOutput
    {
        CKBOOL active;
        int number;
    };

    PhysicsContactData(float timeDelayStart, float timeDelayEnd, PhysicsContactManager *man, CKBehavior *beh);
    ~PhysicsContactData();

    float m_TimeDelayStart;
    float m_TimeDelayEnd;
    PhysicsContactManager *m_Manager;
    CKBehavior *m_Behavior;
    GroupOutput *m_GroupOutputs;
    IVP_Listener_Collision *m_Listener;
};

struct PhysicsContactRecord
{
    IVP_Time m_Time;
    int m_ID;
    PhysicsObject *m_PhysicsObject;
};

class PhysicsContactManager
{
public:
    explicit PhysicsContactManager(CKIpionManager *man)
        : m_IpionManager(man), m_NumberGroupOutput(50), m_ContactID(0) {}

    int GetRecordCount() const;
    void AddRecord(PhysicsObject *obj, int id, IVP_Time time);
    void RemoveRecord(PhysicsObject *obj);
    void RemoveRecord(PhysicsObject *obj, int id);

    void Process(IVP_Time time);

    void SetupContactID();

    int m_NumberGroupOutput;
    IVP_U_Vector<PhysicsContactRecord> m_Records;
    CKIpionManager *m_IpionManager;
    CKDWORD m_ContactID;
};

class PhysicsObject
{
public:
    ~PhysicsObject()
    {
        if (m_ContactData)
        {
            m_ContactData->m_Manager->RemoveRecord(this);
            if (m_ContactData)
                delete m_ContactData;
            m_ContactData = NULL;
        }
    }

    CKBehavior *m_Behavior = NULL;
    IVP_Real_Object *m_RealObject = NULL;
    PhysicsObject *m_PhysicsObject = NULL;
    VxVector m_Scale;
    DWORD m_FrictionCount = 0;
    DWORD m_ID = 0;
    IVP_Time m_CurrentTime;
    DWORD field_28 = 0;
    PhysicsContactData *m_ContactData = NULL;
};

typedef XNHashTable<PhysicsObject, CK_ID> PhysicsObjectTable;

class PhysicsStruct
{
public:
    ~PhysicsStruct()
    {
        if (m_ContactData2)
        {
            m_ContactData2->m_Manager->RemoveRecord(m_PhysicsObject);
            PhysicsContactData *data = m_PhysicsObject->m_ContactData;
            if (data)
                delete data;
            m_PhysicsObject->m_ContactData = NULL;
        }
    }

    CKBehavior *m_Behavior = NULL;
    IVP_Real_Object *m_RealObject = NULL;
    PhysicsObject *m_PhysicsObject = NULL;
    VxVector m_Scale;
    DWORD m_FrictionCount = 0;
    DWORD m_ID = 0;
    IVP_Time m_CurrentTime;
    DWORD field_28 = 0;
    PhysicsContactData *m_ContactData = NULL;
    PhysicsStruct *m_Next = NULL;
    PhysicsContactData *m_ContactData2 = NULL;
};

class PhysicsObjectContainer
{
public:
    PhysicsObjectContainer()
    {
        m_Table.Clear();
    }

    ~PhysicsObjectContainer()
    {
        m_Table.Clear();
    }

    int GetObjectCount() const
    {
        return m_Table.Size();
    }

    PhysicsObject *GetPhysicsObject(CK_ID id)
    {
        PhysicsObjectTable ::Iterator it = m_Table.Find(id);
        if (it == m_Table.End())
            return NULL;
        return it;
    }

    void AddObject(CK_ID id, const PhysicsObject &obj)
    {
        m_Table.Insert(id, obj);
    }

    void RemoveObject(CK_ID id)
    {
        m_Table.Remove(id);
    }

    void ClearObjects()
    {
        m_Table.Clear();
    }

    PhysicsStruct **m_Begin;
    PhysicsStruct **m_End;
    PhysicsStruct m_Data[200];
    PhysicsObjectTable m_Table;
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

    virtual void Reset();

    PhysicsObject *GetPhysicsObject(CK3dEntity *entity, CKBOOL logging = FALSE);

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

    IVP_Constraint *CreateConstraint(const IVP_Template_Constraint *tmpl)
    {
        return m_Environment->create_constraint(tmpl);
    }

    IVP_Actuator_Spring *CreateSpring(IVP_Template_Spring *tmpl)
    {
        return m_Environment->create_spring(tmpl);
    }

    IVP_Environment *GetEnvironment() const { return m_Environment; }
    void CreateEnvironment();
    void DestroyEnvironment();

    void SetTimeFactor(float factor);
    void SetGravity(const VxVector &gravity);

    IVP_SurfaceManager *GetSurfaceManager(CKSTRING collisionSurface) const;
    void AddSurfaceManager(CKSTRING collisionSurface, IVP_SurfaceManager *surfaceManager);

    void SetupCollisionDetectID();
    void DeleteCollisionSurface();

    void ResetProfiler();

    static void FillTemplateInfo(IVP_Template_Real_Object *templ, IVP_U_Point *position, IVP_U_Quat *orientation,
                                 CKSTRING name, float mass, IVP_Material *material,
                                 float linearSpeedDampening, float rotSpeedDampening,
                                 CK3dEntity *target, CKBOOL fixed, CKSTRING collisionGroup,
                                 IVP_U_Matrix *massCenterMatrix, VxVector *shiftMassCenter);

    static int AddConvexSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *convex, VxVector *scale);
    static void AddConcaveSurface(IVP_SurfaceBuilder_Ledge_Soup *builder, CKMesh *concave, VxVector *scale);

    static void UpdateObjectWorldMatrix(IVP_Real_Object *obj);

    static CKIpionManager *GetManager(CKContext *context)
    {
        return (CKIpionManager *)context->GetManagerByGuid(TT_PHYSICS_MANAGER_GUID);
    }

    IVP_U_Vector<IVP_Real_Object> m_RealObjects;
    int field_30;
    IVP_U_Vector<CK3dEntity> m_Entities;
    IVP_U_Vector<IVP_Material> m_Materials;
    IVP_U_Vector<IVP_Liquid_Surface_Descriptor_Simple> m_LiquidSurfaces;
    PhysicsCallbackContainer *m_PreSimulateCallbacks;
    PhysicsCallbackContainer *m_PostSimulateCallbacks;
    PhysicsContactManager *m_ContactManager;
    PhysicsCollisionListener *m_CollisionListener;
    PhysicsObjectListener *m_ObjectListener;
    int m_CollisionDetectionID;
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
    float m_DeltaTime;
    float m_PhysicsDeltaTime;
    float m_PhysicsTimeFactor;
    int field_D8;
    int field_DC;
    int m_PhysicsObjectAccessTimes;
    int m_PhysicalizeTimes;
    int m_DePhysicalizeTimes;
    LARGE_INTEGER m_HasPhysicsTime;
    LARGE_INTEGER m_DePhysicalizeTime;
    LARGE_INTEGER field_FC;
    LARGE_INTEGER field_104;
    LARGE_INTEGER m_ProfilerCounter;
    PhysicsObjectContainer m_PhysicsObjectContainer;
};

#endif // BUILDINGBLOCKS_PHYSICSMANAGER_H