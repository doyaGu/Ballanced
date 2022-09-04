/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        Physics Ball Joint
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorPhysicsBallJointDecl();
CKERROR CreatePhysicsBallJointProto(CKBehaviorPrototype **pproto);
int PhysicsBallJoint(const CKBehaviorContext &behcontext);
CKERROR PhysicsBallJointCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsBalljointDecl()
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
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Destroy");

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

int PhysicsBallJoint(const CKBehaviorContext &behcontext)
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
        context->OutputToConsoleExBeep("TT_PhysicsBallJoint: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsBallJointCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
