/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set TaskSwitchHandler
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetTaskSwitchHandlerDecl();
CKERROR CreateSetTaskSwitchHandlerProto(CKBehaviorPrototype **);
int SetTaskSwitchHandler(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTaskSwitchHandlerDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set TaskSwitchHandler");
	od->SetDescription("Sends Set TaskSwitchHandler");
	od->SetCategory("TT InterfaceManager/General");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x10043A79, 0x3BA2461));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSetTaskSwitchHandlerProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateSetTaskSwitchHandlerProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set TaskSwitchHandler");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Enable TaskSwitch", CKPGUID_BOOL, "TRUE");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(SetTaskSwitchHandler);

	*pproto = proto;
	return CK_OK;
}

int SetTaskSwitchHandler(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man || !man->GetGameInfo())
	{
		TT_ERROR("SetTaskSwitchHandler.cpp", "int SetTaskSwitchHandler(...)", " gameInfo == NULL, exit CMO");
	}

	BOOL enable = TRUE;
	beh->GetInputParameterValue(0, &enable);
	man->SetTaskSwitchEnabled(enable == TRUE);
	beh->ActivateOutput(0);

	return CKBR_OK;
}