////////////////////////////////////
////////////////////////////////////
//
//        TT_FlushTextures
//
////////////////////////////////////
////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorFlushTexturesDecl();
CKERROR CreateFlushTexturesProto(CKBehaviorPrototype **pproto);
int FlushTextures(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFlushTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_FlushTextures");
    od->SetDescription("Flushes all Textures from VideoMemory");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x734f73b4, 0x1fc79ac));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFlushTexturesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateFlushTexturesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_FlushTextures");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FlushTextures);

    *pproto = proto;
    return CK_OK;
}

int FlushTextures(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    CKRenderManager *rm = behcontext.Context->GetRenderManager();
    rm->FlushTextures();
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}