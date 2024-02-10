/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Hinge
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsHingeDecl();
CKERROR CreatePhysicsHingeProto(CKBehaviorPrototype **pproto);
int PhysicsHinge(const CKBehaviorContext &behcontext);
CKERROR PhysicsHingeCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsHingeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Hinge");
    od->SetDescription("Set a Physics Hinge joint...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x41cd3653, 0xde60c1d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsHingeProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsHingeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Hinge");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Joint Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Limitations (-180 to 180 degree)", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Lower Limit", CKPGUID_FLOAT, "-45.0");
    proto->DeclareInParameter("Upper Limit", CKPGUID_FLOAT, "45.0");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsHinge);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PhysicsHingeCallBack);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT2 0
#define REFERENTIAL 1
#define LIMITATIONS 2
#define LOWER_LIMIT 3
#define UPPER_LIMIT 4

class PhysicsHingeCallback : public PhysicsCallback
{
public:
    PhysicsHingeCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return 1;

        CK3dEntity *object2 = (CK3dEntity *)beh->GetInputParameterObject(OBJECT2);
        if (!object2)
            return 1;

        CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL);

        CKBOOL limitations = FALSE;
        beh->GetInputParameterValue(LIMITATIONS, &limitations);

        float lowerLimit;
        beh->GetInputParameterValue(LOWER_LIMIT, &lowerLimit);

        float upperLimit;
        beh->GetInputParameterValue(UPPER_LIMIT, &upperLimit);

        PhysicsObject *poR = m_IpionManager->GetPhysicsObject(ent, TRUE);
        if (!poR)
            return 0;

        IVP_Real_Object *objR = poR->m_RealObject;

        PhysicsObject *poA = m_IpionManager->GetPhysicsObject(object2, TRUE);
        if (!poA)
            return 0;

        IVP_Real_Object *objA = poA->m_RealObject;

        IVP_Template_Constraint tmpl;

        VxVector dir;
        VxVector pos;
        if (referential)
        {
            referential->GetOrientation(&dir, &pos);
            referential->GetPosition(&pos);
        }

        IVP_U_Point anchor(pos.x, pos.y, pos.z);
        IVP_U_Point axis(dir.x, dir.y, dir.z);

        tmpl.set_hinge_ws(objR, &anchor, &axis, objA);

        if (limitations)
        {
            float ll = lowerLimit * (PI / 180);
            float ul = upperLimit * (PI / 180);
            tmpl.limit_rotation_axis(IVP_INDEX_Z, ll, ul);
        }

        IVP_Constraint *constraint = m_IpionManager->CreateConstraint(&tmpl);
        m_Behavior->SetLocalParameterValue(0, &constraint);

        return 1;
    }
};

int PhysicsHinge(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    IVP_Constraint *constraint = NULL;
    beh->GetLocalParameterValue(0, &constraint);

    if (beh->IsInputActive(0))
    {
        if (constraint)
        {
            delete constraint;
            constraint = NULL;
            beh->SetLocalParameterValue(0, &constraint);
        }

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return CKBR_OWNERERROR;

        CKIpionManager *man = CKIpionManager::GetManager(context);

        PhysicsHingeCallback *cb = new PhysicsHingeCallback(man, beh);
        man->m_PreSimulateCallbacks->Process(cb);

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

CKERROR PhysicsHingeCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}