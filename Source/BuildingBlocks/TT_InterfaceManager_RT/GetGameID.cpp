/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Get Game ID
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetGameIDDecl();
CKERROR CreateGetGameIDProto(CKBehaviorPrototype **);
int GetGameID(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGameIDDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Game ID");
    od->SetDescription("Gets game ID to manager");
    od->SetCategory("TT InterfaceManager/GameInfo Behaviors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x63527B75, 0x1F9A40D9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGameIDProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGameIDProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Game ID");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Game ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGameID);

    *pproto = proto;
    return CK_OK;
}

int GetGameID(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        TT_ERROR("GetGameID.cpp", "int GetGameID(...)", " im == NULL");
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriverIndex())->GetWindowHandle(), TT_MSG_NO_GAMEINFO, 0x02, 0);
        TT_ERROR("GetGameID.cpp", "int GetGameID(...)", " gameInfo not exists");
    }

    beh->SetOutputParameterValue(0, &gameInfo->gameID);

    beh->ActivateOutput(0);
    return CKBR_OK;
}