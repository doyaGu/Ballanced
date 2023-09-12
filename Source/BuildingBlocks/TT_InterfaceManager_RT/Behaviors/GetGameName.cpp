/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetGameNameDecl();
CKERROR CreateGetGameNameProto(CKBehaviorPrototype **pproto);
int GetGameName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameNameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game Name");
    od->SetDescription("Gets game name from manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39b21b0c, 0x5db65810));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameNameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameNameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Name");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameName);

    *pproto = proto;
    return CK_OK;
}

int GetGameName(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetGameName: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x03, 0);
        context->OutputToConsoleExBeep("GetGameName: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, gameInfo->gameName);

    beh->ActivateOutput(0);
    return CKBR_OK;
}