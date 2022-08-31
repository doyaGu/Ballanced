//////////////////////////////////////
//////////////////////////////////////
//
//        TT_PreloadTextures
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTPreloadTexturesBehaviorProto(CKBehaviorPrototype **pproto);
int TTPreloadTextures(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTPreloadTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_PreloadTextures");
    od->SetDescription("Copies all textures of the current scene to the video memory of the graphics card");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x416b4f0e,0x192d339a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTPreloadTexturesBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTPreloadTexturesBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_PreloadTextures");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("NumberOfTextures", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTPreloadTextures);

    *pproto = proto;
    return CK_OK;
}

int TTPreloadTextures(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTPreloadTexturesCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}