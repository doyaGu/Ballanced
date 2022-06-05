/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level Name
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetLevelNameDecl();
CKERROR CreateGetLevelNameProto(CKBehaviorPrototype **);
int GetLevelName(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelNameDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level Name");
    od->SetDescription("Gets level name from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6A3D4643, 0x4F0462DD));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelNameProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelNameProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level Name");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelName);

    *pproto = proto;
    return CK_OK;
}

int GetLevelName(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("GetLevelName.cpp", "int GetLevelName(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x0D, 0);
        TT_ERROR("GetLevelName.cpp", "int GetLevelName(...)", " gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, gameInfo->levelName);

    beh->ActivateOutput(0);
    return CKBR_OK;
}