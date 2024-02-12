/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Spring
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsSpringDecl();
CKERROR CreateSetPhysicsSpringProto(CKBehaviorPrototype **pproto);
int SetPhysicsSpring(const CKBehaviorContext &behcontext);
CKERROR SetPhysicsSpringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsSpringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Spring");
    od->SetDescription("Set a Physics Springs...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x24a06a3a, 0x7100fce));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreateSetPhysicsSpringProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetPhysicsSpringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Spring");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position 1", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position 2", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential 2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Length", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Constant", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Linear Dampening", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Global Dampening", CKPGUID_FLOAT, "0.1");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsSpring);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SetPhysicsSpringCallBack);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT2 0
#define POSITION1 1
#define REFERENTIAL1 2
#define POSITION2 3
#define REFERENTIAL2 4
#define LENGTH 5
#define CONSTANT 6
#define LINEAR_DAMPENING 7
#define GLOBAL_DAMPENING 8

class PhysicsSpringCall : public PhysicsCallback
{
public:
    PhysicsSpringCall(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return 1;

        CK3dEntity *object2 = (CK3dEntity *)beh->GetInputParameterObject(OBJECT2);
        if (!object2)
            return 0;

        PhysicsObject *poR = m_IpionManager->GetPhysicsObject(ent, TRUE);
        if (!poR)
            return 0;

        IVP_Real_Object *objR = poR->m_RealObject;

        PhysicsObject *poA = m_IpionManager->GetPhysicsObject(object2, TRUE);
        if (!poA)
            return 0;

        IVP_Real_Object *objA = poA->m_RealObject;

        VxVector position1;
        beh->GetInputParameterValue(POSITION1, &position1);

        CK3dEntity *referential1 = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL1);

        VxVector position2;
        beh->GetInputParameterValue(POSITION2, &position2);

        CK3dEntity *referential2 = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL2);

        float length = 1.0f;
        beh->GetInputParameterValue(LENGTH, &length);

        float constant = 1.0f;
        beh->GetInputParameterValue(CONSTANT, &constant);

        float linearDampening = 0.1f;
        beh->GetInputParameterValue(LINEAR_DAMPENING, &linearDampening);

        float globalDampening = 0.1f;
        beh->GetInputParameterValue(GLOBAL_DAMPENING, &globalDampening);

        IVP_Template_Spring tmpl;
        tmpl.spring_constant = constant;
        tmpl.spring_values_are_relative = IVP_FALSE;
        tmpl.spring_force_only_on_stretch = IVP_FALSE;
        tmpl.spring_damp = linearDampening;
        tmpl.spring_len = length;
        tmpl.rel_pos_damp = globalDampening;

        VxVector pos1;
        if (referential1)
            referential1->Transform(&pos1, &position1);
        else
            pos1 = position1;

        IVP_U_Point anchorPos1(pos1.x, pos1.y, pos1.z);

        IVP_Template_Anchor anchor1;
        anchor1.set_anchor_position_ws(objR, &anchorPos1);

        VxVector pos2;
        if (referential2)
            referential2->Transform(&pos2, &position2);
        else
            pos2 = position2;

        IVP_U_Point anchorPos2(pos2.x, pos2.y, pos2.z);

        IVP_Template_Anchor anchor2;
        anchor2.set_anchor_position_ws(objA, &anchorPos2);

        tmpl.anchors[0] = &anchor1;
        tmpl.anchors[1] = &anchor2;

        IVP_Actuator_Spring *spring = m_IpionManager->CreateSpring(&tmpl);
        m_Behavior->SetLocalParameterValue(0, &spring);

        return 1;
    }
};

int SetPhysicsSpring(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    IVP_Actuator_Spring *spring = NULL;
    beh->GetLocalParameterValue(0, &spring);

    if (beh->IsInputActive(0))
    {
        if (!spring)
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return CKBR_OWNERERROR;

            CKIpionManager *man = CKIpionManager::GetManager(context);

            PhysicsSpringCall *cb = new PhysicsSpringCall(man, beh);
            man->m_PreSimulateCallbacks->Process(cb);
        }

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }
    else
    {
        if (spring)
        {
            delete spring;
            spring = NULL;
            beh->SetLocalParameterValue(0, &spring);
        }

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
    }

    return CKBR_OK;
}

CKERROR SetPhysicsSpringCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}
