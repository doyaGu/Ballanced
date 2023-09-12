/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level Reached
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetLevelReachedDecl();
CKERROR CreateGetLevelReachedProto(CKBehaviorPrototype **pproto);
int GetLevelReached(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelReachedDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level Reached");
    od->SetDescription("Gets Level reached from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e8344e0, 0x5101432));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelReachedProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelReachedProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level Reached");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level Reached", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelReached);

    *pproto = proto;
    return CK_OK;
}

int GetLevelReached(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetLevelReached: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x0F, 0);
        context->OutputToConsoleExBeep("GetLevelReached: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelReached);

    beh->ActivateOutput(0);
    return CKBR_OK;
}