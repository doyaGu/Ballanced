/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetLevelNameDecl();
CKERROR CreateSetLevelNameProto(CKBehaviorPrototype **pproto);
int SetLevelName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelNameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Name");
    od->SetDescription("Sets level name to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7aec3d66, 0x4aa904c5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelNameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelNameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Name");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Name", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelName);

    *pproto = proto;
    return CK_OK;
}

int SetLevelName(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelName: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x13, 0);
        context->OutputToConsoleExBeep("SetLevelName: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, gameInfo->levelName);

    beh->ActivateOutput(0);
    return CKBR_OK;
}