#include "PhysicsCallback.h"

void PhysicsCallbackContainer::Process()
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

void PhysicsCallbackContainer::Process(PhysicsCallback *pc)
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
