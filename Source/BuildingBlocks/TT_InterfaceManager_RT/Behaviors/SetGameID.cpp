/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetGameIDDecl();
CKERROR CreateSetGameIDProto(CKBehaviorPrototype **pproto);
int SetGameID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game ID");
    od->SetDescription("Sets game ID to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x36af3944, 0x67c75aa7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game ID");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameID);

    *pproto = proto;
    return CK_OK;
}

int SetGameID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetGameID: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x07, 0);
        context->OutputToConsoleExBeep("SetGameID: gameInfo not exists");

    }

    beh->GetInputParameterValue(0, &gameInfo->gameID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}