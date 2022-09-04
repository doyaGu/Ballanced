/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Spring
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

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
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Destroy");

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

int SetPhysicsSpring(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKIpionManager *man = CKIpionManager::GetManager(context);

    // TODO

    return CKBR_OK;
}

CKERROR SetPhysicsSpringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
