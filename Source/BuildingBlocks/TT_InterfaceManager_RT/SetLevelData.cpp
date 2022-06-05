/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Level Data
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetLevelDataDecl();
CKERROR CreateSetLevelDataProto(CKBehaviorPrototype **);
int SetLevelData(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLevelDataDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Level Data");
    od->SetDescription("Sets the level datas");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2030EA1, 0x30182971));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLevelDataProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetLevelDataProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Level Data");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Level Score", CKPGUID_INT);
    proto->DeclareInParameter("Level Bonus", CKPGUID_INT);
    proto->DeclareInParameter("Level Reached", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLevelData);

    *pproto = proto;
    return CK_OK;
}

int SetLevelData(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("SetLevelData.cpp", "int SetLevelData(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x11, 0);
        TT_ERROR("SetLevelData.cpp", "int SetLevelData(...)", " gameInfo not exists");
    }

    beh->GetInputParameterValue(0, &gameInfo->levelScore);
    beh->GetInputParameterValue(1, &gameInfo->levelBonus);
    beh->GetInputParameterValue(2, &gameInfo->levelReached);

    beh->ActivateOutput(0);
    return CKBR_OK;
}