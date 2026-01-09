///////////////////////////////////
///////////////////////////////////
//
//        TT_AnisoMapping
//
///////////////////////////////////
///////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>

CKObjectDeclaration *FillBehaviorAnisoMappingDecl();
CKERROR CreateAnisoMappingProto(CKBehaviorPrototype **pproto);
int AnisoMapping(const CKBehaviorContext &behcontext);
CKERROR AnisoMappingCallBack(const CKBehaviorContext &behcontext);

void BuildMappingMatrix(VxMatrix &mat, const VxVector &eyeDir, const VxVector &lightDir)
{
    mat[0][0] = eyeDir.x;
    mat[0][1] = eyeDir.y;
    mat[0][2] = eyeDir.z;
    mat[0][3] = 1.0f;
    mat[1][0] = lightDir.x;
    mat[1][1] = lightDir.y;
    mat[1][2] = lightDir.z;
    mat[1][3] = 1.0f;
    mat[2][0] = 0.0f;
    mat[2][1] = 0.0f;
    mat[2][2] = 0.0f;
    mat[2][3] = 0.0f;
    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
}

CKObjectDeclaration *FillBehaviorAnisoMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_AnisoMapping");
    od->SetDescription("Creates light dependent bump mapping");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x73b30b5e, 0x4fed4c65));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAnisoMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateAnisoMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_AnisoMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Camera", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Texture Size", CKPGUID_INT, "128");
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,128");
    proto->DeclareInParameter("Metal", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("Mapping", CKPGUID_MAPPING, "1");
    proto->DeclareInParameter("Size", CKPGUID_FLOAT, "1.0");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AnisoMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(AnisoMappingCallBack);

    *pproto = proto;
    return CK_OK;
}

int AnisoMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    CKContext *ctx = behcontext.Context;

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CK3dEntity *light = (CK3dEntity *)beh->GetInputParameterObject(0);
    CK3dEntity *camera = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!light || !camera)
        return CKBR_PARAMETERERROR;

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    CKScene *scene = ctx->GetCurrentScene();

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Read parameters
        int texSize = 2;
        beh->GetInputParameterValue(2, &texSize);
        if (texSize < 2)
            texSize = 2;

        VxColor color(0.0f, 0.0f, 0.0f, 0.0f);
        beh->GetInputParameterValue(3, &color);

        CKBOOL metal = FALSE;
        beh->GetInputParameterValue(4, &metal);

        int mapping = 1;
        beh->GetInputParameterValue(5, &mapping);

        float size = 1.0f;
        beh->GetInputParameterValue(6, &size);

        // Create material
        CK_OBJECTCREATION_OPTIONS createOpts = (beh->GetObjectFlags() & CK_OBJECT_NOTTOBEDELETED) ? CK_OBJECTCREATION_NONAMECHECK : CK_OBJECTCREATION_DYNAMIC;
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "Mat AnisoMap", createOpts);
        if (!mat)
            return CKBR_OK;
        mat->SetAmbient(VxColor(0.0f, 0.0f, 0.0f, 1.0f));
        mat->SetDiffuse(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        mat->SetEmissive(VxColor(0.0f, 0.0f, 0.0f, 1.0f));
        mat->SetSpecular(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        mat->Show(CKSHOW);
        mat->SetTwoSided(TRUE);

        // Create texture
        CK_OBJECTCREATION_OPTIONS texCreateOpts = (beh->GetObjectFlags() & CK_OBJECT_NOTTOBEDELETED) ? CK_OBJECTCREATION_NONAMECHECK : CK_OBJECTCREATION_DYNAMIC;
        CKTexture *tex = (CKTexture *)ctx->CreateObject(CKCID_TEXTURE, "Tex AnisoMap", texCreateOpts);
        if (!tex)
        {
            CKDestroyObject(mat);
            return CKBR_OK;
        }
        if (!tex->Create(texSize, texSize, 32))
        {
            CKDestroyObject(tex);
            CKDestroyObject(mat);
            return CKBR_OK;
        }

        XBYTE *surfacePtr = tex->LockSurfacePtr();
        if (!surfacePtr)
        {
            CKDestroyObject(tex);
            CKDestroyObject(mat);
            return CKBR_OK;
        }

        // Generate anisotropic lookup texture
        for (int y = 0; y < texSize; ++y)
        {
            float v = 1.0f - (2.0f * y) / (texSize - 1);
            float cosTerm = sqrtf(1.0f - v * v);

            for (int x = 0; x < texSize; ++x)
            {
                float u = (2.0f * x) / (texSize - 1) - 1.0f;
                float specBase = sqrtf(1.0f - u * u) * cosTerm - u * v;
                if (specBase < 0.0f)
                    specBase = 0.0f;

                float specular = powf(specBase, 40.0f) * 0.8f;

                VxColor pixel;
                float colorComp;

                // Red
                colorComp = metal ? color.r : 1.0f;
                colorComp = (colorComp * specular + color.r * cosTerm * 0.2f) * color.a;
                if (colorComp < 0.0f) pixel.r = 0.0f;
                else if (colorComp > 1.0f) pixel.r = 1.0f;
                else pixel.r = colorComp;

                // Green
                colorComp = metal ? color.g : 1.0f;
                colorComp = (colorComp * specular + color.g * cosTerm * 0.2f) * color.a;
                if (colorComp < 0.0f) pixel.g = 0.0f;
                else if (colorComp > 1.0f) pixel.g = 1.0f;
                else pixel.g = colorComp;

                // Blue
                colorComp = metal ? color.b : 1.0f;
                colorComp = (colorComp * specular + color.b * cosTerm * 0.2f) * color.a;
                if (colorComp < 0.0f) pixel.b = 0.0f;
                else if (colorComp > 1.0f) pixel.b = 1.0f;
                else pixel.b = colorComp;

                pixel.a = 1.0f;
                tex->SetPixel(y, x, RGBAFTOCOLOR(&pixel));
            }
        }

        tex->ReleaseSurfacePtr();

        mat->SetTexture0(tex);

        // Add new channel to mesh
        int channelCount = mesh->GetChannelCount();
        int newChannel = mesh->AddChannel(mat, TRUE);
        if (newChannel < 0 || channelCount != newChannel)
        {
            if (newChannel >= 0)
                mesh->RemoveChannel(newChannel);
            CKDestroyObject(tex);
            CKDestroyObject(mat);
            return CKBR_PARAMETERERROR;
        }

        mesh->SetChannelSourceBlend(newChannel, VXBLEND_SRCCOLOR);
        mesh->SetChannelDestBlend(newChannel, VXBLEND_SRCCOLOR);

        if (scene)
        {
            scene->AddObjectToScene(tex, TRUE);
            scene->AddObjectToScene(mat, TRUE);
        }

        // Store UV data
        CKDWORD vertexStride;
        VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vertexStride);
        int vertexCount = mesh->GetVertexCount();

        // Calculate reference direction
        const VxMatrix &wm = target->GetWorldMatrix();
        VxVector refDir(wm[1][0], wm[1][1], wm[1][2]);
        float refLength = refDir.Magnitude();
        if (refLength <= 0.0f)
            refLength = 1.0f;

        // Allocate UV buffer
        int uvSize = vertexCount * sizeof(float) * 2;
        float *uvData = new float[vertexCount * 2];
        if (uvData)
        {
            for (int i = 0; i < vertexCount * 2; i += 2)
            {
                uvData[i] = 0.0f;
                uvData[i + 1] = 0.0f;
            }
        }

        // Generate UVs based on normals
        CKDWORD normalStride;
        VxVector *normalPtr = (VxVector *)mesh->GetNormalsPtr(&normalStride);

        for (int i = 0; i < vertexCount; ++i)
        {
            VxVector normal = *normalPtr;
            if (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f)
            {
                normal.Normalize();
            }

            float normalMag = sqrtf(normalPtr->x * normalPtr->x + normalPtr->y * normalPtr->y + normalPtr->z * normalPtr->z);

            if (mapping == 1)
            {
                // Spherical mapping
                if (normalMag >= 0.01f)
                    uvData[i * 2] = cosf(fabsf(normal.x)) * size;
                else
                    uvData[i * 2] = 0.0f;

                if (normalMag >= 0.01f)
                    uvData[i * 2 + 1] = sinf(fabsf(normal.x)) * size;
                else
                    uvData[i * 2 + 1] = 0.0f;
            }
            else
            {
                // Linear mapping
                float t = normalMag / refLength;
                uvData[i * 2] = t * size;
                uvData[i * 2 + 1] = t * size;
            }

            normalPtr = (VxVector *)((CKBYTE *)normalPtr + normalStride);
        }

        beh->SetLocalParameterValue(0, uvData, uvSize);
        delete[] uvData;
    }

    // Update UVs based on light and camera positions
    VxVector lightPos, cameraPos;
    lightPos.z = 0.0f;
    light->GetPosition(&lightPos, target);
    cameraPos.z = 0.0f;
    camera->GetPosition(&cameraPos, target);

    int channelCount = mesh->GetChannelCount();
    if (channelCount <= 0)
        return CKBR_OK;
    CKDWORD uvStride;
    float *channelUVs = (float *)mesh->GetTextureCoordinatesPtr(&uvStride, channelCount - 1);

    float *localUVs = (float *)beh->GetLocalParameterReadDataPtr(0);
    if (!channelUVs || !localUVs)
        return CKBR_OK;

    CKDWORD vertexStride;
    VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vertexStride);

    CKDWORD normalStride;
    VxVector *normalPtr = (VxVector *)mesh->GetNormalsPtr(&normalStride);

    int vertexCount = mesh->GetVertexCount();

    for (int i = 0; i < vertexCount; ++i)
    {
        VxVector toLight = lightPos - *vertices;
        toLight.Normalize();

        VxVector toCamera = cameraPos - *vertices;
        toCamera.Normalize();

        VxMatrix mat;
        BuildMappingMatrix(mat, toCamera, toLight);

        VxVector inUV(localUVs[i * 2], localUVs[i * 2 + 1], 0.0f);
        VxVector outUV;
        Vx3DMultiplyMatrixVector(&outUV, mat, &inUV);

        *channelUVs = outUV.x;
        *(channelUVs + 1) = outUV.y;

        channelUVs = (float *)((CKBYTE *)channelUVs + uvStride);
        vertices = (VxVector *)((CKBYTE *)vertices + vertexStride);
        normalPtr = (VxVector *)((CKBYTE *)normalPtr + normalStride);
    }

    mesh->UVChanged();

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR AnisoMappingCallBack(const CKBehaviorContext &behcontext)
{
    if (behcontext.CallbackMessage == CKM_BEHAVIORDEACTIVATESCRIPT)
    {
        CKBehavior *beh = behcontext.Behavior;
        CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
        if (!target)
            return CKBR_OWNERERROR;

        CKMesh *mesh = target->GetCurrentMesh();
        if (mesh)
        {
            int channelCount = mesh->GetChannelCount() - 1;
            if (mesh->GetChannelMaterial(channelCount))
            {
                CKMaterial *mat = mesh->GetChannelMaterial(channelCount);
                CKTexture *tex = mat->GetTexture();
                CKDestroyObject(tex);
                CKDestroyObject(mat);
                mesh->RemoveChannel(channelCount);
            }
        }
    }

    return CKBR_OK;
}
