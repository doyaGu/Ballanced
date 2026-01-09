/////////////////////////////////
/////////////////////////////////
//
//        TT_CelShading
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>

CKObjectDeclaration *FillBehaviorCelShadingDecl();
CKERROR CreateCelShadingProto(CKBehaviorPrototype **pproto);
int CelShading(const CKBehaviorContext &behcontext);
int CelShadingOutlineCallback(CKRenderContext *rc, CKRenderObject *ro, void *arg);

CKObjectDeclaration *FillBehaviorCelShadingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_CelShading");
    od->SetDescription("Cartoon-Shader");
    od->SetCategory("TT Toolbox/Material");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2d2d2a64, 0x5a0501c7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCelShadingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateCelShadingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_CelShading");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,128");
    proto->DeclareInParameter("Outline", CKPGUID_BOOL, "false");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CelShading);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int CelShadingOutlineCallback(CKRenderContext *rc, CKRenderObject *ro, void *arg)
{
    CK3dEntity *entity = (CK3dEntity *)ro;
    if (!rc || !entity)
        return 1;

    CKDWORD savedCullMode = rc->GetState(VXRENDERSTATE_CULLMODE);
    CKDWORD savedFillMode = rc->GetState(VXRENDERSTATE_FILLMODE);
    CKDWORD savedShadeMode = rc->GetState(VXRENDERSTATE_SHADEMODE);

    // Render outline by disabling culling and using wireframe+flat shading.
    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_FILLMODE, VXFILL_WIREFRAME);
    rc->SetState(VXRENDERSTATE_SHADEMODE, VXSHADE_FLAT);

    CKMesh *mesh = entity->GetCurrentMesh();
    if (mesh)
        mesh->Render(rc, entity);

    rc->SetState(VXRENDERSTATE_CULLMODE, savedCullMode);
    rc->SetState(VXRENDERSTATE_FILLMODE, savedFillMode);
    rc->SetState(VXRENDERSTATE_SHADEMODE, savedShadeMode);

    return 1;
}

int CelShading(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (!beh)
        return CKBR_OK;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CK3dEntity *light = (CK3dEntity *)beh->GetInputParameterObject(0);

    int channelIndex = 0;

    VxColor color(0.0f, 0.0f, 0.0f, 0.0f);
    beh->GetInputParameterValue(1, &color);

    VxVector lightPos;
    lightPos.z = 0.0f;
    light->GetPosition(&lightPos, target);

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Create cel shading material
        CK_OBJECTCREATION_OPTIONS createOpts = (beh->GetObjectFlags() & CK_OBJECT_NOTTOBEDELETED) ? CK_OBJECTCREATION_NONAMECHECK : CK_OBJECTCREATION_DYNAMIC;
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "Mat CelShading", createOpts);
        mat->SetAmbient(VxColor(0.0f, 0.0f, 0.0f, 1.0f));
        mat->SetDiffuse(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        mat->SetEmissive(VxColor(0.0f, 0.0f, 0.0f, 1.0f));
        mat->SetSpecular(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        mat->Show(CKSHOW);
        mat->SetTwoSided(TRUE);

        // Create cel shading texture (1D lookup)
        CK_OBJECTCREATION_OPTIONS texCreateOpts = (beh->GetObjectFlags() & CK_OBJECT_NOTTOBEDELETED) ? CK_OBJECTCREATION_NONAMECHECK : CK_OBJECTCREATION_DYNAMIC;
        CKTexture *tex = (CKTexture *)ctx->CreateObject(CKCID_TEXTURE, "Tex CelShading", texCreateOpts);
        if (tex->Create(16, 1, 32))
        {
            VxColor pixel;
            pixel.a = 0.0f;

            // Dark zone (0-2)
            for (int i = 0; i < 3; ++i)
            {
                pixel.r = color.r * 0.5f;
                pixel.g = color.g * 0.5f;
                pixel.b = color.b * 0.5f;
                tex->SetPixel(i, 0, RGBAFTOCOLOR(&pixel));
            }

            // Mid zone (3-7)
            for (int i = 3; i < 8; ++i)
            {
                pixel.r = (color.r + 0.5f) * 0.5f;
                pixel.g = (color.g + 0.5f) * 0.5f;
                pixel.b = (color.b + 0.5f) * 0.5f;
                tex->SetPixel(i, 0, RGBAFTOCOLOR(&pixel));
            }

            // Bright zone (8-15)
            for (int i = 8; i < 16; ++i)
            {
                pixel.r = (color.r + 1.0f) * 0.5f;
                pixel.g = (color.g + 1.0f) * 0.5f;
                pixel.b = (color.b + 1.0f) * 0.5f;
                tex->SetPixel(i, 0, RGBAFTOCOLOR(&pixel));
            }
        }

        mat->SetTexture0(tex);

        // Add channel to mesh
        int oldChannelCount = mesh->GetChannelCount();
        channelIndex = mesh->AddChannel(mat, TRUE);
        if (oldChannelCount != channelIndex)
            return CKBR_PARAMETERERROR;

        mesh->SetChannelSourceBlend(channelIndex, VXBLEND_SRCCOLOR);
        mesh->SetChannelDestBlend(channelIndex, VXBLEND_ONE);

        // Calculate vertex normals for cel shading
        int vertexCount = mesh->GetVertexCount();
        int faceCount = mesh->GetFaceCount();

        int normalSize = vertexCount * 3 * sizeof(float);
        float *normals = new float[vertexCount * 3];
        if (normals)
        {
            for (int i = 0; i < vertexCount; ++i)
            {
                normals[i * 3 + 2] = 0.0f;
            }
        }

        // Accumulate face normals to vertex normals
        for (int v = 0; v < vertexCount; ++v)
        {
            VxVector vertPos;
            vertPos.z = 0.0f;
            mesh->GetVertexPosition(v, &vertPos);

            normals[v * 3] = 0.0f;
            normals[v * 3 + 1] = 0.0f;
            normals[v * 3 + 2] = 0.0f;

            for (int f = 0; f < faceCount; ++f)
            {
                int v0, v1, v2;
                mesh->GetFaceVertexIndex(f, v0, v1, v2);

                VxVector pos0, pos1, pos2;
                pos0.z = 0.0f;
                pos1.z = 0.0f;
                pos2.z = 0.0f;
                mesh->GetVertexPosition(v0, &pos0);
                mesh->GetVertexPosition(v1, &pos1);
                mesh->GetVertexPosition(v2, &pos2);

                // Check if this face contains our vertex
                if ((vertPos.x == pos0.x && vertPos.y == pos0.y && vertPos.z == pos0.z) ||
                    (vertPos.x == pos1.x && vertPos.y == pos1.y && vertPos.z == pos1.z) ||
                    (vertPos.x == pos2.x && vertPos.y == pos2.y && vertPos.z == pos2.z))
                {
                    const VxVector &faceNormal = mesh->GetFaceNormal(f);
                    normals[v * 3] += faceNormal.x;
                    normals[v * 3 + 1] += faceNormal.y;
                    normals[v * 3 + 2] += faceNormal.z;
                }
            }

            // Normalize
            float len = sqrtf(normals[v * 3] * normals[v * 3] +
                             normals[v * 3 + 1] * normals[v * 3 + 1] +
                             normals[v * 3 + 2] * normals[v * 3 + 2]);
            if (len > 0.0f)
            {
                normals[v * 3] /= len;
                normals[v * 3 + 1] /= len;
                normals[v * 3 + 2] /= len;
            }
        }

        beh->SetLocalParameterValue(0, normals, normalSize);
        delete[] normals;

        // Check if outline is enabled
        CKBOOL outline = FALSE;
        beh->GetInputParameterValue(2, &outline);
        if (outline)
        {
            target->AddPreRenderCallBack(CelShadingOutlineCallback, (void *)beh, FALSE);
        }
    }

    // Update UVs based on light direction
    float *storedNormals = (float *)beh->GetLocalParameterReadDataPtr(0);

    CKDWORD uvStride;
    float *uvPtr = (float *)mesh->GetTextureCoordinatesPtr(&uvStride, channelIndex);

    CKDWORD vertexStride;
    float *vertices = (float *)mesh->GetPositionsPtr(&vertexStride);

    VxVector toLight;
    toLight.z = 0.0f;

    int vertexCount = mesh->GetVertexCount();
    float *normalPtr = storedNormals + 1;

    for (int i = 0; i < vertexCount; ++i)
    {
        toLight.x = vertices[0] - lightPos.x;
        toLight.y = vertices[1] - lightPos.y;
        toLight.z = vertices[2] - lightPos.z;
        toLight.Normalize();

        float dot = toLight.x * normalPtr[-1] + toLight.y * normalPtr[0] + toLight.z * normalPtr[1];
        if (dot < 0.0f)
            dot = 0.0f;

        *(uvPtr + 1) = 0.0f;
        *uvPtr = 1.0f - dot;

        uvPtr = (float *)((CKBYTE *)uvPtr + uvStride);
        vertices = (float *)((CKBYTE *)vertices + vertexStride);
        normalPtr += 3;
    }

    mesh->UVChanged();

    return CKBR_ACTIVATENEXTFRAME;
}