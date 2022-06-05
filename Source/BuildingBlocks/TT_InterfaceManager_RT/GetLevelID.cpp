/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Level ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetLevelIDDecl();
CKERROR CreateGetLevelIDProto(CKBehaviorPrototype **);
int GetLevelID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetLevelIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Level ID");
    od->SetDescription("Gets level id from manager");
    od->SetCategory("TT InterfaceManager/LevelInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x649011FE, 0x437D5918));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetLevelIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetLevelIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Level ID");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Level ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetLevelID);

    *pproto = proto;
    return CK_OK;
}

int GetLevelID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("GetLevelID.cpp", "int GetLevelID(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x0E, 0);
        TT_ERROR("GetLevelID.cpp", "int GetLevelID(...)", " gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->levelID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}