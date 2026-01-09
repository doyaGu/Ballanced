//////////////////////////////////
//////////////////////////////////
//
//        TT ShadowPlane
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorShadowPlaneDecl();
CKERROR CreateShadowPlaneProto(CKBehaviorPrototype **pproto);
int ShadowPlane(const CKBehaviorContext &behcontext);
int ShadowPlaneRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *argument);

// Shadow plane vertex data (48 bytes = 4 vertices * 3 floats each)
struct ShadowPlaneData
{
    VxVector corners[4];  // 4 corner positions
};

CKObjectDeclaration *FillBehaviorShadowPlaneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ShadowPlane");
    od->SetDescription("Creates shadow planes without Z-Buffer writing");
    od->SetCategory("TT Toolbox/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x137c3d66, 0x3ff1752d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShadowPlaneProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateShadowPlaneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ShadowPlane");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("SourceBlend", CKPGUID_BLENDFACTOR, "Zero");
    proto->DeclareInParameter("DestBlend", CKPGUID_BLENDFACTOR, "Source Color");

    proto->DeclareLocalParameter("ShadowStruct", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShadowPlane);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ShadowPlaneRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *argument)
{
    CKBehavior *beh = (CKBehavior *)argument;
    CKContext *ctx = beh->GetCKContext();

    // Get texture
    CKTexture *texture = NULL;
    CKParameterIn *texParam = beh->GetInputParameter(1);
    if (texParam && texParam->GetDirectSource())
        texture = (CKTexture *)beh->GetInputParameterObject(1);

    ShadowPlaneData **dataPtr = (ShadowPlaneData **)beh->GetLocalParameterReadDataPtr(0);
    if (!dataPtr || !*dataPtr)
    {
        ctx->OutputToConsole("Keine Geometrie Daten!", FALSE);
        return CKBR_GENERICERROR;
    }

    ShadowPlaneData *data = *dataPtr;

    // Get blend modes
    VXBLEND_MODE srcBlend = VXBLEND_ZERO;
    VXBLEND_MODE destBlend = VXBLEND_SRCCOLOR;
    beh->GetInputParameterValue(3, &srcBlend);
    beh->GetInputParameterValue(4, &destBlend);

    CK3dEntity *target = (CK3dEntity *)obj;

    // Save current world matrix
    VxMatrix savedMatrix;
    savedMatrix = dev->GetWorldTransformationMatrix();

    // Set target's world matrix
    VxMatrix worldMatrix;
    worldMatrix = target->GetWorldMatrix();
    dev->SetWorldTransformationMatrix(worldMatrix);

    // Set texture
    dev->SetTexture(texture, FALSE, 0);

    // Set render states
    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, srcBlend);
    dev->SetState(VXRENDERSTATE_DESTBLEND, destBlend);
    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_LIGHTING, FALSE);
    dev->SetState(VXRENDERSTATE_FOGENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_WRAP0, TRUE);

    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_DECAL, 0);
    dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEAR, 0);
    dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEAR, 0);

    CKWORD indices[4] = {0, 1, 2, 3};

    // Create and fill vertex buffer for quad (positions + colors + texcoords)
    VxDrawPrimitiveData *dpData = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4);
    if (dpData)
    {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxVector *positions = (VxVector *)dpData->PositionPtr;
        CKDWORD *colors = (CKDWORD *)dpData->ColorPtr;
        VxUV *uvs = (VxUV *)dpData->TexCoordPtr;
#else
        VxVector *positions = (VxVector *)dpData->Positions.Ptr;
        CKDWORD *colors = (CKDWORD *)dpData->Colors.Ptr;
        VxUV *uvs = (VxUV *)dpData->TexCoord.Ptr;
#endif

        VxColor white(1.0f, 1.0f, 1.0f, 1.0f);
        CKDWORD color = RGBAFTOCOLOR(&white);

        const VxUV quadUVs[4] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };

        for (int i = 0; i < 4; ++i)
        {
            *positions = data->corners[i];
            *colors = color;
            uvs->u = quadUVs[i].u;
            uvs->v = quadUVs[i].v;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            positions = (VxVector *)((CKBYTE *)positions + dpData->PositionStride);
            colors = (CKDWORD *)((CKBYTE *)colors + dpData->ColorStride);
            uvs = (VxUV *)((CKBYTE *)uvs + dpData->TexCoordStride);
#else
            positions = (VxVector *)((CKBYTE *)positions + dpData->Positions.Stride);
            colors = (CKDWORD *)((CKBYTE *)colors + dpData->Colors.Stride);
            uvs = (VxUV *)((CKBYTE *)uvs + dpData->TexCoord.Stride);
#endif
        }

        dev->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, dpData);
    }

    // Restore render states
    dev->SetState(VXRENDERSTATE_FOGENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_LIGHTING, TRUE);
    dev->SetWorldTransformationMatrix(savedMatrix);

    return 1;
}

int ShadowPlane(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    // Handle Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Handle On input - initialize
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Allocate shadow data
        ShadowPlaneData *data = new ShadowPlaneData();
        if (data)
        {
            memset(data, 0, sizeof(ShadowPlaneData));
        }

        beh->SetLocalParameterValue(0, &data, sizeof(void *));
    }

    // Add pre-render callback
    target->AddPreRenderCallBack((CK_RENDEROBJECT_CALLBACK)ShadowPlaneRenderCallback, beh, TRUE);

    // Get light
    CKLight *light = (CKLight *)beh->GetInputParameterObject(0);
    if (!light)
        return CKBR_ACTIVATENEXTFRAME;

    // Get size
    Vx2DVector size(0.0f, 0.0f);
    beh->GetInputParameterValue(2, &size);

    // Get light position relative to target
    VxVector lightPos;
    light->GetPosition(&lightPos, target);

    // Get target's bounding box
    const VxBbox &bbox = target->GetBoundingBox(TRUE);
    VxBbox scaledBbox;
    scaledBbox.Min.x = bbox.Min.x * size.x;
    scaledBbox.Max.x = bbox.Max.x * size.x;
    scaledBbox.Min.z = bbox.Min.z * size.y;
    scaledBbox.Max.z = bbox.Max.z * size.y;

    // Get shadow data
    ShadowPlaneData **dataPtr = (ShadowPlaneData **)beh->GetLocalParameterReadDataPtr(0);
    if (!dataPtr || !*dataPtr)
        return CKBR_ACTIVATENEXTFRAME;

    ShadowPlaneData *data = *dataPtr;

    // Initialize corner positions (quad corners)
    VxVector corners[4] = {
        VxVector(scaledBbox.Max.x, 0.0f, scaledBbox.Min.z),
        VxVector(scaledBbox.Min.x, 0.0f, scaledBbox.Min.z),
        VxVector(scaledBbox.Min.x, 0.0f, scaledBbox.Max.z),
        VxVector(scaledBbox.Max.x, 0.0f, scaledBbox.Max.z)
    };

    // Get floor manager
    CKFloorManager *floorMgr = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);
    if (!floorMgr)
        return CKBR_ACTIVATENEXTFRAME;

    int floorAttribute = floorMgr->GetFloorAttribute();
    CKAttributeManager *attrMgr = ctx->GetAttributeManager();
    const XObjectPointerArray &floorObjects = attrMgr->GetGlobalAttributeListPtr(floorAttribute);

    // Find floor below target and calculate shadow projection
    CKBOOL foundFloor = FALSE;
    float floorY = 0.0f;

    for (CKObject **it = floorObjects.Begin(); it != floorObjects.End(); ++it)
    {
        CK3dEntity *floorObj = (CK3dEntity *)*it;
        if (!floorObj->IsVisible() || floorObj->IsToBeRendered())
            continue;

        // Get floor position
        VxVector floorPos;
        floorObj->GetPosition(&floorPos, target);

        VxIntersectionDesc isect;
        VxVector origin(0.0f, lightPos.y, 0.0f);
        VxVector end(origin.x, origin.y - 100000.0f, origin.z);

        // Intersection test along a vertical segment
        if (floorObj->RayIntersection(&origin, &end, &isect, target))
        {
            foundFloor = TRUE;
            floorY = isect.IntersectionPoint.y;

            // Calculate shadow projection for each corner
            if (floorY > 0.0f)
            {
                for (int i = 0; i < 4; i++)
                {
                    // Direction from light to corner
                    VxVector toCorner;
                    toCorner.x = corners[i].x - lightPos.x;
                    toCorner.y = corners[i].y - lightPos.y;
                    toCorner.z = corners[i].z - lightPos.z;

                    // Calculate distance to project
                    float dist = sqrtf(toCorner.x * toCorner.x + toCorner.y * toCorner.y + toCorner.z * toCorner.z);
                    float lightDist = sqrtf(lightPos.x * lightPos.x + lightPos.y * lightPos.y + lightPos.z * lightPos.z);
                    float scale = dist * floorY / lightDist;

                    // Normalize and scale
                    float invLen = 1.0f / dist;
                    toCorner.x *= invLen * scale;
                    toCorner.y *= invLen * scale;
                    toCorner.z *= invLen * scale;

                    // Project to floor plane
                    data->corners[i].x = lightPos.x + toCorner.x;
                    data->corners[i].y = lightPos.y + toCorner.y;
                    data->corners[i].z = lightPos.z + toCorner.z;
                }
            }
            break;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}