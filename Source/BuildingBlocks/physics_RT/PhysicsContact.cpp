#include "PhysicsContact.h"

#include "CK3dEntity.h"
#include "CKBehavior.h"
#include "CKParameterOut.h"
#include "CKAttributeManager.h"
#include "CKIpionManager.h"

void PhysicsContactManager::Process(IVP_Time time)
{
    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        PhysicsObject *po = record->m_PhysicsObject;
        PhysicsContactData *data = po->m_ContactData;

        double delta = time - record->m_Time;
        int index = record->m_Index;
        PhysicsContactData::GroupOutput *output = data->m_GroupOutputs;

        if (output[index].active)
        {
            if (output[index].number != 0)
            {
                m_Records.remove_at(i);
                delete record;
            }
            else if (delta > data->m_TimeDelayEnd)
            {
                output[index].active = FALSE;
                data->m_Behavior->ActivateOutput(2 * index + 1, TRUE);
                m_Records.remove_at(i);
                delete record;
            }
        }
        else if (output[index].number <= 0)
        {
            if (delta * 0.5 > data->m_TimeDelayStart)
            {
                m_Records.remove_at(i);
                delete record;
            }
        }
        else if (delta > data->m_TimeDelayStart)
        {
            output[index].active = TRUE;
            data->m_Behavior->ActivateOutput(2 * index, TRUE);
            m_Records.remove_at(i);
            delete record;
        }
    }
}

int PhysicsContactManager::GetRecordCount() const
{
    return m_Records.len();
}

PhysicsContactRecord *PhysicsContactManager::GetRecord(int index)
{
    return m_Records.element_at(index);
}

void PhysicsContactManager::AddRecord(PhysicsObject *obj, int index, IVP_Time time)
{
    bool found = false;
    const int len = m_Records.len();
    for (int i = 0; i < len; ++i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj && record->m_Index == index)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        PhysicsContactRecord *record = new PhysicsContactRecord(time, index, obj);
        m_Records.add(record);
    }
}

void PhysicsContactManager::RemoveRecord(int index)
{
    PhysicsContactRecord *record = m_Records.element_at(index);
    m_Records.remove_at(index);
    delete record;
}

void PhysicsContactManager::RemoveRecord(PhysicsObject *obj)
{
    if (!obj)
        return;

    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj)
        {
            m_Records.remove_at(i);
            delete record;
        }
    }
}

void PhysicsContactManager::RemoveRecord(PhysicsObject *obj, int index)
{
    if (!obj)
        return;

    const int len = m_Records.len();
    for (int i = len - 1; i >= 0; --i)
    {
        PhysicsContactRecord *record = m_Records.element_at(i);
        if (record->m_PhysicsObject == obj && record->m_Index == index)
        {
            m_Records.remove_at(i);
            delete record;
        }
    }
}

void PhysicsContactManager::SetupContactID()
{
    CKAttributeManager *am = m_IpionManager->m_Context->GetAttributeManager();
    m_ContactIDAttribType = am->GetAttributeTypeByName("Continuous Contact ID");
}

int PhysicsContactManager::GetContactID(CK3dEntity *entity) const
{
    int contactID = -1;
    if (m_ContactIDAttribType != -1 && entity)
    {
        CKParameterOut *pa = entity->GetAttributeParameter(m_ContactIDAttribType);
        if (pa)
            pa->GetValue(&contactID);
    }
    return contactID;
}

PhysicsContactData::PhysicsContactData(float timeDelayStart, float timeDelayEnd,
                                       PhysicsContactManager *man, CKBehavior *beh)
    : m_TimeDelayStart(timeDelayStart), m_TimeDelayEnd(timeDelayEnd), m_Manager(man), m_Behavior(beh)
{
    m_GroupOutputs = new GroupOutput[m_Manager->m_NumberGroupOutput];
    m_Listener = NULL;
}

PhysicsContactData::~PhysicsContactData()
{
    CKBehavior *beh = m_Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return;

    PhysicsObject *po = m_Manager->m_IpionManager->GetPhysicsObject(ent);
    m_Manager->RemoveRecord(po);

    PhysicsContactData *data = NULL;
    beh->SetLocalParameterValue(1, &data);

    const int count = m_Manager->m_NumberGroupOutput;
    for (int i = 0; i < count; ++i)
    {
        GroupOutput *output = &m_GroupOutputs[i];
        if (output->active == TRUE)
        {
            output->active = FALSE;
            beh->ActivateOutput(2 * i + 1, TRUE);
        }
    }

    delete[] m_GroupOutputs;
}