/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT Change ScreenMode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorChangeScreenModeDecl();
CKERROR CreateChangeScreenModeProto(CKBehaviorPrototype **);
int ChangeScreenMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChangeScreenModeDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Change ScreenMode");
	od->SetDescription("Change Screenmode");
	od->SetCategory("TT InterfaceManager/Display");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x38B84D97, 0x13932F28));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateChangeScreenModeProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateChangeScreenModeProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Change ScreenMode");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");
	proto->DeclareOutput("Error");

	proto->DeclareInParameter("Driver ID", CKPGUID_INT);
	proto->DeclareInParameter("ScreenMode ID", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(ChangeScreenMode);

	*pproto = proto;
	return CK_OK;
}

int ChangeScreenMode(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	int driverId = 0;
	int screenModeId = 0;
	beh->GetInputParameterValue(0, &driverId);
	beh->GetInputParameterValue(1, &screenModeId);

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("ChangeScreenMode.cpp", "int ChangeScreenMode(...)", " im == NULL");
		beh->ActivateOutput(1);
		return CKBR_OK;
	}

	VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(driverId);
	if (!drDesc)
	{
		context->OutputToConsoleExBeep("Change Screen Mode: No Driver Description for Driver-ID '%d' is found", driverId);
		beh->ActivateOutput(1);
		return CKBR_OK;
	}

	if (screenModeId >= drDesc->DisplayModeCount ||
		(screenModeId & 0x80000000) != 0 ||
		drDesc->DisplayModes[screenModeId].Bpp < 16)
	{
		context->OutputToConsoleExBeep("Change Screen Mode: ScreenMode is not exist");
		beh->ActivateOutput(1);
		return CKBR_OK;
	}

	if (!::SendMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_SCREEN_MODE_CHG, screenModeId, driverId))
	{
		beh->ActivateOutput(1);
		return CKBR_OK;
	}

	beh->ActivateOutput(0);
	return CKBR_OK;
}