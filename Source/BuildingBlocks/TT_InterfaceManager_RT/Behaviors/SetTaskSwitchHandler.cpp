/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set TaskSwitchHandler
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetTaskSwitchHandlerDecl();
CKERROR CreateSetTaskSwitchHandlerProto(CKBehaviorPrototype **pproto);
int SetTaskSwitchHandler(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTaskSwitchHandlerDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set TaskSwitchHandler");
	od->SetDescription("Sends Set TaskSwitchHandler");
	od->SetCategory("TT InterfaceManager/General");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x10043a79, 0x3ba2461));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Terratools");
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

	InterfaceManager *man = InterfaceManager::GetManager(context);
	if (!man || !man->GetGameInfo())
	{
        context->OutputToConsoleExBeep("SetTaskSwitchHandler: gameInfo == NULL, exit CMO");
		return CKBR_OK;
	}

	CKBOOL enable = TRUE;
	beh->GetInputParameterValue(0, &enable);
	man->SetTaskSwitchEnabled(enable == TRUE);
	beh->ActivateOutput(0);

	return CKBR_OK;
}