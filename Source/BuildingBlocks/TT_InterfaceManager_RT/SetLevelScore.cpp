/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Score
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetLevelScoreDecl();
CKERROR CreateSetLevelScoreProto(CKBehaviorPrototype **);
int SetLevelScore(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelScoreDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Score");
    od->SetDescription("Sets level score to manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x568E2C3C, 0x24547EF7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelScoreProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelScoreProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Score");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Score", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelScore);

    *pproto = proto;
    return CK_OK;
}

int SetLevelScore(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetLevelScore.cpp", "int SetLevelScore(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriver())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x14, 0);
        TT_ERROR("SetLevelScore.cpp", "int SetLevelScore(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelScore);

    beh->ActivateOutput(0);
    return CKBR_OK;
}