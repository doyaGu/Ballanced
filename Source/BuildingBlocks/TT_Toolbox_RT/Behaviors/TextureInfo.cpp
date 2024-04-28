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
    CKContext *context = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    CKRenderManager *rm = context->GetRenderManager();
    VxDriverDesc *drDesc = rm->GetRenderDriverDescription(0);
    int maxVideoMemory = drDesc->Caps2D.MaxVideoMemory >> 20;
    beh->SetOutputParameterValue(0, &maxVideoMemory);

    VxImageDescEx texDesc;

    int texturesInVideoMemory = 0;
    int numberOfTextures = 0;
    int videoTexSize = 0;
    int sysTexSize = 0;

    int textureCount = context->GetObjectsCountByClassID(CKCID_TEXTURE);
    CK_ID *textureIds = context->GetObjectsListByClassID(CKCID_TEXTURE);
    for (int i = 0; i < textureCount; ++i)
    {
        CKTexture *texture = (CKTexture *)context->GetObject(textureIds[i]);
        if (texture->IsInVideoMemory() && texture->GetVideoTextureDesc(texDesc))
        {
            ++texturesInVideoMemory;
            if (texDesc.Flags != 0)
                context->OutputToConsole("Compressed Texture Found!!!");
            else
                videoTexSize += texDesc.BytesPerLine * texDesc.Height;
        }
        texture->GetSystemTextureDesc(texDesc);
        ++numberOfTextures;
        if (texDesc.Flags != 0)
            context->OutputToConsole("Compressed Texture Found!!!");
        else
            sysTexSize += texDesc.BytesPerLine * texDesc.Height;
    }

    float usedSystemMemory = (float)((double)sysTexSize / 1024);
    float usedVideoTextureMemory = (float)((double)videoTexSize / 1024);

    beh->SetOutputParameterValue(1, &numberOfTextures);
    beh->SetOutputParameterValue(2, &usedSystemMemory);
    beh->SetOutputParameterValue(3, &texturesInVideoMemory);
    beh->SetOutputParameterValue(4, &usedVideoTextureMemory);

    beh->ActivateOutput(0);
    return CKBR_OK;
}