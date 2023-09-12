///////////////////////////////
///////////////////////////////
//
//        TT_DebugOFF
//
///////////////////////////////
///////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "DebugManager.h"

CKObjectDeclaration *FillBehaviorDebugOFFDecl();
CKERROR CreateDebugOFFProto(CKBehaviorPrototype **pproto);
int DebugOFF(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDebugOFFDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_DebugOFF");
    od->SetDescription("Debug OFF");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x70f65b3e, 0x2dc41372));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDebugOFFProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDebugOFFProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_DebugOFF");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DebugOFF);

    *pproto = proto;
    return CK_OK;
}

int DebugOFF(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    DebugManager *dm = DebugManager::GetManager(context);
    if (dm)
        dm->EnableDebugMode(false);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}