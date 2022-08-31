////////////////////////////////////
////////////////////////////////////
//
//        TT_FlushTextures
//
////////////////////////////////////
////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTFlushTexturesBehaviorProto(CKBehaviorPrototype **pproto);
int TTFlushTextures(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTFlushTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_FlushTextures");
    od->SetDescription("Flushes all Textures from VideoMemory");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x734f73b4,0x1fc79ac));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTFlushTexturesBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTFlushTexturesBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_FlushTextures");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTFlushTextures);

    *pproto = proto;
    return CK_OK;
}

int TTFlushTextures(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTFlushTexturesCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}