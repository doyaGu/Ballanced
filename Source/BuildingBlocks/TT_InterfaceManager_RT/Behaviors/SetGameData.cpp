/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetGameDataDecl();
CKERROR CreateSetGameDataProto(CKBehaviorPrototype **pproto);
int SetGameData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Data");
    od->SetDescription("Sets the game datas");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7c805c49, 0x78a601d3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Data");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Games Score", CKPGUID_INT);
    proto->DeclareInParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameData);

    *pproto = proto;
    return CK_OK;
}

int SetGameData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetGameData: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x06, 0);
        context->OutputToConsoleExBeep("SetGameData: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameScore);
    beh->GetInputParameterValue(1, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}