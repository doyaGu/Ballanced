/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		     Physics Continuous Contact
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsContinuousContactDecl();
CKERROR CreatePhysicsContinuousContactProto(CKBehaviorPrototype **pproto);
int PhysicsContinuousContact(const CKBehaviorContext &behcontext);
CKERROR PhysicsContinuousContactCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsContinuousContactDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("PhysicsContinuousContact");
    od->SetDescription("PhysicsContinuousContact");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x199e4cf1, 0x545a78fe));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsContinuousContactProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsContinuousContactProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PhysicsContinuousContact");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Stop");

    char buf[256];
    for (int i = 1; i <= 5; ++i)
    {
        sprintf(buf, "contact on %d", i);
        proto->DeclareOutput(buf);
        sprintf(buf, "contact off %d", i);
        proto->DeclareOutput(buf);
    }

    proto->DeclareInParameter("Time Delay Start", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Time Delay End", CKPGUID_FLOAT, "0.1");

    proto->DeclareSetting("Number Group Output", CKPGUID_INT, "5");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER, "NULL");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsContinuousContact);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PhysicsContinuousContactCallBack);

    *pproto = proto;
    return CK_OK;
}

#define TIME_DELAY_START 0
#define TIME_DELAY_END 1

class PhysicsContactListener : public IVP_Listener_Collision
{
public:
    PhysicsContactListener(IVP_Real_Object *obj, CKIpionManager *man)
        : IVP_Listener_Collision(IVP_LISTENER_COLLISION_CALLBACK_OBJECT_DELETED |
                                 IVP_LISTENER_COLLISION_CALLBACK_FRICTION),
          m_RealObject(obj), m_IpionManager(man)
    {
        obj->add_listener_collision(this);
    }

    ~PhysicsContactListener()
    {
        if (m_RealObject)
            m_RealObject->remove_listener_collision(this);
    }

    void event_collision_object_deleted(IVP_Real_Object *obj)
    {
        if (obj == m_RealObject)
        {
            m_RealObject->remove_listener_collision(this);
            m_RealObject = NULL;
        }
    }

    virtual void event_friction_created(IVP_Event_Friction *friction)
    {
        IVP_Contact_Situation *situation = friction->contact_situation;
        IVP_Real_Object *obj = situation->objects[0];
        if (obj == m_RealObject)
            obj = situation->objects[1];

        int attributeType = m_IpionManager->m_ContactManager->GetContactIDAttributeType();
        if (attributeType == 0)
            return;

        CK3dEntity *ent = (CK3dEntity *)obj->client_data;
        CKParameterOut *pa = ent->GetAttributeParameter(attributeType);
        if (!pa)
            return;

        int contactID;
        pa->GetValue(&contactID);

        int id = contactID - 1;
        if (id < 0)
            return;

        ent = (CK3dEntity *)m_RealObject->client_data;
        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent);
        if (!po)
            return;

        PhysicsContactData *data = po->m_ContactData;
        if (!data)
            return;

        PhysicsContactData::GroupOutput &output = data->m_GroupOutputs[id];
        output.number += 1;
        if (output.number != 1)
            return;

        if (!output.active)
        {
            bool activated = false;
            IVP_Time time = m_IpionManager->GetSimulationTime();
            if (m_IpionManager->GetContactManager()->GetRecordCount() != 0)
            {
                IVP_U_Vector<PhysicsContactRecord> &records = m_IpionManager->GetContactManager()->m_Records;
                const int len = records.len();
                for (int i = 0; i < len; ++i)
                {
                    PhysicsContactRecord *record = records.element_at(i);
                    if (record->m_PhysicsObject == po && record->m_ID == id &&
                        data->m_TimeDelayStart < time - record->m_Time)
                    {
                        records.remove_at(i);
                        delete record;
                        data->m_GroupOutputs[id].active = TRUE;
                        data->m_Behavior->ActivateOutput(2 * id, TRUE);
                        activated = true;
                    }
                }
            }

            if (!activated)
                m_IpionManager->GetContactManager()->AddRecord(po, id, time);
        }
        else
        {
            m_IpionManager->GetContactManager()->RemoveRecord(po, id);
        }
    }

    virtual void event_friction_deleted(IVP_Event_Friction *friction)
    {
        IVP_Contact_Situation *situation = friction->contact_situation;
        IVP_Real_Object *obj = situation->objects[0];
        if (obj == m_RealObject)
            obj = situation->objects[1];

        int attributeType = m_IpionManager->m_ContactManager->GetContactIDAttributeType();
        if (attributeType == 0)
            return;

        CK3dEntity *ent = (CK3dEntity *)obj->client_data;
        CKParameterOut *pa = ent->GetAttributeParameter(attributeType);
        if (!pa)
            return;

        int contactID;
        pa->GetValue(&contactID);

        int id = contactID - 1;
        if (id < 0)
            return;

        ent = (CK3dEntity *)m_RealObject->client_data;
        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent);
        if (!po)
            return;

        PhysicsContactData *data = po->m_ContactData;
        if (!data)
            return;

        PhysicsContactData::GroupOutput &output = data->m_GroupOutputs[id];
        --output.number;
        if (output.number < 0)
            output.number = 0;

        if (output.number == 0 && output.active == TRUE)
            m_IpionManager->GetContactManager()->AddRecord(po, id, m_IpionManager->GetSimulationTime());
    }

private:
    IVP_Real_Object *m_RealObject;
    CKIpionManager *m_IpionManager;
};

class PhysicsContinuousContactCallback : public PhysicsCallback
{
public:
    PhysicsContinuousContactCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return 1;

        float timeDelayStart = 0.1f;
        beh->GetInputParameterValue(TIME_DELAY_START, &timeDelayStart);

        float timeDelayEnd = 0.1f;
        beh->GetInputParameterValue(TIME_DELAY_END, &timeDelayEnd);

        int numberGroupOutput;
        beh->GetLocalParameterValue(0, &numberGroupOutput);

        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent);
        if (!po)
            return 0;

        IVP_Real_Object *obj = po->m_RealObject;

        if (!po->m_ContactData)
        {
            PhysicsContactData *data = new PhysicsContactData(timeDelayStart, timeDelayEnd,
                                                              m_IpionManager->GetContactManager(), beh);
            po->m_ContactData = data;

            PhysicsContactListener *listener = new PhysicsContactListener(obj, m_IpionManager);
            data->m_Listener = listener;
            beh->SetLocalParameterValue(1, &data);
        }

        return 1;
    }
};

int PhysicsContinuousContact(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    if (beh->IsInputActive(0))
    {
        PhysicsContactData *data = NULL;
        beh->GetLocalParameterValue(1, &data);
        if (!data)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            PhysicsContinuousContactCallback *cb = new PhysicsContinuousContactCallback(man, beh);
            man->m_PreSimulateCallbacks->Process(cb);
        }

        beh->ActivateInput(0, FALSE);
        return CKBR_ACTIVATENEXTFRAME;
    }
    else if (beh->IsInputActive(1))
    {
        PhysicsContactData *data = NULL;
        beh->GetLocalParameterValue(1, &data);
        if (data)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (ent)
            {
                PhysicsObject *po = man->GetPhysicsObject(ent);
                if (po)
                {
                    delete data->m_Listener;
                    po->m_ContactData = NULL;
                    data->m_Manager->RemoveRecord(po);
                    delete data;
                }
            }
        }

        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR PhysicsContinuousContactCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (!beh->GetOwner())
        return CKBR_OWNERERROR;

    if (behcontext.CallbackMessage == CKM_BEHAVIORSETTINGSEDITED)
    {
        int numberGroupOutput;
        beh->GetLocalParameterValue(0, &numberGroupOutput);

        while (beh->GetOutputCount() > 0)
            beh->DeleteOutput(0);

        char buf[256];
        for (int i = 0; i < numberGroupOutput; ++i)
        {
            sprintf(buf, "contact on %d", i);
            beh->AddOutput(buf);
            sprintf(buf, "contact off %d", i);
            beh->AddOutput(buf);
        }
    }

    return CKBR_OK;
}
