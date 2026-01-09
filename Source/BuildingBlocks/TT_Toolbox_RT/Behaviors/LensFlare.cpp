////////////////////////////////
////////////////////////////////
//
//        TT_LensFlare
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include <math.h>

CKObjectDeclaration *FillBehaviorLensFlareDecl();
CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto);
int LensFlare(const CKBehaviorContext &behcontext);
CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext);
int LensFlareRenderCallback(CKRenderContext *dev, void *arg, CKBehavior *beh);

// Lens flare element structure (64 bytes)
struct FlareElement
{
    float distance;             // 0: Position on flare line (0.0 = center, 1.0 = source)
    float intensity;            // 4: Current intensity for fade in/out
    float sizeX;                // 8: X size
    float sizeY;                // 12: Y size  
    CKWORD fadeInFrames;        // 16: Frames to fade in
    CKWORD fadeOutFrames;       // 18: Frames to fade out
    float rotationFactor;       // 20: Rotation factor (cos of angle)
    int state;                  // 24: State (0=off, 1=fading in, 2=fading out, 4=on)
    VxColor color;              // 28: RGBA color
    float maxIntensity;         // 44: Maximum intensity
    float uvLeft;               // 48: Texture UV left
    float uvTop;                // 52: Texture UV top
    float uvRight;              // 56: Texture UV right
    float uvBottom;             // 60: Texture UV bottom
};

// Flare data header
struct FlareData
{
    FlareElement *elements;     // Pointer to element array
    int elementCount;           // Number of elements
};

// Allocate/reallocate flare array
static void ResizeFlareArray(FlareData *data, int newCount)
{
    if (newCount == data->elementCount)
        return;

    if (data->elements)
    {
        delete[] data->elements;
        data->elements = NULL;
    }

    data->elementCount = newCount;
    
    if (newCount > 0)
    {
        data->elements = new FlareElement[newCount];
        if (data->elements)
        {
            for (int i = 0; i < newCount; i++)
            {
                FlareElement *e = &data->elements[i];
                e->distance = 0.0f;
                e->intensity = 0.0f;
                e->sizeX = 1.0f;
                e->sizeY = 1.0f;
                e->fadeInFrames = 100;
                e->fadeOutFrames = 100;
                e->rotationFactor = 0.0f;
                e->state = 0;
                e->color = VxColor(1.0f, 1.0f, 1.0f, 1.0f);
                e->maxIntensity = 0.0f;
                e->uvLeft = 0.0f;
                e->uvTop = 0.0f;
                e->uvRight = 0.0f;
                e->uvBottom = 0.0f;
            }
        }
    }
}

// Start fade in for visible flares
static CKBOOL UpdateFlareVisibility(FlareElement *elements, int count, float dotProduct)
{
    CKBOOL anyActive = FALSE;
    for (int i = 0; i < count; i++)
    {
        FlareElement *e = &elements[i];
        
        if (e->state == 0 || (e->state & 2) != 0)
        {
            // Off or fading out - check if should start fading in
            if (e->maxIntensity < dotProduct)
            {
                e->state = 1; // Start fade in
                anyActive = TRUE;
            }
        }
        else
        {
            // On or fading in - check if should start fading out  
            if (e->maxIntensity > dotProduct)
            {
                e->state = 2; // Start fade out
                anyActive = TRUE;
            }
        }
    }
    return anyActive;
}

// Turn off all flares
static CKBOOL TurnOffFlares(FlareElement *elements, int count)
{
    CKBOOL anyActive = FALSE;
    for (int i = 0; i < count; i++)
    {
        if (elements[i].state != 0)
        {
            elements[i].state = 2; // Start fade out
            anyActive = TRUE;
        }
    }
    return anyActive;
}

// Render callback for lens flare
int LensFlareRenderCallback(CKRenderContext *dev, void *arg, CKBehavior *beh)
{
    // Get blend modes
    VXBLEND_MODE srcBlend = VXBLEND_ONE;
    VXBLEND_MODE destBlend = VXBLEND_ZERO;
    beh->GetInputParameterValue(3, &srcBlend);
    beh->GetInputParameterValue(4, &destBlend);

    // Get flare data
    FlareData **dataPtr = (FlareData **)beh->GetLocalParameterReadDataPtr(0);
    if (!dataPtr || !*dataPtr)
        return 1;

    FlareData *data = *dataPtr;
    int flareCount = data->elementCount;
    FlareElement *elements = data->elements;

    // Get target entity
    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return 1;

    // Get camera
    CKCamera *camera = dev->GetAttachedCamera();
    float nearPlane = camera->GetFrontPlane();
    float depth = nearPlane + 0.01f;

    // Get entity position in screen space
    VxVector entityPos;
    target->GetPosition(&entityPos, NULL);

    VxVector screenPos;
    dev->Transform(&screenPos, &entityPos, NULL);

    // Get flare line direction
    VxVector cameraPos;
    camera->GetPosition(&cameraPos, NULL);

    VxVector direction = entityPos - cameraPos;
    float dirLen = direction.Magnitude();
    if (dirLen > 0.0f)
        direction /= dirLen;

    float cosAngle = direction.z / sqrtf(depth * depth);
    
    // Calculate flare line
    float flareX = screenPos.x;
    float flareY = screenPos.y;
    float flareZ = screenPos.z - depth;

    // Get screen size / center
    int screenWidth = dev->GetWidth();
    int screenHeight = dev->GetHeight();
    float centerX = (float)screenWidth * 0.5f;
    float centerY = (float)screenHeight * 0.5f;

    // Check visibility (screen pick)
#if CKVERSION == 0x13022002
    CKPICKRESULT pickRes;
#else
    VxIntersectionDesc pickRes;
#endif
    CKRenderObject *picked = dev->Pick((int)screenPos.x, (int)screenPos.y, &pickRes, FALSE);
    CKBOOL visible = (picked == NULL) || (picked == (CKRenderObject *)target);

    if (visible)
    {
        // Also check screen bounds
        if (screenPos.x <= 15.0f || screenPos.x >= (float)(screenWidth - 15) || screenPos.y <= 15.0f)
        {
            TurnOffFlares(elements, flareCount);
        }
        else
        {
            UpdateFlareVisibility(elements, flareCount, cosAngle);
        }
    }

    // Get size multiplier
    Vx2DVector sizeMultiplier(1.0f, 1.0f);
    beh->GetInputParameterValue(2, &sizeMultiplier);

    // Get texture
    CKTexture *texture = NULL;
    CKParameterIn *texParam = beh->GetInputParameter(1);
    if (texParam && texParam->GetDirectSource())
    {
        texture = (CKTexture *)beh->GetInputParameterObject(1);
    }

    float invTexWidth = 1.0f;
    float invTexHeight = 1.0f;
    if (texture)
    {
        invTexWidth = 1.0f / (float)texture->GetWidth();
        invTexHeight = 1.0f / (float)texture->GetHeight();
    }

    // Set render states
    dev->SetTexture(texture, FALSE, 0);
    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, srcBlend);
    dev->SetState(VXRENDERSTATE_DESTBLEND, destBlend);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_LIGHTING, FALSE);
    dev->SetState(VXRENDERSTATE_FOGENABLE, FALSE);

    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_DECAL, 0);
    dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEAR, 0);
    dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEAR, 0);

    // Allocate vertex/index buffers
    const int maxVertexCount = flareCount * 4;
    VxDrawPrimitiveData *dpData = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, maxVertexCount);
    if (!dpData)
        return 1;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector *positions = (VxVector *)dpData->PositionPtr;
    CKDWORD *colors = (CKDWORD *)dpData->ColorPtr;
    VxUV *uvs = (VxUV *)dpData->TexCoordPtr;
#else
    VxVector *positions = (VxVector *)dpData->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)dpData->Colors.Ptr;
    VxUV *uvs = (VxUV *)dpData->TexCoord.Ptr;
#endif

    CKWORD *indices = new CKWORD[flareCount * 6];
    int indexCount = 0;
    int vertexIndex = 0;

    // Process each flare element
    for (int i = 0; i < flareCount; i++)
    {
        FlareElement *e = &elements[i];

        // Update fade state
        switch (e->state)
        {
        case 0: // Off
            continue;

        case 1: // Fading in
            e->intensity += e->maxIntensity / (float)e->fadeInFrames;
            if (e->intensity > e->maxIntensity)
            {
                e->intensity = e->maxIntensity;
                e->state = 4; // Full on
            }
            break;

        case 2: // Fading out
            e->intensity -= e->maxIntensity / (float)e->fadeOutFrames;
            if (e->intensity < 0.01f)
            {
                e->intensity = 0.0f;
                e->state = 0; // Off
                continue;
            }
            break;

        case 4: // On
            break;

        default:
            continue;
        }

        // Position on flare line (0 = screen center, 1 = source)
        float elemX = centerX + (flareX - centerX) * e->distance;
        float elemY = centerY + (flareY - centerY) * e->distance;

        // Calculate rotation
        float cosRot = 1.0f;
        float sinRot = 0.0f;
        if (e->rotationFactor != 0.0f)
        {
            float angle = (elemX + elemY) * e->rotationFactor;
            cosRot = cosf(angle);
            sinRot = sinf(angle);
        }

        // Calculate quad corners with rotation
        float halfWidth = sizeMultiplier.x * e->sizeX;
        float halfHeight = sizeMultiplier.y * e->sizeY;
        
        // Resolve UVs (accept either normalized 0..1 or pixel space)
        float u0 = e->uvLeft;
        float v0 = e->uvTop;
        float u1 = e->uvRight;
        float v1 = e->uvBottom;

        if (u1 <= u0 && v1 <= v0)
        {
            u0 = 0.0f;
            v0 = 0.0f;
            u1 = 1.0f;
            v1 = 1.0f;
        }
        else if (texture && (u0 > 1.0f || u1 > 1.0f || v0 > 1.0f || v1 > 1.0f))
        {
            u0 *= invTexWidth;
            u1 *= invTexWidth;
            v0 *= invTexHeight;
            v1 *= invTexHeight;
        }

        // Modulate color by intensity
        VxColor c = e->color;
        c.r *= e->intensity;
        c.g *= e->intensity;
        c.b *= e->intensity;
        c.a *= e->intensity;
        CKDWORD packedColor = RGBAFTOCOLOR(&c);

        // Offsets for a rotated quad
        const float ox[4] = {-halfWidth, halfWidth, halfWidth, -halfWidth};
        const float oy[4] = {-halfHeight, -halfHeight, halfHeight, halfHeight};
        const float tu[4] = {u0, u1, u1, u0};
        const float tv[4] = {v0, v0, v1, v1};

        for (int v = 0; v < 4; ++v)
        {
            float rx = ox[v] * cosRot - oy[v] * sinRot;
            float ry = ox[v] * sinRot + oy[v] * cosRot;

            positions->x = elemX + rx;
            positions->y = elemY + ry;
            positions->z = 0.0f;
            *colors = packedColor;
            uvs->u = tu[v];
            uvs->v = tv[v];

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

        // Build 2 triangles (6 indices)
        indices[indexCount++] = vertexIndex;
        indices[indexCount++] = vertexIndex + 1;
        indices[indexCount++] = vertexIndex + 2;
        indices[indexCount++] = vertexIndex;
        indices[indexCount++] = vertexIndex + 2;
        indices[indexCount++] = vertexIndex + 3;

        vertexIndex += 4;
    }

    // Draw
    if (indexCount > 0)
    {
        dpData->VertexCount = vertexIndex;
        dev->DrawPrimitive(VX_TRIANGLELIST, indices, indexCount, dpData);
    }

    delete[] indices;

    // Restore render states
    dev->SetState(VXRENDERSTATE_FOGENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_LIGHTING, TRUE);

    return 1;
}

CKObjectDeclaration *FillBehaviorLensFlareDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_LensFlare");
    od->SetDescription("Creates camera reflections");
    od->SetCategory("TT Toolbox/Object");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x241b32a5, 0x71fe3357));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLensFlareProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLensFlareProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_LensFlare");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Flares Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Flares Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size Multiplier", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "One");
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR, "Zero");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Active", CKPGUID_BOOL, "FALSE");

    proto->DeclareSetting("Read Array Each Frame", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LensFlare);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(LensFlareCallBack);

    *pproto = proto;
    return CK_OK;
}

int LensFlare(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    CKBOOL active = FALSE;
    beh->GetLocalParameterValue(1, &active);

    // Set up render callback
    target->ModifyMoveableFlags(VX_MOVEABLE_RENDERLAST, 0);
    target->AddPreRenderCallBack((CK_RENDEROBJECT_CALLBACK)LensFlareRenderCallback, beh, TRUE);

    // Handle On input
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        active = TRUE;
    }
    // Handle Off input
    else if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        
        // Remove callback
        target->RemovePreRenderCallBack((CK_RENDEROBJECT_CALLBACK)LensFlareRenderCallback, beh);
        return CKBR_OK;
    }

    if (!active)
        return CKBR_ACTIVATENEXTFRAME;

    beh->SetLocalParameterValue(1, &active);

    // Get data array
    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    FlareData **dataPtr = (FlareData **)beh->GetLocalParameterReadDataPtr(0);
    
    if (array && dataPtr && *dataPtr)
    {
        FlareData *data = *dataPtr;
        int rowCount = array->GetRowCount();

        // Resize if needed
        ResizeFlareArray(data, rowCount);

        // Read array data
        for (int i = 0; i < data->elementCount; i++)
        {
            FlareElement *e = &data->elements[i];

            // Column 0: Texture (check GUID 0x7AA6EB20, 0x6942DC29)
            CKGUID guid = array->GetColumnParameterGuid(0);
            if (guid.IsValid())
            {
                // Read texture slot info (uvLeft, uvTop, uvRight, uvBottom)
                array->GetElementValue(i, 0, &e->uvLeft);
            }

            // Column 1: Distance (float)
            if (array->GetColumnType(1) == CKARRAYTYPE_FLOAT)
            {
                array->GetElementValue(i, 1, &e->distance);
            }

            // Column 2: Color (check GUID 0x57E5FAEE, 0x7CB6CB91)  
            guid = array->GetColumnParameterGuid(2);
            if (guid.IsValid())
            {
                VxColor color;
                array->GetElementValue(i, 2, &color);
                e->color = color;
            }

            // Column 3: Size (2DVector, GUID 0x4EF8B54A, 0x6074EBAF)
            guid = array->GetColumnParameterGuid(3);
            if (guid.IsValid())
            {
                Vx2DVector size;
                array->GetElementValue(i, 3, &size);
                e->sizeX = size.x;
                e->sizeY = size.y;
            }

            // Column 4: MaxIntensity (float)
            if (array->GetColumnType(4) == CKARRAYTYPE_FLOAT)
            {
                array->GetElementValue(i, 4, &e->maxIntensity);
            }

            // Column 5: FadeInFrames (int)
            if (array->GetColumnType(5) == CKARRAYTYPE_INT)
            {
                int fadeIn;
                array->GetElementValue(i, 5, &fadeIn);
                e->fadeInFrames = (CKWORD)fadeIn;
            }

            // Column 6: FadeOutFrames (int)
            if (array->GetColumnType(6) == CKARRAYTYPE_INT)
            {
                int fadeOut;
                array->GetElementValue(i, 6, &fadeOut);
                e->fadeOutFrames = (CKWORD)fadeOut;
            }

            // Column 7: Angle (float, GUID 0x11278CF5, 0x30AF1B3A)
            guid = array->GetColumnParameterGuid(7);
            if (guid.IsValid())
            {
                float angle = 0.0f;
                array->GetElementValue(i, 7, &angle);
                if (angle == 0.0f)
                    angle = 1.57f; // PI/2
                e->rotationFactor = cosf(angle);
            }
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR LensFlareCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        // Allocate flare data
        FlareData *data = new FlareData();
        if (data)
        {
            data->elements = NULL;
            data->elementCount = 0;
        }
        beh->SetLocalParameterValue(0, &data, sizeof(void *));
    }
    break;

    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        // Free flare data
        FlareData **dataPtr = (FlareData **)beh->GetLocalParameterReadDataPtr(0);
        if (dataPtr && *dataPtr)
        {
            FlareData *data = *dataPtr;
            if (data->elements)
            {
                delete[] data->elements;
            }
            delete data;

            void *nullPtr = NULL;
            beh->SetLocalParameterValue(0, &nullPtr, sizeof(void *));
        }

        // Remove render callback
        CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
        if (target)
        {
            target->RemovePreRenderCallBack((CK_RENDEROBJECT_CALLBACK)LensFlareRenderCallback, beh);
        }
    }
    break;
    }

    return CKBR_OK;
}