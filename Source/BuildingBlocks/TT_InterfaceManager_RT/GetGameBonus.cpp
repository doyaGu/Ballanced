/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetGameBonusDecl();
CKERROR CreateGetGameBonusProto(CKBehaviorPrototype **);
int GetGameBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game Bonus");
    od->SetDescription("Gets game bonus to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12480CB5, 0x4816349F));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Bonus");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameBonus);

    *pproto = proto;
    return CK_OK;
}

int GetGameBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("GetGameBonus.cpp", "int GetGameBonus(...)", " im == NULL");
		return CKBR_OK;
	}

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x00, 0);
        TT_ERROR("GetGameBonus.cpp", "int GetGameBonus(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}