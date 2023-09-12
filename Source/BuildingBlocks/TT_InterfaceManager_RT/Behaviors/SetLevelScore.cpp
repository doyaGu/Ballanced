/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Score
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetLevelScoreDecl();
CKERROR CreateSetLevelScoreProto(CKBehaviorPrototype **pproto);
int SetLevelScore(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelScoreDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Score");
    od->SetDescription("Sets level score to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x568e2c3c, 0x24547ef7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelScoreProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelScoreProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Score");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Score", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelScore);

    *pproto = proto;
    return CK_OK;
}

int SetLevelScore(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelScore: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x14, 0);
        context->OutputToConsoleExBeep("SetLevelScore: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelScore);

    beh->ActivateOutput(0);
    return CKBR_OK;
}