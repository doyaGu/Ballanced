/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Game Score
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"
#include "ErrorProtocol.h"

CKObjectDeclaration *FillBehaviorSetGameScoreDecl();
CKERROR CreateSetGameScoreProto(CKBehaviorPrototype **pproto);
int SetGameScore(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetGameScoreDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Game Score");
    od->SetDescription("Sets game score to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2a5a3d0b, 0x4e6466c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetGameScoreProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetGameScoreProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Game Score");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Game Score", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetGameScore);

    *pproto = proto;
    return CK_OK;
}

int SetGameScore(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetGameScore.cpp", "int SetGameScore(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_NO_GAMEINFO, 0x09, 0);
        TT_ERROR("SetGameScore.cpp", "int SetGameScore(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->gameScore);

    beh->ActivateOutput(0);
    return CKBR_OK;
}