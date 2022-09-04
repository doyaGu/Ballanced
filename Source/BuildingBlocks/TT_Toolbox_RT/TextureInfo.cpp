//////////////////////////////////
//////////////////////////////////
//
//        TT_TextureInfo
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorTextureInfoDecl();
CKERROR CreateTextureInfoProto(CKBehaviorPrototype **pproto);
int TextureInfo(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureInfoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_TextureInfo");
    od->SetDescription("Gets Available TextureMemory, Number of used Textures etc.");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7c663b40, 0x2f4b347e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureInfoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTextureInfoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_TextureInfo");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("MaxVideoMemory", CKPGUID_INT);
    proto->DeclareOutParameter("NumberOfTextures", CKPGUID_INT);
    proto->DeclareOutParameter("UsedSystemMemory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("TexturesInVideoMemory", CKPGUID_INT);
    proto->DeclareOutParameter("UsedVideoTextureMemory", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextureInfo);

    *pproto = proto;
    return CK_OK;
}

int TextureInfo(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}