/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetLevelDataDecl();
CKERROR CreateSetLevelDataProto(CKBehaviorPrototype **pproto);
int SetLevelData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Data");
    od->SetDescription("Sets the level data");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2030ea1, 0x30182971));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Data");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Score", CKPGUID_INT);
    proto->DeclareInParameter("Level Bonus", CKPGUID_INT);
    proto->DeclareInParameter("Level Reached", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelData);

    *pproto = proto;
    return CK_OK;
}

int SetLevelData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelData: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x11, 0);
        context->OutputToConsoleExBeep("SetLevelData: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelScore);
    beh->GetInputParameterValue(1, &gameInfo->levelBonus);
    beh->GetInputParameterValue(2, &gameInfo->levelReached);

    beh->ActivateOutput(0);
    return CKBR_OK;
}