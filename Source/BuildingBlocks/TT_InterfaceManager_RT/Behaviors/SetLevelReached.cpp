/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Reached
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetLevelReachedDecl();
CKERROR CreateSetLevelReachedProto(CKBehaviorPrototype **pproto);
int SetLevelReached(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelReachedDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Reached");
    od->SetDescription("Sets level reached to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xcf151b2, 0xb1b6001));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelReachedProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelReachedProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Reached");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Reached", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelReached);

    *pproto = proto;
    return CK_OK;
}

int SetLevelReached(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelReached: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x15, 0);
        context->OutputToConsoleExBeep("SetLevelReached: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelReached);

    beh->ActivateOutput(0);
    return CKBR_OK;
}