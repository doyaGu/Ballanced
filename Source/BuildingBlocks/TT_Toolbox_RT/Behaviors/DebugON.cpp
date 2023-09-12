//////////////////////////////
//////////////////////////////
//
//        TT_DebugON
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "DebugManager.h"

CKObjectDeclaration *FillBehaviorDebugONDecl();
CKERROR CreateDebugONProto(CKBehaviorPrototype **pproto);
int DebugON(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDebugONDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_DebugON");
    od->SetDescription("Debug ON");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d00718f, 0x2c8b33a2));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDebugONProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDebugONProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_DebugON");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DebugON);

    *pproto = proto;
    return CK_OK;
}

int DebugON(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    DebugManager *dm = DebugManager::GetManager(context);
    if (dm)
        dm->EnableDebugMode();

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}