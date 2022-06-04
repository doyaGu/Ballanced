/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Physics Impulse
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorPhysicsImpulseDecl();
CKERROR CreatePhysicsImpulseProto(CKBehaviorPrototype **);
int PhysicsImpulse(const CKBehaviorContext &behcontext);
CKERROR PhysicsImpulseCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPhysicsImpulseDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Physics Impulse");
    od->SetDescription("Give a instantaneous Impulse to the object");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xC7E39BB, 0x16DB20D5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
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

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_TARGETABLE));
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
    {
        return CKBR_OWNERERROR;
    }

    CKPhysicsManager *man = CKPhysicsManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_PhysicsImpulse: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR PhysicsImpulseCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
