/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetLevelIDDecl();
CKERROR CreateSetLevelIDProto(CKBehaviorPrototype **pproto);
int SetLevelID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level ID");
    od->SetDescription("Sets level ID to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x20b86a4c, 0x3c62472a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level ID");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelID);

    *pproto = proto;
    return CK_OK;
}

int SetLevelID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelID: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x12, 0);
        context->OutputToConsoleExBeep("SetLevelID: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}