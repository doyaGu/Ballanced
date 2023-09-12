/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Delete GameInfo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorDeleteGameInfoDecl();
CKERROR CreateDeleteGameInfoProto(CKBehaviorPrototype **pproto);
int DeleteGameInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeleteGameInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Delete GameInfo ");
    od->SetDescription("Deletes GameInfo");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x29f17890, 0x2e0117a2));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeleteGameInfoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDeleteGameInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Delete GameInfo ");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Deleted");
    proto->DeclareOutput("Not Found");

    proto->DeclareInParameter("Address of GameInfo (hex)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeleteGameInfo);

    *pproto = proto;
    return CK_OK;
}

int DeleteGameInfo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CGameInfo *gameInfo = NULL;
    beh->GetInputParameterValue(0, &gameInfo);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (gameInfo == man->GetGameInfo())
    {
        if (gameInfo)
            delete gameInfo;
        man->SetGameInfo(NULL);
        beh->ActivateOutput(0);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}