/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Restart CMO
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorRestartCMODecl();
CKERROR CreateRestartCMOProto(CKBehaviorPrototype **pproto);
int RestartCMO(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRestartCMODecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restart CMO");
    od->SetDescription("Restarts CMO");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xb585f6f, 0x46253c21));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRestartCMOProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRestartCMOProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restart CMO");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RestartCMO);

    *pproto = proto;
    return CK_OK;
}

int RestartCMO(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man || !man->GetGameInfo())
        context->OutputToConsoleExBeep("RestartCMO: gameInfo == NULL, exit CMO");

    ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_CMO_RESTART, 0, 0);
    beh->ActivateOutput(0);

    return CKBR_OK;
}