/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetGameDataDecl();
CKERROR CreateGetGameDataProto(CKBehaviorPrototype **pproto);
int GetGameData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT  Get Game Data");
    od->SetDescription("Gets the game datas");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x16875322, 0x72422c5c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Data");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Games Score", CKPGUID_INT);
    proto->DeclareOutParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameData);

    *pproto = proto;
    return CK_OK;
}

int GetGameData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetGameData: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x01, 0);
        context->OutputToConsoleExBeep("GetGameData: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->gameScore);
    beh->SetOutputParameterValue(1, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}