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
    if (!proto) return CKERR_OUTOFMEMORY;

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
#define JOINT_REFERENTIAL 1
#define LIMITATIONS 2
#define LOWER_LIMIT 3
#define UPPER_LIMIT 4

int PhysicsHinge(const CKBehaviorContext &behcontext)
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
        context->OutputToConsoleExBeep("TT_PhysicsHinge: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsHingeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
