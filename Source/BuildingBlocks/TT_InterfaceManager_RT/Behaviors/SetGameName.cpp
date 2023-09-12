/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetGameNameDecl();
CKERROR CreateSetGameNameProto(CKBehaviorPrototype **pproto);
int SetGameName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameNameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Name");
    od->SetDescription("Sets game name to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30641426, 0x12403bb9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameNameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameNameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Name");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game Name", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameName);

    *pproto = proto;
    return CK_OK;
}

int SetGameName(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetGameName: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x08, 0);
        context->OutputToConsoleExBeep("SetGameName: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, gameInfo->gameName);

    beh->ActivateOutput(0);
    return CKBR_OK;
}