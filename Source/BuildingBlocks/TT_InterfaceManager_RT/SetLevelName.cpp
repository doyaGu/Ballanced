/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetLevelNameDecl();
CKERROR CreateSetLevelNameProto(CKBehaviorPrototype **);
int SetLevelName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelNameDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Name");
	od->SetDescription("Sets level name to manager");
	od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x7AEC3D66, 0x4AA904C5));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSetLevelNameProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateSetLevelNameProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Name");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Level Name", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(SetLevelName);

	*pproto = proto;
	return CK_OK;
}

int SetLevelName(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetLevelName.cpp", "int SetLevelName(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
	if (!gameInfo)
	{
		::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x13, 0);
		TT_ERROR("SetLevelName.cpp", "int SetLevelName(...)", " gameInfo not exists");
	}

	beh->GetInputParameterValue(0, gameInfo->levelName);

	beh->ActivateOutput(0);
	return CKBR_OK;
}