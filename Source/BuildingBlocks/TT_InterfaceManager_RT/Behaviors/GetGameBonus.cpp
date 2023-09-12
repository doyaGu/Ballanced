/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetGameBonusDecl();
CKERROR CreateGetGameBonusProto(CKBehaviorPrototype **pproto);
int GetGameBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game Bonus");
    od->SetDescription("Gets game bonus from manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12480cb5, 0x4816349f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Bonus");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameBonus);

    *pproto = proto;
    return CK_OK;
}

int GetGameBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetGameBonus: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x00, 0);
        context->OutputToConsoleExBeep("GetGameBonus: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}