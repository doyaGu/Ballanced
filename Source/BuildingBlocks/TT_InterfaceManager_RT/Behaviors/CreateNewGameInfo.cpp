/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Create new GameInfo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorCreateNewGameInfoDecl();
CKERROR CreateCreateNewGameInfoProto(CKBehaviorPrototype **pproto);
int CreateNewGameInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateNewGameInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Create new GameInfo");
    od->SetDescription("Creates new GameInfo");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xa8b6c27, 0x402b43b4));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateNewGameInfoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCreateNewGameInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Create new GameInfo");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Registry Path", CKPGUID_STRING);

    proto->DeclareOutParameter("Address of GameInfo (hex)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CreateNewGameInfo);

    *pproto = proto;
    return CK_OK;
}

int CreateNewGameInfo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKSTRING path = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

    InterfaceManager *man = InterfaceManager::GetManager(context);
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