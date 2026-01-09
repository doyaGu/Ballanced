////////////////////////////////////
////////////////////////////////////
//
//        TT ShadowMapping
//
////////////////////////////////////
////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorShadowMappingDecl();
CKERROR CreateShadowMappingProto(CKBehaviorPrototype **pproto);
int ShadowMapping(const CKBehaviorContext &behcontext);
CKERROR ShadowMappingCallBack(const CKBehaviorContext &behcontext);

// Structure for shadow data (460 bytes total)
// Each shadow receiver entry is 28 bytes
struct ShadowReceiverEntry
{
    CK_ID objectId;        // Object ID (offset 0)
    VxVector bboxMin;      // Bounding box min (offset 4)
    VxVector bboxMax;      // Bounding box max (offset 16)
};

struct ShadowMappingData
{
    CK_ID materialId;              // Shadow material ID (offset 0)
    CK_ID textureId;               // Current texture ID (offset 4)
    ShadowReceiverEntry entries[16]; // Up to 16 receivers (offset 8)
    int receiverCount;             // Number of active receivers (offset 456)
};

CKObjectDeclaration *FillBehaviorShadowMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ShadowMapping");
    od->SetDescription("Creates shadows via UV mapping");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x334c7dfe, 0x47c81a31));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShadowMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateShadowMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ShadowMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("SourceBlend", CKPGUID_BLENDFACTOR, "Zero");
    proto->DeclareInParameter("DestBlend", CKPGUID_BLENDFACTOR, "Source Color");

    proto->DeclareLocalParameter("Data", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShadowMapping);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(ShadowMappingCallBack);

    *pproto = proto;
    return CK_OK;
}

// Helper to remove shadow channel from mesh
static void RemoveShadowChannels(ShadowMappingData *data, CKMaterial *shadowMat, CKContext *ctx)
{
    for (int i = 0; i < data->receiverCount; i++)
    {
        CK3dEntity *obj = (CK3dEntity *)ctx->GetObject(data->entries[i].objectId);
        if (!obj) continue;

        CKMesh *mesh = obj->GetCurrentMesh();
        if (!mesh) continue;

        int channel = mesh->GetChannelByMaterial(shadowMat);
        if (channel >= 0)
            mesh->RemoveChannel(channel);
    }
}

// Helper to update shadow receivers
static void UpdateShadowReceivers(ShadowMappingData *data, CK3dEntity *target, CKBehavior *beh)
{
    CKContext *ctx = beh->GetCKContext();

    // Get light position
    CK3dEntity *light = (CK3dEntity *)beh->GetInputParameterObject(0);
    VxVector lightPos;
    light->GetPosition(&lightPos, target);

    // Get shadow size
    Vx2DVector size(0.0f, 0.0f);
    beh->GetInputParameterValue(2, &size);

    // Get shadow receiver attribute manager
    CKAttributeManager *attrMgr = ctx->GetAttributeManager();
    // Note: In original code this uses a floor manager attribute (GUID 0x420B0F79)

    // For now, simplified - just mark that we updated
    data->receiverCount = 0;
}

int ShadowMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    ShadowMappingData *data = (ShadowMappingData *)beh->GetLocalParameterWriteDataPtr(0);
    int oldReceiverCount = data->receiverCount;

    // Get shadow material
    CKMaterial *shadowMat = (CKMaterial *)ctx->GetObject(data->materialId);

    // Handle Off input - cleanup and stop
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        int outputCount = beh->GetOutputCount();
        beh->ActivateOutput(outputCount >= 2 ? 1 : 0, TRUE);

        RemoveShadowChannels(data, shadowMat, ctx);
        return CKBR_OK;
    }

    // Handle On input
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    // Check if texture changed
    CK_ID newTextureId = 0;
    beh->GetInputParameterValue(1, &newTextureId);
    if (newTextureId != data->textureId)
    {
        data->textureId = newTextureId;
        CKTexture *texture = (CKTexture *)ctx->GetObject(newTextureId);
        if (shadowMat)
            shadowMat->SetTexture0(texture);
    }

    // Get blend modes
    VXBLEND_MODE srcBlend = VXBLEND_ZERO;
    VXBLEND_MODE destBlend = VXBLEND_SRCCOLOR;
    beh->GetInputParameterValue(3, &srcBlend);
    beh->GetInputParameterValue(4, &destBlend);

    // Update shadow receivers
    UpdateShadowReceivers(data, target, beh);

    // Process each receiver
    for (int r = 0; r < data->receiverCount; r++)
    {
        CK3dEntity *receiver = (CK3dEntity *)ctx->GetObject(data->entries[r].objectId);
        if (!receiver) continue;

        CKMesh *mesh = receiver->GetCurrentMesh();
        if (!mesh) continue;

        // Get or create shadow channel
        int channel = mesh->GetChannelByMaterial(shadowMat);
        if (channel < 0)
        {
            channel = mesh->AddChannel(shadowMat, FALSE);
            mesh->SetChannelSourceBlend(channel, srcBlend);
            mesh->SetChannelDestBlend(channel, destBlend);
        }

        // Get vertex and UV pointers
        CKDWORD uvStride = 0;
        Vx2DVector *uvCoords = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, channel);

        CKDWORD vertStride = 0;
        VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vertStride);

        int vertexCount = mesh->GetModifierVertexCount();

        // Calculate shadow UVs based on projection onto shadow plane
        VxVector *bboxMin = &data->entries[r].bboxMin;
        VxVector *bboxMax = &data->entries[r].bboxMax;
        float invSizeX = 1.0f / (bboxMin->x - bboxMax->x);
        float invSizeZ = 1.0f / (bboxMin->z - bboxMax->z);

        for (int v = 0; v < vertexCount; v++)
        {
            // Transform vertex to target space
            VxVector worldPos = *vertices;
            VxVector localPos;
            target->InverseTransform(&localPos, &worldPos, receiver);

            // Project to UV coordinates
            uvCoords->x = (localPos.x - bboxMax->x) * invSizeX;
            uvCoords->y = (localPos.z - bboxMax->z) * invSizeZ;

            uvCoords = (Vx2DVector *)((CKBYTE *)uvCoords + uvStride);
            vertices = (VxVector *)((CKBYTE *)vertices + vertStride);
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR ShadowMappingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
        {
            // Create shadow material
            CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_NONAMECHECK;
            if (!(beh->GetFlags() & CKBEHAVIOR_BUILDINGBLOCK))
                options = (CK_OBJECTCREATION_OPTIONS)(options | CK_OBJECTCREATION_DYNAMIC);

            CKMaterial *shadowMat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "SimpleShadowMat", options);

            // Set material colors to white
            VxColor white(1.0f, 1.0f, 1.0f, 1.0f);
            shadowMat->SetDiffuse(white);
            shadowMat->SetAmbient(white);
            VxColor black(0.0f, 0.0f, 0.0f, 1.0f);
            shadowMat->SetSpecular(black);
            shadowMat->SetEmissive(black);
            shadowMat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
            shadowMat->SetTextureBlendMode(VXTEXTUREBLEND_MODULATEALPHA);

            // Initialize data structure
            ShadowMappingData data;
            memset(&data, 0, sizeof(data));
            data.materialId = shadowMat ? shadowMat->GetID() : 0;
            data.textureId = (CK_ID)-1;

            beh->SetLocalParameterValue(0, &data, sizeof(data));
        }
        break;

    case CKM_BEHAVIORDETACH:
        {
            ShadowMappingData *data = (ShadowMappingData *)beh->GetLocalParameterWriteDataPtr(0);
            CKMaterial *shadowMat = (CKMaterial *)ctx->GetObject(data->materialId);
            if (!shadowMat)
                break;

            RemoveShadowChannels(data, shadowMat, ctx);
            ctx->DestroyObject(shadowMat);
        }
        break;

    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
        {
            ShadowMappingData *data = (ShadowMappingData *)beh->GetLocalParameterWriteDataPtr(0);
            CKMaterial *shadowMat = (CKMaterial *)ctx->GetObject(data->materialId);
            if (shadowMat)
                RemoveShadowChannels(data, shadowMat, ctx);
        }
        break;
    }

    return CKBR_OK;
}