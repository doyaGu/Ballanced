//////////////////////////////////////
//////////////////////////////////////
//
//        TT GetMemoryStatus
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetMemoryStatusDecl();
CKERROR CreateGetMemoryStatusProto(CKBehaviorPrototype **pproto);
int GetMemoryStatus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetMemoryStatusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetMemoryStatus");
    od->SetDescription("Reads system memory status");
    od->SetCategory("TT Toolbox/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3b826e04, 0x6e764285));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetMemoryStatusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetMemoryStatusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetMemoryStatus");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("MemoryLoad", CKPGUID_INT);
    proto->DeclareOutParameter("Total Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Total Virtual Address Space", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Virtual Address Space", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetMemoryStatus);

    *pproto = proto;
    return CK_OK;
}

int GetMemoryStatus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    MEMORYSTATUS statex;
    statex.dwLength = sizeof(statex);
    ::GlobalMemoryStatus(&statex);

    int memoryLoad = statex.dwMemoryLoad;
    beh->SetOutputParameterValue(0, &memoryLoad, sizeof(int));

    float dwTotalPhys = statex.dwTotalPhys / (1024.0f * 1024.0f);
    beh->SetOutputParameterValue(1, &dwTotalPhys, sizeof(float));

    float dwAvailPhys = statex.dwAvailPhys / (1024.0f * 1024.0f);
    beh->SetOutputParameterValue(2, &dwAvailPhys, sizeof(float));

    float dwTotalVirtual = statex.dwTotalVirtual / (1024.0f * 1024.0f);
    beh->SetOutputParameterValue(3, &dwTotalVirtual, sizeof(float));

    float dwAvailVirtual = statex.dwAvailVirtual / (1024.0f * 1024.0f);
    beh->SetOutputParameterValue(4, &dwAvailVirtual, sizeof(float));

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}