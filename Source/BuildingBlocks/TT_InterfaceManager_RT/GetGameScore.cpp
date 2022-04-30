/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Score
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetGameScoreDecl();
CKERROR CreateGetGameScoreProto(CKBehaviorPrototype **);
int GetGameScore(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameScoreDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game Score");
	od->SetDescription("Gets game score to manager");
	od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x34951557, 0x4D180895));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateGetGameScoreProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateGetGameScoreProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Score");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareOutParameter("Game Score", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(GetGameScore);

	*pproto = proto;
	return CK_OK;
}

int GetGameScore(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("GetGameScore.cpp", "int GetGameScore(...)", " im == NULL");
		return CKBR_OK;
	}

    CGameInfo *gameInfo = man->GetGameInfo();
	if (!gameInfo)
	{
		::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x04, 0);
		TT_ERROR("GetGameScore.cpp", "int GetGameScore(...)", " gameInfo not exists");
	}

    beh->SetOutputParameterValue(0, &gameInfo->gameScore);

    beh->ActivateOutput(0);
    return CKBR_OK;
}