#ifndef PHYSICS_RT_PHYSICSCALLBACK_H
#define PHYSICS_RT_PHYSICSCALLBACK_H

#include "CKBehavior.h"

#include "ivu_types.hxx"
#include "ivu_vector.hxx"

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

    void Process();
    void Process(PhysicsCallback *pc);

    CKIpionManager *m_IpionManager;
    CKBOOL m_HasCallbacks;
    IVP_U_Vector<PhysicsCallback> m_Callbacks[3];
};

#endif // PHYSICS_RT_PHYSICSCALLBACK_H
