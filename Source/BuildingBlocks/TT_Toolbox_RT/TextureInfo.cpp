//////////////////////////////////
//////////////////////////////////
//
//        TT_TextureInfo
//
//////////////////////////////////
//////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTTextureInfoBehaviorProto(CKBehaviorPrototype **pproto);
int TTTextureInfo(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTTextureInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_TextureInfo");
    od->SetDescription("Gets Available TextureMemory, Number of used Textures etc.");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7c663b40,0x2f4b347e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTTextureInfoBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTTextureInfoBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_TextureInfo");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("MaxVideoMemory", CKPGUID_INT);
    proto->DeclareOutParameter("NumberOfTextures", CKPGUID_INT);
    proto->DeclareOutParameter("UsedSystemMemory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("TexturesInVideoMemory", CKPGUID_INT);
    proto->DeclareOutParameter("UsedVideoTextureMemory", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTTextureInfo);

    *pproto = proto;
    return CK_OK;
}

int TTTextureInfo(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTTextureInfoCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}