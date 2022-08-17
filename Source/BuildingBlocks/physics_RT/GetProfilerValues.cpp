/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Get Profiler Values
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorGetProfilerValuesDecl();
CKERROR CreateGetProfilerValuesProto(CKBehaviorPrototype **);
int GetProfilerValues(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetProfilerValuesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Profiler Values");
    od->SetDescription("Get Profiler Values");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1C8E61D1, 0x32723C6F));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetProfilerValuesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetProfilerValuesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Profiler Values");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Start");
    proto->DeclareInput("Reset");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("ResetOut");

    proto->DeclareOutParameter("HasPhysicsCalls", CKPGUID_INT);
    proto->DeclareOutParameter("PhysicalizeCalls", CKPGUID_INT);
    proto->DeclareOutParameter("DePhysicalizeCalls", CKPGUID_INT);
    proto->DeclareOutParameter("HasPhysicsTime", CKPGUID_FLOAT);
    proto->DeclareOutParameter("DePhysicalizeTime", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetProfilerValues);
    
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetProfilerValues(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_GetProfilerValues: pm==NULL.");
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // sub_10008770

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }
    else
    {
        // QueryPerformanceFrequency();

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}