#ifndef BUILDINGBLOCKS_PHYSICSCALL_H
#define BUILDINGBLOCKS_PHYSICSCALL_H

#include "CKBehavior.h"
#include "ivu_vector.hxx"

class CKIpionManager;

class PhysicsCall
{
public:
    PhysicsCall(CKIpionManager *pm, CKBehavior *beh, int type) : m_IpionManager(pm), m_Type(type), m_Behavior(beh) {}
    virtual CKBOOL Execute() = 0;
    virtual ~PhysicsCall() {};

    CKIpionManager *m_IpionManager;
    int m_Type;
    CKBehavior *m_Behavior;
};

class PhysicsForceCall : public PhysicsCall
{
    virtual CKBOOL Execute()
    {
        return TRUE;
    }
};

class PhysicsBallJointCall : public PhysicsCall
{
    virtual CKBOOL Execute()
    {
        return TRUE;
    }
};

class PhysicsHingeCall : public PhysicsCall
{
    virtual CKBOOL Execute()
    {
        return TRUE;
    }
};

class PhysicsSliderCall : public PhysicsCall
{
    virtual CKBOOL Execute()
    {
        return TRUE;
    }
};

class PhysicsSpringCall : public PhysicsCall
{
    virtual CKBOOL Execute()
    {
        return TRUE;
    }
};

class PhysicsCallManager
{
public:
    explicit PhysicsCallManager(CKIpionManager *manager) : m_IpionManager(manager), m_HasPhysicsCalls(FALSE) { }

    void Process()
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = m_PhysicsCalls[i].len() - 1; j >= 0; --j)
            {
                PhysicsCall *pc = m_PhysicsCalls[i].element_at(j);
                if (pc && pc->m_Behavior && pc->Execute())
                {
                    delete pc;
                    m_PhysicsCalls[i].remove_at(j);
                }
            }
        }
    }

    void Process(PhysicsCall *pc)
    {
        if (pc->m_Behavior)
        {
            if (pc->Execute())
            {
                delete pc;
            }
            else if (0 <= pc->m_Type && pc->m_Type < 3)
            {
                m_PhysicsCalls[pc->m_Type].add(pc);
                m_HasPhysicsCalls = TRUE;
            }
        }
    }

    CKIpionManager *m_IpionManager;
    CKBOOL m_HasPhysicsCalls;
    IVP_U_Vector<PhysicsCall> m_PhysicsCalls[3];
};

#endif // BUILDINGBLOCKS_PHYSICSCALL_H
