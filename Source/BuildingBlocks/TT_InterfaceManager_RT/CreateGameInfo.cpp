/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Create new GameInfo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorCreateGameInfoDecl();
CKERROR CreateCreateGameInfoProto(CKBehaviorPrototype **);
int CreateGameInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateGameInfoDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Create new GameInfo");
	od->SetDescription("Creates new GameInfo");
	od->SetCategory("TT InterfaceManager/General");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x0A8B6C27, 0x402B43B4));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateCreateGameInfoProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

CKERROR CreateCreateGameInfoProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Create new GameInfo");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Registry Path", CKPGUID_STRING);

	proto->DeclareOutParameter("Address of GameInfo (hex)", CKPGUID_INT);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(CreateGameInfo);

	*pproto = proto;
	return CK_OK;
}

int CreateGameInfo(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *context = behcontext.Context;

	CKSTRING path = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man || !man->GetGameInfo())
	{
		CGameInfo *gameInfo = new CGameInfo;
		strcpy(gameInfo->regSubkey, path);
		man->SetGameInfo(gameInfo);
		beh->SetOutputParameterValue(0, &gameInfo, sizeof(CGameInfo *));
	}

	beh->ActivateOutput(0);
	return CKBR_OK;
}