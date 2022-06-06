/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetGameBonusDecl();
CKERROR CreateSetGameBonusProto(CKBehaviorPrototype **);
int SetGameBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Bonus");
    od->SetDescription("Sets game bonus to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78587CF6, 0xDDB1BB0));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Bonus");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameBonus);

    *pproto = proto;
    return CK_OK;
}

int SetGameBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetGameBonus.cpp", "int SetGameBonus(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriver())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x05, 0);
        TT_ERROR("SetGameBonus.cpp", "int SetGameBonus(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}