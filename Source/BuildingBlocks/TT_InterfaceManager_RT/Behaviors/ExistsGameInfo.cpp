/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Exists GameInfo
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorExistsGameInfoDecl();
CKERROR CreateExistsGameInfoProto(CKBehaviorPrototype **);
int ExistsGameInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorExistsGameInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Exists GameInfo ?");
    od->SetDescription("Checks GameInfo");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x56173aa5, 0x73ff1a7f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExistsGameInfoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateExistsGameInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Exists GameInfo ?");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Address of GameInfo (hex)", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ExistsGameInfo);

    *pproto = proto;
    return CK_OK;
}

int ExistsGameInfo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CGameInfo *gameInfo = NULL;
    beh->GetInputParameterValue(0, &gameInfo);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (gameInfo == man->GetGameInfo())
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_OK;
}