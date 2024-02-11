/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Get Profiler Values
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorGetProfilerValuesDecl();
CKERROR CreateGetProfilerValuesProto(CKBehaviorPrototype **pproto);
int GetProfilerValues(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetProfilerValuesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Profiler Values");
    od->SetDescription("Get Profiler Values");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1c8e61d1, 0x32723c6f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
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

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetProfilerValues(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKIpionManager *man = CKIpionManager::GetManager(context);

    if (beh->IsInputActive(1))
    {
        man->ResetProfiler();

        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
    }
    else
    {
        int HasPhysicsCalls = man->GetPhysicsObjectCount();
        beh->SetOutputParameterValue(0, &HasPhysicsCalls);
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    return CKBR_OK;
}