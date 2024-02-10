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
    if (!proto) return CKERR_OUTOFMEMORY;

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
    PhysicsControllerForce(IVP_Real_Object *obj, CKIpionManager *man) : m_IpionManager(man) {
        m_Core = obj->get_core();
        IVP_Environment *env = m_IpionManager->GetEnvironment();
        env->get_controller_manager()->add_controller_to_core(this, m_Core);
    }

    ~PhysicsControllerForce()
    {
        IVP_Environment *env = m_IpionManager->GetEnvironment();
        if (env)
            env->get_controller_manager()->remove_controller_from_core(this, m_Core);
    }

    void core_is_going_to_be_deleted_event(IVP_Core *my_core)
    {
        my_core->rem_core_controller(this);
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

    CKIpionManager *m_IpionManager;
    IVP_Core *m_Core;
    DWORD field_C;
    IVP_U_Point m_Position;
    IVP_U_Point m_Force;
    DWORD field_3C;
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
            return CKBR_ACTIVATENEXTFRAME;

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
            return CKBR_OK;

        IVP_Real_Object *obj = po->m_RealObject;

        PhysicsControllerForce *controller = new PhysicsControllerForce(obj, m_IpionManager);

        VxVector dir;
        if (directionRef)
            directionRef->TransformVector(&dir, &direction);
        else
            dir = direction;

        IVP_U_Point force(dir.x, dir.y, dir.z);
        if (force.quad_length() < 0.0001f)
            force.set(1.0f, 0.0f, 0.0f);
        else
            force.normize();

        force.mult(forceValue);
        controller->m_Force = force;

        if (referential == ent)
        {
            controller->m_Position.set(position.x, position.y, position.z);
        }
        else
        {
            VxVector pos;
            if (referential)
                referential->Transform(&pos, position);
            else
                pos = position;

            IVP_U_Point p(pos.x, pos.y, pos.z);
            IVP_U_Matrix mat;
            obj->get_core()->calc_at_matrix(obj->get_environment()->get_current_time(), &mat);
            mat.vimult4(&p, &controller->m_Position);
        }

        beh->SetLocalParameterValue(0, &controller);

        return CKBR_ACTIVATENEXTFRAME;
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
            man->m_PhysicsCallbackContainer->Process(cb);
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
