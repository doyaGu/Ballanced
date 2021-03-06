/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Globals
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "PhysicsManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsGlobalsDecl();
CKERROR CreateSetPhysicsGlobalsProto(CKBehaviorPrototype **);
int SetPhysicsGlobals(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsGlobalsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Globals");
    od->SetDescription("Set the Physics Global Variables...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x72AF347C, 0x3DA71E1));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPhysicsGlobalsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetPhysicsGlobalsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Globals");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Set Values");
    proto->DeclareInput("Clean Physics World");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Gravity", CKPGUID_VECTOR, "0,-9.81,0");
    proto->DeclareInParameter("Physics Time Factor", CKPGUID_FLOAT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsGlobals);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetPhysicsGlobals(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKPhysicsManager *man = CKPhysicsManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetPhysicsGlobals: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}