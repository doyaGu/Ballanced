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
    if (!proto) return CKERR_OUTOFMEMORY;

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

int PhysicsBuoyancy(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsBuoyancy: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsBuoyancyCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
