/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level Bonus
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetLevelBonusDecl();
CKERROR CreateGetLevelBonusProto(CKBehaviorPrototype **);
int GetLevelBonus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelBonusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level Bonus");
    od->SetDescription("Gets level bonus from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x573705FA, 0x1710468E));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelBonusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelBonusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level Bonus");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelBonus);

    *pproto = proto;
    return CK_OK;
}

int GetLevelBonus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
	if (!man)
	{
		TT_ERROR("GetLevelBonus.cpp", "int GetLevelBonus(...)", " im == NULL");
		return CKBR_OK;
	}

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x0A, 0);
        TT_ERROR("GetLevelBonus.cpp", "int GetLevelBonus(...)", " gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}