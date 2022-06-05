/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetGameDataDecl();
CKERROR CreateSetGameDataProto(CKBehaviorPrototype **);
int SetGameData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Data");
    od->SetDescription("Sets the game datas");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7C805C49, 0x78A601D3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Data");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game Score", CKPGUID_INT);
    proto->DeclareInParameter("Game Bonus", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameData);

    *pproto = proto;
    return CK_OK;
}

int SetGameData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetGameData.cpp", "int SetGameData(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x06, 0);
        TT_ERROR("SetGameData.cpp", "int SetGameData(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameScore);
    beh->GetInputParameterValue(1, &gameInfo->gameBonus);

    beh->ActivateOutput(0);
    return CKBR_OK;
}