/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Limit Framerate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorLimitFramerateDecl();
CKERROR CreateLimitFramerateProto(CKBehaviorPrototype **);
int LimitFramerate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLimitFramerateDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Limit Framerate");
	od->SetDescription("Limits the framerate.");
	od->SetCategory("TT InterfaceManager/General");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x12A40E8F, 0x6F2D0613));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateLimitFramerateProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateLimitFramerateProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Limit Framerate");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Framerate", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(LimitFramerate);

	*pproto = proto;
	return CK_OK;
}

int LimitFramerate(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

    int framerate = 60;
    beh->GetInputParameterValue(0, &framerate);

	::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_LIMIT_FPS, framerate, 0);
	
    beh->ActivateInput(0, FALSE);
	beh->ActivateOutput(0, TRUE);

	return CKBR_OK;
}