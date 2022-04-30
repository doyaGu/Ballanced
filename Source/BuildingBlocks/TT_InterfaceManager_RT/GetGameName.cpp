/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetGameNameDecl();
CKERROR CreateGetGameNameProto(CKBehaviorPrototype **);
int GetGameName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameNameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game Name");
    od->SetDescription("Gets game name to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39B21B0C, 0x5DB65810));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameNameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameNameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game Name");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameName);

    *pproto = proto;
    return CK_OK;
}

int GetGameName(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

	CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("GetGameName.cpp", "int GetGameName(...)", " im == NULL");
		return CKBR_OK;
	}

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x03, 0);
        TT_ERROR("GetGameName.cpp", "int GetGameName(...)", " gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, gameInfo->gameName);

    beh->ActivateOutput(0);
    return CKBR_OK;
}