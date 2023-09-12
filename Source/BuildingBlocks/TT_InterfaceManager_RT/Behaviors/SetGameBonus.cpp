/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetGameBonusDecl();
CKERROR CreateSetGameBonusProto(CKBehaviorPrototype **pproto);
int SetGameBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Bonus");
    od->SetDescription("Sets game bonus to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78587cf6, 0xddb1bb0));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Bonus");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameBonus);

    *pproto = proto;
    return CK_OK;
}

int SetGameBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetGameBonus: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x05, 0);
        context->OutputToConsoleExBeep("SetGameBonus: gameInfo not exists");

    }

    beh->GetInputParameterValue(0, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}