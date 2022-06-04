/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Spring
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorPhysicsSpringDecl();
CKERROR CreatePhysicsSpringProto(CKBehaviorPrototype **);
int PhysicsSpring(const CKBehaviorContext &behcontext);
CKERROR PhysicsSpringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsSpringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Spring");
    od->SetDescription("Set a Physics Springs...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x24A06A3A, 0x7100FCE));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreatePhysicsSpringProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreatePhysicsSpringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Physics Spring");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Destroy");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position 1", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position 2", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential 2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Length", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Constant", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Linear Dampening", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Global Dampening", CKPGUID_FLOAT, "0.1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PhysicsSpring);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PhysicsSpringCallBack);

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

int PhysicsSpring(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKPhysicsManager *man = CKPhysicsManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsSpring: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsSpringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
