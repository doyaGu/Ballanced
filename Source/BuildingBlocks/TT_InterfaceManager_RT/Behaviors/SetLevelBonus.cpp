/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
CKObjectDeclaration *FillBehaviorSetLevelBonusDecl();
CKERROR CreateSetLevelBonusProto(CKBehaviorPrototype **pproto);
int SetLevelBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Bonus");
    od->SetDescription("Sets level bonus to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f9b6fa1, 0x57040b1b));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Bonus");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelBonus);

    *pproto = proto;
    return CK_OK;
}

int SetLevelBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("SetLevelBonus: im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x10, 0);
        context->OutputToConsoleExBeep("SetLevelBonus: gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}