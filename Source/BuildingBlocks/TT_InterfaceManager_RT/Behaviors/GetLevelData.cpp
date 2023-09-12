/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetLevelDataDecl();
CKERROR CreateGetLevelDataProto(CKBehaviorPrototype **);
int GetLevelData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level Data");
    od->SetDescription("Gets the level data");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6bca40f9, 0x42641a24));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level Data");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level Score", CKPGUID_INT);
    proto->DeclareOutParameter("Level Bonus", CKPGUID_INT);
    proto->DeclareOutParameter("Level Reached", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelData);

    *pproto = proto;
    return CK_OK;
}

int GetLevelData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetLevelData: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x0B, 0);
        context->OutputToConsoleExBeep("GetLevelData: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelScore);
    beh->SetOutputParameterValue(1, &gameInfo->levelBonus);
    beh->SetOutputParameterValue(2, &gameInfo->levelReached);

    beh->ActivateOutput(0);
    return CKBR_OK;
}