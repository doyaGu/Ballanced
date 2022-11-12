//////////////////////////////////////
//////////////////////////////////////
//
//        TT OperationSystem
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <windows.h>

CKObjectDeclaration *FillBehaviorOperationSystemDecl();
CKERROR CreateOperationSystemProto(CKBehaviorPrototype **pproto);
int OperationSystem(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorOperationSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT OperationSystem");
    od->SetDescription("Reads operation system");
    od->SetCategory("TT Toolbox/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c94621d, 0x24fe2cf3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateOperationSystemProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateOperationSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT OperationSystem");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("PlattformId", CKPGUID_INT);
    proto->DeclareOutParameter("MinorVersion", CKPGUID_INT);
    proto->DeclareOutParameter("MajorVersion", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(OperationSystem);

    *pproto = proto;
    return CK_OK;
}

int OperationSystem(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    OSVERSIONINFO osvi;
    ::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    ::GetVersionEx(&osvi);

    beh->SetOutputParameterValue(0, &osvi.dwPlatformId, sizeof(DWORD));
    beh->SetOutputParameterValue(1, &osvi.dwMinorVersion, sizeof(DWORD));
    beh->SetOutputParameterValue(2, &osvi.dwMajorVersion, sizeof(DWORD));

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}