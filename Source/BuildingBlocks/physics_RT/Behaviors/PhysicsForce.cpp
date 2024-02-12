/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Force
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsForceDecl();
CKERROR CreatePhysicsForceProto(CKBehaviorPrototype **pproto);
int PhysicsForce(const CKBehaviorContext &behcontext);
CKERROR PhysicsForceCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsForceDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Force");
    od->SetDescription("Physics Force");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56e20c57, 0xb926068));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsForceProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsForceProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("SetPhysicsForce");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Pos Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Direction Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Force Value", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsForce);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PhysicsForceCallBack);

    *pproto = proto;
    return CK_OK;
}

#define POSITION 0
#define REFERENTIAL 1
#define DIRECTION 2
#define DIRECTION_REF 3
#define FORCE_VALUE 4

class PhysicsControllerForce : public IVP_Controller_Independent
{
public:
    PhysicsControllerForce(IVP_Real_Object *obj, const IVP_U_Point &pos, const IVP_U_Point &force)
    {
        m_Core = obj->get_core();
        m_Position = pos;
        m_Force = force;

        if (m_Core)
            IVP_Controller_Manager::add_controller_to_core(this, m_Core);
    }

    ~PhysicsControllerForce()
    {
        if (m_Core)
            IVP_Controller_Manager::remove_controller_from_core(this, m_Core);
    }

    void core_is_going_to_be_deleted_event(IVP_Core *my_core)
    {
        if (my_core == m_Core)
        {
            my_core->rem_core_controller(this);
            m_Core = NULL;
        }
    }

    void do_simulation_controller(IVP_Event_Sim *es, IVP_U_Vector<IVP_Core> *core_list)
    {
        if (core_list && core_list->len() != 0)
        {
            IVP_U_Matrix mat;
            m_Core->calc_at_matrix(m_Core->get_environment()->get_current_time(), &mat);

            IVP_U_Point out;
            mat.vimult3(&m_Force, &out);

            IVP_U_Float_Point pos(m_Position.k[0], m_Position.k[1], m_Position.k[2]);
            IVP_U_Float_Point fcs(out.k[0], out.k[1], out.k[2]);
            IVP_U_Float_Point fws(m_Force.k[0], m_Force.k[1], m_Force.k[2]);
            m_Core->async_push_core(&pos, &fcs, &fws);
        }
    }

    IVP_CONTROLLER_PRIORITY get_controller_priority() { return IVP_CP_ACTUATOR; };

    IVP_Core *m_Core;
    IVP_U_Point m_Position;
    IVP_U_Point m_Force;
};

class PhysicsForceCallback : public PhysicsCallback
{
public:
    PhysicsForceCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return 1;

        VxVector position;
        beh->GetInputParameterValue(POSITION, &position);

        CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL);

        VxVector direction;
        beh->GetInputParameterValue(DIRECTION, &direction);

        CK3dEntity *directionRef = (CK3dEntity *)beh->GetInputParameterObject(DIRECTION_REF);

        float forceValue = 10.0f;
        beh->GetInputParameterValue(FORCE_VALUE, &forceValue);

        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent);
        if (!po)
            return 0;

        IVP_Real_Object *obj = po->m_RealObject;

        VxVector vec;
        if (directionRef)
            directionRef->TransformVector(&vec, &direction);
        else
            vec = direction;

        IVP_U_Point force(vec.x, vec.y, vec.z);
        if (force.quad_length() <= 0.0001)
            force.set(1.0, 0.0, 0.0);
        else
            force.normize();
        force.mult(forceValue);

        IVP_U_Point pos(position.x, position.y, position.z);
        if (referential != ent)
        {
            VxVector p;
            if (referential)
                referential->Transform(&p, &position);
            else
                p = position;

            IVP_U_Point p1(p.x, p.y, p.z);
            IVP_U_Matrix mat;
            obj->get_core()->calc_at_matrix(obj->get_environment()->get_current_time(), &mat);
            mat.vimult4(&p1, &pos);
        }

        PhysicsControllerForce *controller = new PhysicsControllerForce(obj, pos, force);
        beh->SetLocalParameterValue(0, &controller);

        return 1;
    }
};

int PhysicsForce(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    PhysicsControllerForce *controller = NULL;
    beh->GetLocalParameterValue(0, &controller);

    if (beh->IsInputActive(0))
    {
        if (!controller)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            CKIpionManager *man = CKIpionManager::GetManager(context);

            PhysicsForceCallback *cb = new PhysicsForceCallback(man, beh);
            man->m_PreSimulateCallbacks->Process(cb);
        }

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        if (controller)
        {
            delete controller;
            controller = NULL;
            beh->SetLocalParameterValue(0, &controller);
        }

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}

CKERROR PhysicsForceCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}
