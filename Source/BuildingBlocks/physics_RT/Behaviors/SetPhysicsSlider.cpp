/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Slider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl();
CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **pproto);
int SetPhysicsSlider(const CKBehaviorContext &behcontext);
CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Slider");
    od->SetDescription("Set a Physics Slider...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2973360e, 0x23d31aa7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreateSetPhysicsSliderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Slider");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis first Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis second Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Limitations (meter)", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Lower Limit", CKPGUID_FLOAT, "-1.0");
    proto->DeclareInParameter("Upper Limit", CKPGUID_FLOAT, "1.0");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsSlider);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SetPhysicsSliderCallBack);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT2 0
#define AXIS_POINT1 1
#define AXIS_POINT2 2
#define LIMITATIONS 3
#define LOWER_LIMIT 4
#define UPPER_LIMIT 5

class PhysicsSliderCall : public PhysicsCallback
{
public:
    PhysicsSliderCall(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return 1;

        CK3dEntity *object2 = (CK3dEntity *)beh->GetInputParameterObject(OBJECT2);
        if (!object2)
            return 1;

        CK3dEntity *axisPoint1 = (CK3dEntity *)beh->GetInputParameterObject(AXIS_POINT1);
        CK3dEntity *axisPoint2 = (CK3dEntity *)beh->GetInputParameterObject(AXIS_POINT2);

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

        VxVector axisPosition1;
        VxVector axisPosition2;

        if (axisPoint1)
            axisPoint1->GetPosition(&axisPosition1);

        if (axisPoint2)
            axisPoint2->GetPosition(&axisPosition2);

        IVP_U_Point anchor(axisPosition1.x, axisPosition1.y, axisPosition1.z);
        VxVector ax = axisPosition2 - axisPosition1;
        IVP_U_Point axis(ax.x, ax.y, ax.z);

        tmpl.set_constraint_ws(objR, &anchor, &axis, 3, 2, objA, NULL);
        tmpl.set_constraint_ws(objR, &anchor, &axis, 2, 3, objA, NULL);

        if (limitations)
            tmpl.limit_translation_axis(IVP_INDEX_Z, lowerLimit, upperLimit);

        IVP_Constraint *constraint = m_IpionManager->CreateConstraint(&tmpl);
        m_Behavior->SetLocalParameterValue(0, &constraint);

        return 1;
    }
};

int SetPhysicsSlider(const CKBehaviorContext &behcontext)
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

            PhysicsSliderCall *cb = new PhysicsSliderCall(man, beh);
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

CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORRESET)
    {
        CKBehavior *beh = behcontext.Behavior;
        void *handle = NULL;
        beh->SetLocalParameterValue(0, &handle);
    }

    return CKBR_OK;
}
