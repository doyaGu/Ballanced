/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetGameIDDecl();
CKERROR CreateGetGameIDProto(CKBehaviorPrototype **);
int GetGameID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game ID");
    od->SetDescription("Gets game ID from manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x63527b75, 0x1f9a40d9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game ID");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameID);

    *pproto = proto;
    return CK_OK;
}

int GetGameID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetGameID: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x02, 0);
        context->OutputToConsoleExBeep("GetGameID: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->gameID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}