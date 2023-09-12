/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetLevelIDDecl();
CKERROR CreateGetLevelIDProto(CKBehaviorPrototype **pproto);
int GetLevelID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level ID");
    od->SetDescription("Gets level ID from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x649011fe, 0x437d5918));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level ID");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelID);

    *pproto = proto;
    return CK_OK;
}

int GetLevelID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("GetLevelID: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x0E, 0);
        context->OutputToConsoleExBeep("GetLevelID: gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}