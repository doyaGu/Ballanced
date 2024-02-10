/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Globals
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsGlobalsDecl();
CKERROR CreateSetPhysicsGlobalsProto(CKBehaviorPrototype **pproto);
int SetPhysicsGlobals(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsGlobalsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Globals");
    od->SetDescription("Set the Physics Global Variables...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x72af347c, 0x3da71e1));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
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
    proto->DeclareInParameter("Physic Time Factor", CKPGUID_FLOAT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsGlobals);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetPhysicsGlobals(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    VxVector gravity(0.0f, -9.81f, 0.0f);
    beh->GetInputParameterValue(0, &gravity);

    float physicsTimeFactor = 1.0f;
    beh->GetInputParameterValue(1, &physicsTimeFactor);

    CKIpionManager *man = CKIpionManager::GetManager(context);

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        if (man)
        {
            man->SetTimeFactor(physicsTimeFactor);
            man->SetGravity(gravity);
        }
    }
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        if (man)
        {
            man->DestroyEnvironment();
            man->CreateEnvironment();
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}