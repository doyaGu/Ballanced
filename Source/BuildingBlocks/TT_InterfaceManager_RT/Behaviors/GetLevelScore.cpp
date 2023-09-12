/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level Score
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetLevelScoreDecl();
CKERROR CreateGetLevelScoreProto(CKBehaviorPrototype **pproto);
int GetLevelScore(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelScoreDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level Score");
    od->SetDescription("Gets Level score from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7fe65a65, 0x490a49b5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelScoreProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelScoreProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level Score");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level Score", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelScore);

    *pproto = proto;
    return CK_OK;
}

int GetLevelScore(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetLevelScore: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x0E, 0);
        context->OutputToConsoleExBeep("GetLevelScore: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelScore);

    beh->ActivateOutput(0);
    return CKBR_OK;
}