/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetLevelIDDecl();
CKERROR CreateSetLevelIDProto(CKBehaviorPrototype **);
int SetLevelID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelIDDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level ID");
	od->SetDescription("Sets level ID to manager");
	od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x20B86A4C, 0x3C62472A));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSetLevelIDProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateSetLevelIDProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level ID");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Level ID", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(SetLevelID);

	*pproto = proto;
	return CK_OK;
}

int SetLevelID(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetLevelID.cpp", "int SetLevelID(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
	if (!gameInfo)
	{
		::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x12, 0);
		TT_ERROR("SetLevelID.cpp", "int SetLevelID(...)", " gameInfo not exists");
	}

	beh->GetInputParameterValue(0, &gameInfo->levelID);

	beh->ActivateOutput(0);
	return CKBR_OK;
}