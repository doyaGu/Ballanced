/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        Physics Ball Joint
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsBallJointDecl();
CKERROR CreatePhysicsBallJointProto(CKBehaviorPrototype **pproto);
int PhysicsBallJoint(const CKBehaviorContext &behcontext);
CKERROR PhysicsBallJointCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsBallJointDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Balljoint");
    od->SetDescription("Sets a Physics Balljoint ...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5e624f0a, 0x35160450));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsBallJointProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsBallJointProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Ball Joint");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position 1", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential 1", CKPGUID_3DENTITY);

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->DeclareSetting("Specify 2 Points ?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsBallJoint);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PhysicsBallJointCallBack);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT2 0
#define POSITION1 1
#define REFERENTIAL1 2

class PhysicsBallJointCallback : public PhysicsCallback
{
public:
    PhysicsBallJointCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *object2 = (CK3dEntity *)beh->GetInputParameterObject(OBJECT2);
        if (!object2)
            return CKBR_ACTIVATENEXTFRAME;

        PhysicsObject *poR = m_IpionManager->GetPhysicsObject(ent, TRUE);
        if (!poR)
            return CKBR_OK;

        IVP_Real_Object *objR = poR->m_RealObject;

        PhysicsObject *poA = m_IpionManager->GetPhysicsObject(object2, TRUE);
        if (!poA)
            return CKBR_OK;

        IVP_Real_Object *objA = poA->m_RealObject;

        VxVector position1;
        beh->GetInputParameterValue(POSITION1, &position1);

        CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL1);

        IVP_Template_Constraint tmpl;

        VxVector pos;
        if (referential)
            referential->Transform(&pos, &position1);
        else
            pos = position1;

        IVP_U_Point anchor(pos.x, pos.y, pos.z);
        tmpl.set_ballsocket_ws(objR, &anchor, objA);

        IVP_Constraint *constraint = m_IpionManager->CreateConstraint(&tmpl);
        m_Behavior->SetLocalParameterValue(0, &constraint);

        return CKBR_ACTIVATENEXTFRAME;
    }
};

int PhysicsBallJoint(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    IVP_Constraint *constraint = NULL;
    beh->GetLocalParameterValue(0, &constraint);

    if (beh->IsInputActive(0))
    {
        if (!constraint)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            CKIpionManager *man = CKIpionManager::GetManager(context);

            PhysicsBallJointCallback *cb = new PhysicsBallJointCallback(man, beh);
            man->m_PreSimulateCallbacks->Process(cb);
        }

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        if (constraint)
        {
            delete constraint;
            constraint = NULL;
            beh->SetLocalParameterValue(0, &constraint);
        }

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}

CKERROR PhysicsBallJointCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (!beh->GetOwner())
        return CKBR_OWNERERROR;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    {
        void *handle = NULL;
        beh->SetLocalParameterValue(0, handle);
        return CKBR_OK;
    }
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        int count = beh->GetInputParameterCount();
        if (count == 3)
            return CKBR_OK;

        for (int i = count - 1; i >= 3; --i)
        {
            CKParameterIn *pin = beh->RemoveInputParameter(i);
            CKDestroyObject(pin);
        }
    }
    default:
        break;
    }

    return CKBR_OK;
}
