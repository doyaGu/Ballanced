#ifndef BUILDINGBLOCKS_PHYSICSMANAGER_H
#define BUILDINGBLOCKS_PHYSICSMANAGER_H

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

class CKPhysicsManager;

class PhysicsCall
{
public:
    PhysicsCall(CKPhysicsManager *pm, CKBehavior *beh) : m_PhysicsManager(pm), m_Value(2), m_Behavior(beh) {}
    virtual int Process();
    virtual ~PhysicsCall();

protected:
    CKPhysicsManager *m_PhysicsManager;
    int m_Value;
    CKBehavior *m_Behavior;
};

class PhysicsForceCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsWakeUpCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsResetCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsBallJointCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsHingeCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsSliderCall : public PhysicsCall
{
    int Process()
    {
    }
};

class PhysicsSpringCall : public PhysicsCall
{
    int Process()
    {
    }
};

class IPhysicsCollision
{
public:
    virtual ~IPhysicsCollision() {}
};

class IPhysicsCollisionListener
{
public:
    virtual void PreCollision(IVP_Event_Collision *pEvent) = 0;
    virtual void PostCollision(IVP_Event_Collision *pEvent) = 0;
    virtual void FrictionCreated(IVP_Event_Friction *pEvent) = 0;
    virtual void FrictionDeleted(IVP_Event_Friction *pEvent) = 0;
};

class IPhysicsObjectListener
{
public:
    virtual void ObjectDeleted(IVP_Event_Object *pEvent) = 0;
    virtual void ObjectCreated(IVP_Event_Object *pEvent) = 0;
    virtual void ObjectRevived(IVP_Event_Object *pEvent) = 0;
    virtual void ObjectFrozen(IVP_Event_Object *pEvent) = 0;
};

class IPhysicsObject
{
public:
    virtual ~IPhysicsObject(void) {}

    // returns true if this object is static/unmoveable
    // NOTE: returns false for objects that are not created static, but set EnableMotion(false);
    // Call IsMoveable() to find if the object is static OR has motion disabled
    virtual bool IsStatic(void) = 0;

    // "wakes up" an object
    // NOTE: ALL OBJECTS ARE "Asleep" WHEN CREATED
    virtual void Wake(void) = 0;
    virtual void Sleep(void) = 0;
    virtual bool IsAsleep(void) = 0;

    // Game can store data in each object (link back to game object)
    virtual void SetGameData(void *pGameData) = 0;
    virtual void *GetGameData(void) const = 0;
    // This flags word can be defined by the game as well
    virtual void SetGameFlags(unsigned short userFlags) = 0;
    virtual unsigned short GetGameFlags(void) const = 0;

    // setup various callbacks for this object
    virtual void SetCallbackFlags(unsigned short callbackflags) = 0;
    // get the current callback state for this object
    virtual unsigned short GetCallbackFlags(void) = 0;

    // mass accessors
    virtual void SetMass(float mass) = 0;
    virtual float GetMass(void) const = 0;

    virtual void SetDamping(const float *speed, const float *rot) = 0;
    virtual void GetDamping(float *speed, float *rot) = 0;

    // material index
    virtual int GetMaterialIndex() const = 0;
    virtual void SetMaterialIndex(int materialIndex) = 0;

    // Enable / disable collisions for this object
    virtual void EnableCollisions(bool enable) = 0;
    // Enable / disable gravity for this object
    virtual void EnableGravity(bool enable) = 0;
    // Enable / disable air friction / drag for this object
    virtual void EnableDrag(bool enable) = 0;
    // Enable / disable motion (pin / unpin the object)
    virtual void EnableMotion(bool enable) = 0;

    // call this when the collision filter conditions change due to this
    // object's state (e.g. changing solid type or collision group)
    virtual void RecheckCollisionFilter() = 0;

    // NOTE:	These are here for convenience, but you can do them yourself by using the matrix
    //			returned from GetPositionMatrix()
    // convenient coordinate system transformations (params - dest, src)
    virtual void LocalToWorld(VxVector &worldPosition, const VxVector &localPosition) = 0;
    virtual void WorldToLocal(VxVector &localPosition, const VxVector &worldPosition) = 0;

    // transforms a vector (no translation) from object-local to world space
    virtual void LocalToWorldVector(VxVector &worldVector, const VxVector &localVector) = 0;
    // transforms a vector (no translation) from world to object-local space
    virtual void WorldToLocalVector(VxVector &localVector, const VxVector &worldVector) = 0;

    // push on an object
    // force vector is direction & magnitude of impulse kg in / s
    virtual void ApplyForceCenter(const VxVector &forceVector) = 0;
    virtual void ApplyForceOffset(const VxVector &forceVector, const VxVector &worldPosition) = 0;

    // Calculates the force/torque on the center of mass for an offset force impulse
    virtual void CalculateForceOffset(const VxVector &forceVector, const VxVector &worldPosition, VxVector &centerForce, VxVector &centerTorque) = 0;
    // Calculates the linear/angular velocities on the center of mass for an offset velocity impulse
    virtual void CalculateVelocityOffset(const VxVector &velocityVector, const VxVector &worldPosition, VxVector &centerVelocity, VxVector &centerAngularVelocity) = 0;

    // NOTE: This will teleport the object
    virtual void SetPosition(const VxVector &worldPosition, const Vx3DMatrixToEulerAngles &angles, bool isTeleport) = 0;
    virtual void SetPositionMatrix(const VxMatrix &matrix, bool isTeleport) = 0;

    virtual void GetPosition(VxVector *worldPosition, VxVector *angles) = 0;
    virtual void GetPositionMatrix(VxMatrix &positionMatrix) = 0;
    // force the velocity to a new value
    // NOTE: velocity is in worldspace, angularVelocity is relative to the object's
    // local axes (just like pev->velocity, pev->avelocity)
    virtual void SetVelocity(const VxVector *velocity, const VxVector *angularVelocity) = 0;

    // NOTE: velocity is in worldspace, angularVelocity is relative to the object's
    // local axes (just like pev->velocity, pev->avelocity)
    virtual void GetVelocity(VxVector *velocity, VxVector *angularVelocity) = 0;

    // NOTE: These are velocities, not forces.  i.e. They will have the same effect regardless of
    // the object's mass or inertia
    virtual void AddVelocity(const VxVector *velocity, const VxVector *angularVelocity) = 0;
    virtual void GetVelocityAtPoint(const VxVector &worldPosition, VxVector &velocity) = 0;

    virtual float GetEnergy() = 0;

    // returns true if the object is in contact with another object
    // if true, puts a point on the contact surface in contactPoint, and
    // a pointer to the object in contactObject
    // NOTE: You can pass NULL for either to avoid computations
    // JAY: This is still an experiment
    virtual bool GetContactPoint(VxVector *contactPoint, IPhysicsObject **contactObject) = 0;

    // refactor this a bit - move some of this to IPhysicsShadowController
    virtual void SetShadow(const VxVector &maxVelocity, const VxVector &maxAngularVelocity, bool allowPhysicsMovement, bool allowPhysicsRotation) = 0;
    virtual void UpdateShadow(const VxVector &targetPosition, const VxVector &targetAngles, bool tempDisableGravity, float timeOffset) = 0;

    // returns number of ticks since last Update() call
    virtual int GetShadowPosition(VxVector *position, VxVector *angles) = 0;

    virtual const CPhysCollide *GetCollide(void) const = 0;
    virtual const char *GetName() = 0;
    virtual void RemoveShadowController() = 0;
    virtual bool IsMoveable() = 0;

    // coefficients are optional, pass either
    virtual void SetDragCoefficient(float *pDrag, float *pAngularDrag) = 0;

    virtual float GetSphereRadius() = 0;

    virtual float CalculateLinearDrag(const VxVector &unitDirection) const = 0;
    virtual float CalculateAngularDrag(const VxVector &objectSpaceRotationAxis) const = 0;

    virtual void BecomeTrigger() = 0;
    virtual void RemoveTrigger() = 0;
    virtual bool IsTrigger() = 0;
    virtual bool IsFluid() = 0;						// fluids are special triggers with fluid controllers attached, they return true to IsTrigger() as well!
    virtual void SetBuoyancyRatio(float ratio) = 0; // Override bouyancy
    virtual unsigned int GetContents() = 0;
    virtual void SetContents(unsigned int contents) = 0;
};

class CKPhysicsManager : public CKBaseManager
{
public:
    CKPhysicsManager(CKContext *ctx);
    ~CKPhysicsManager();

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

    int Physicalize(CK3dEntity *target, int convexCount, CKMesh **convexes, int ballCount, int concaveCount, CKMesh **concaves, float ballRadius, CKSTRING collisionSurface, VxVector *shiftMassCenter, BOOL fixed, IVP_Material *material, float mass, CKSTRING collisionGroup, BOOL startFrozen, BOOL enableCollision, BOOL autoCalcMassCenter, float linearSpeedDampening, float rotSpeedDampening);

    IVP_Ball *CreatePhysicsBall(CKSTRING name, float mass, float ballRadius, IVP_Material *material, float linearSpeedDampening, float rotSpeedDampening, CK3dEntity *target, BOOL startFrozen, BOOL fixed, char *collisionGroup, BOOL enableCollision, VxVector *shiftMassCenter);

    IVP_Polygon *CreatePhysicsPolygon(CKSTRING name, float mass, IVP_Material *material, float linearSpeedDampening, float rotSpeedDampening, CK3dEntity *target, BOOL startFrozen, BOOL fixed, char *collisionGroup, BOOL enableCollision, IVP_SurfaceManager *surman, VxVector *shiftMassCenter);

    IVP_SurfaceManager *GetSurfaceManager(const CKSTRING collisionSurface);

    void AddSurfaceManager(const CKSTRING collisionSurface, IVP_SurfaceManager *surman);

    static CKPhysicsManager *GetManager(CKContext *context)
    {
        return (CKPhysicsManager *)context->GetManagerByGuid(TT_PHYSICS_MANAGER_GUID);
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
    XArray<PhysicsCall> m_Transactionsw;
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
    XNHashTable<CK_ID, IVP_Compact_Surface> m_Table;
};

#endif // BUILDINGBLOCKS_PHYSICSMANAGER_H