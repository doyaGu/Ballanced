/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Restart CMO
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSendCMORestartDecl();
CKERROR CreateSendCMORestartProto(CKBehaviorPrototype **);
int SendCMORestart(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSendCMORestartDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restart CMO");
	od->SetDescription("Restarts CMO");
	od->SetCategory("TT InterfaceManager/General");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0xB585F6F, 0x46253C21));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSendCMORestartProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateSendCMORestartProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restart CMO");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(SendCMORestart);

	*pproto = proto;
	return CK_OK;
}

int SendCMORestart(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man || !man->GetGameInfo())
	{
		TT_ERROR("SendCMORestart.cpp", "int SendCMORestart(...)", " gameInfo == NULL, exit CMO");
	}

	::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_CMO_RESTART, 0, 0);
	beh->ActivateOutput(0);

	return CKBR_OK;
}