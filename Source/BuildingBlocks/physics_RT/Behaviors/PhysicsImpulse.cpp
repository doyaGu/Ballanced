/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Impulse
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsImpulseDecl();
CKERROR CreatePhysicsImpulseProto(CKBehaviorPrototype **pproto);
int PhysicsImpulse(const CKBehaviorContext &behcontext);
CKERROR PhysicsImpulseCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsImpulseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Impulse");
    od->SetDescription("Give a instantaneous Impulse to the object");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xc7e39bb, 0x16db20d5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsImpulseProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsImpulseProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics Impulse");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Direction Ref", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Impulse", CKPGUID_FLOAT, "10");

    proto->DeclareSetting("2 pos instead of dir ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Constant Force ?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsImpulse);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PhysicsImpulseCallBack);

    *pproto = proto;
    return CK_OK;
}

#define POSITION 0
#define REFERENTIAL 1
#define DIRECTION 2
#define DIRECTION_REF 3
#define IMPULSE 4

int PhysicsImpulse(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    CKBOOL twoPosInsteadOfDir = FALSE;
    beh->GetLocalParameterValue(0, &twoPosInsteadOfDir);

    CKBOOL isConstantForce = FALSE;
    beh->GetLocalParameterValue(1, &isConstantForce);

    VxVector position;
    beh->GetInputParameterValue(POSITION, &position);

    CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(REFERENTIAL);

    VxVector direction;
    beh->GetInputParameterValue(DIRECTION, &direction);

    CK3dEntity *directionRef = (CK3dEntity *)beh->GetInputParameterObject(DIRECTION_REF);

    float impulse = 10.0f;
    beh->GetInputParameterValue(IMPULSE, &impulse);
    if (isConstantForce)
        impulse *= man->m_PhysicsDeltaTime;

    PhysicsObject *po = man->GetPhysicsObject(ent);
    if (!po)
        return CKBR_OK;

    IVP_Real_Object *obj = po->m_RealObject;
    IVP_Core *core = obj->get_core();
    if (core->physical_unmoveable)
        return CKBR_OK;

    obj->ensure_in_simulation();

    VxVector vec;
    if (referential)
        referential->Transform(&vec, &position);
    else
        vec = position;

    IVP_U_Point pos(vec.x, vec.y, vec.z);

    IVP_U_Point dir;
    if (twoPosInsteadOfDir)
    {
        if (directionRef)
            directionRef->Transform(&vec, &direction);
        else
            vec = direction;

        dir.set(vec.x, vec.y, vec.z);
        dir.subtract(&pos);
    }
    else
    {
        if (directionRef)
            directionRef->TransformVector(&vec, &direction);
        else
            vec = direction;
        dir.set(vec.x, vec.y, vec.z);
    }
    dir.normize();
    dir.mult(impulse);

    if (referential == ent)
    {
        IVP_U_Matrix mat;
        obj->get_m_world_f_object_AT(&mat);
        IVP_U_Point ipz;
        mat.vimult3(&dir, &ipz);

        IVP_U_Float_Point p(position.x, position.y, position.z);
        IVP_U_Float_Point i(&ipz);
        IVP_U_Float_Point d(&dir);
        core->async_push_core(&p, &i, &d);
    }
    else
    {
        IVP_U_Float_Point d(&dir);
        obj->async_push_object_ws(&pos, &d);
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}

CKERROR PhysicsImpulseCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKERR_INVALIDPARAMETER;

    CK3dEntity *eno = (CK3dEntity *)beh->GetOwner();
    if (!eno)
        return CKBR_OWNERERROR;

    if (behcontext.CallbackMessage != CKM_BEHAVIORSETTINGSEDITED)
        return CKBR_OK;

    CKBOOL twoPosInsteadOfDir = FALSE;
    beh->GetLocalParameterValue(0, &twoPosInsteadOfDir);

    CKParameterIn *pin2 = beh->GetInputParameter(DIRECTION);
    bool flag = strcmp(pin2->GetName(), "Direction") == 0;
    if (!twoPosInsteadOfDir)
    {
        if (flag)
            return CKBR_OK;

        CKParameterIn *pin0 = beh->GetInputParameter(POSITION);
        pin0->SetName("Position");
        CKParameter *param0 = pin0->GetRealSource();
        if (param0)
            param0->SetName("Position");

        CKParameterIn *pin1 = beh->GetInputParameter(REFERENTIAL);
        pin1->SetName("Referential");
        CKParameter *param1 = pin1->GetRealSource();
        if (param1)
            param1->SetName("Referential");

        pin2->SetName("Direction");
        CKParameter *param2 = pin2->GetRealSource();
        if (param2)
            param2->SetName("Direction");

        CKParameterIn *pin3 = beh->GetInputParameter(DIRECTION_REF);
        pin3->SetName("Direction Ref");
        CKParameter *param3 = pin3->GetRealSource();
        if (param3)
            param3->SetName("Direction Ref");
    }
    else
    {
        if (!flag)
            return CKBR_OK;

        CKParameterIn *pin0 = beh->GetInputParameter(POSITION);
        pin0->SetName("Position 1");
        CKParameter *param0 = pin0->GetRealSource();
        if (param0)
            param0->SetName("Position 1");

        CKParameterIn *pin1 = beh->GetInputParameter(REFERENTIAL);
        pin1->SetName("Referential 1");
        CKParameter *param1 = pin1->GetRealSource();
        if (param1)
            param1->SetName("Referential 1");

        pin2->SetName("Position 2");
        CKParameter *param2 = pin2->GetRealSource();
        if (param2)
            param2->SetName("Position 2");

        CKParameterIn *pin3 = beh->GetInputParameter(DIRECTION_REF);
        pin3->SetName("Referential 2");
        CKParameter *param3 = pin3->GetRealSource();
        if (param3)
            param3->SetName("Referential 2");
    }

    return CKBR_OK;
}
