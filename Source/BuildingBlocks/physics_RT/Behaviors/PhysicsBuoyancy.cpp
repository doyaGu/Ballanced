/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Buoyancy
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsBuoyancyDecl();
CKERROR CreatePhysicsBuoyancyProto(CKBehaviorPrototype **pproto);
int PhysicsBuoyancy(const CKBehaviorContext &behcontext);
CKERROR PhysicsBuoyancyCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsBuoyancyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("PhysicsBuoyancy");
    od->SetDescription("PhysicsBuoyancys...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2c015f2b, 0x5b147512));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePhysicsBuoyancyProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsBuoyancyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PhysicsBuoyancy");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("First Surface Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Second Surface Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Third Surface Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Current Start Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Current End Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Strength of Current (meter/sec)", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Medium Density (kg/meter^3)", CKPGUID_FLOAT, "998.0");
    proto->DeclareInParameter("Medium Damp Factor", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Medium Friction Factor", CKPGUID_FLOAT, "0.05");
    proto->DeclareInParameter("Viscosity", CKPGUID_FLOAT, ".01");
    proto->DeclareInParameter("Airplane like factor", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Suction factor", CKPGUID_FLOAT, ".1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsBuoyancy);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(PhysicsBuoyancyCallBack);

    *pproto = proto;
    return CK_OK;
}

#define SURFACE_POINT1 0
#define SURFACE_POINT2 1
#define SURFACE_POINT3 2
#define CURRENT_START_POINT 3
#define CURRENT_END_POINT 4
#define CURRENT_STRENGTH 5
#define MEDIUM_DENSITY 6
#define MEDIUM_DAMP_FACTOR 7
#define MEDIUM_FRICTION_FACTOR 8
#define VISCOSITY 9
#define AIRPLANE_LIKE_FACTOR 10
#define SUCTION_FACTOR 11

class PhysicsBuoyancyCallback : public PhysicsCallback
{
public:
    PhysicsBuoyancyCallback(CKIpionManager *man, CKBehavior *beh) : PhysicsCallback(man, beh, 2) {}

    virtual int Execute()
    {
        CKBehavior *beh = m_Behavior;

        CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
        if (!ent)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *surfacePoint1 = (CK3dEntity *)beh->GetInputParameterObject(SURFACE_POINT1);
        if (!surfacePoint1)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *surfacePoint2 = (CK3dEntity *)beh->GetInputParameterObject(SURFACE_POINT2);
        if (!surfacePoint2)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *surfacePoint3 = (CK3dEntity *)beh->GetInputParameterObject(SURFACE_POINT3);
        if (!surfacePoint3)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *startPoint = (CK3dEntity *)beh->GetInputParameterObject(CURRENT_START_POINT);
        if (!startPoint)
            return CKBR_ACTIVATENEXTFRAME;

        CK3dEntity *endPoint = (CK3dEntity *)beh->GetInputParameterObject(CURRENT_END_POINT);
        if (!endPoint)
            return CKBR_ACTIVATENEXTFRAME;

        float strength = 1.0f;
        beh->GetInputParameterValue(CURRENT_STRENGTH, &strength);

        VxVector pos1;
        VxVector pos2;
        VxVector pos3;
        VxVector startPos;
        VxVector endPos;
        surfacePoint1->GetPosition(&pos1);
        surfacePoint2->GetPosition(&pos2);
        surfacePoint3->GetPosition(&pos3);
        startPoint->GetPosition(&startPos);
        endPoint->GetPosition(&endPos);

        IVP_U_Float_Point point1(pos1.x, pos1.y, pos1.z);
        IVP_U_Float_Point point2(pos2.x, pos2.y, pos2.z);
        IVP_U_Float_Point point3(pos3.x, pos3.y, pos3.z);

        VxVector vec = endPos - startPos;
        IVP_U_Float_Point speed(vec.x, vec.y, vec.z);
        speed.fast_normize();

        float mediumDensity = 998.0f;
        beh->GetInputParameterValue(MEDIUM_DENSITY, &mediumDensity);

        float mediumDampFactor = 1.0f;
        beh->GetInputParameterValue(MEDIUM_DAMP_FACTOR, &mediumDampFactor);

        float mediumFrictionFactor = 0.05f;
        beh->GetInputParameterValue(MEDIUM_FRICTION_FACTOR, &mediumFrictionFactor);

        float viscosity = 0.01f;
        beh->GetInputParameterValue(VISCOSITY, &viscosity);

        float airplaneLikeFactor = 0.0f;
        beh->GetInputParameterValue(AIRPLANE_LIKE_FACTOR, &airplaneLikeFactor);

        float suctionFactor = 0.1f;
        beh->GetInputParameterValue(SUCTION_FACTOR, &suctionFactor);

        PhysicsObject *po = m_IpionManager->GetPhysicsObject(ent, TRUE);
        if (!po)
            return CKBR_OK;

        IVP_Real_Object *obj = po->m_RealObject;
        IVP_Template_Phantom phantom;
        phantom.manage_intruding_cores = IVP_TRUE;
        obj->convert_to_phantom(&phantom);

        IVP_U_Float_Hesse surface;
        surface.calc_hesse(&point1, &point2, &point3);
        surface.normize();

        IVP_Liquid_Surface_Descriptor_Simple *descriptor = new IVP_Liquid_Surface_Descriptor_Simple(&surface, &speed);
        m_IpionManager->m_LiquidSurfaces.add(descriptor);

        IVP_Template_Buoyancy tmpl;
        tmpl.simulate_wing_behavior = IVP_FALSE;
        tmpl.use_interpolation = IVP_FALSE;
        tmpl.pressure_damp_factor = mediumDampFactor;
        tmpl.medium_density = mediumDensity;
        tmpl.friction_damp_factor = mediumFrictionFactor;
        tmpl.buoyancy_eps = 1.0e-10f;
        tmpl.ball_rot_dampening_factor = 0.01f;
        tmpl.max_interpolation_tries = 10;
        tmpl.max_tries_nr_of_vectors_involved = 15;
        tmpl.use_stochastic_insertion = IVP_TRUE;
        tmpl.insert_extrapol_only = IVP_TRUE;
        tmpl.mi_weights.weight_current_speed = 1.0f;
        tmpl.mi_weights.weight_surface = 1.0f;
        tmpl.mi_weights.weight_rot_speed = 1.0f;
        tmpl.max_res = 0.01f;
        tmpl.nr_future_psi_for_extrapolation = 1;
        tmpl.viscosity_factor = viscosity;
        tmpl.torque_factor = airplaneLikeFactor;
        tmpl.viscosity_input_factor = suctionFactor;

        IVP_U_Set_Active<IVP_Core> *cores = obj->get_controller_phantom()->get_intruding_cores();
        new IVP_Attacher_To_Cores_Buoyancy(tmpl, cores, descriptor);

        return CKBR_ACTIVATENEXTFRAME;
    }
};

int PhysicsBuoyancy(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    PhysicsBuoyancyCallback *cb = new PhysicsBuoyancyCallback(man, beh);
    man->m_PreSimulateCallbacks->Process(cb);

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}

CKERROR PhysicsBuoyancyCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (!beh->GetOwner())
        return CKBR_OWNERERROR;

    return CKBR_OK;
}
