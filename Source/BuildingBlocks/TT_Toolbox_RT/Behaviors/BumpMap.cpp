//////////////////////////////
//////////////////////////////
//
//        TT_BumpMap
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>

CKObjectDeclaration *FillBehaviorBumpMapDecl();
CKERROR CreateBumpMapProto(CKBehaviorPrototype **pproto);
int BumpMap(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBumpMapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_BumpMap");
    od->SetDescription("Creates light dependent bump mapping");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x630d05f7, 0x1dda541f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBumpMapProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateBumpMapProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_BumpMap");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Light", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Emboss-Factor", CKPGUID_FLOAT, "0.02");

    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BumpMap);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int BumpMap(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    int matChannel = -1;
    beh->GetInputParameterValue(0, &matChannel);

    CK3dEntity *light = (CK3dEntity *)beh->GetInputParameterObject(1);

    float embossFactor = 0.01f;
    beh->GetInputParameterValue(2, &embossFactor);

    VxVector lightPos;
    lightPos.z = 0.0f;
    light->GetPosition(&lightPos, target);

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Store original UVs
        CKDWORD uvStride;
        float *uvPtr = (float *)mesh->GetTextureCoordinatesPtr(&uvStride, matChannel);
        int vertexCount = mesh->GetVertexCount();

        int uvSize = vertexCount * 2 * sizeof(float);
        float *uvData = new float[vertexCount * 2];
        if (uvData)
        {
            for (int i = 0; i < vertexCount; ++i)
            {
                uvData[i * 2] = 0.0f;
                uvData[i * 2 + 1] = 0.0f;
            }
        }

        // Copy original UVs
        float *dst = uvData;
        float *src = uvPtr;
        for (int i = 0; i < vertexCount; ++i)
        {
            dst[0] = src[0];
            dst[1] = src[1];
            dst += 2;
            src = (float *)((CKBYTE *)src + uvStride);
        }

        beh->SetLocalParameterValue(0, uvData, uvSize);
        delete[] uvData;
    }

    // Get stored UVs and update based on light direction
    float *storedUVs = (float *)beh->GetLocalParameterReadDataPtr(0);

    int faceCount = mesh->GetFaceCount();

    for (int face = 0; face < faceCount; ++face)
    {
        int v0, v1, v2;
        mesh->GetFaceVertexIndex(face, v0, v2, v1);

        VxVector pos0, pos1, pos2;
        mesh->GetVertexPosition(v0, &pos0);
        mesh->GetVertexPosition(v2, &pos1);
        mesh->GetVertexPosition(v1, &pos2);

        // Calculate light direction to first vertex
        VxVector lightDir0;
        lightDir0.x = lightPos.x - pos0.x;
        lightDir0.y = lightPos.y - pos0.y;
        lightDir0.z = lightPos.z - pos0.z;
        float invLen = 1.0f / sqrtf(lightDir0.x * lightDir0.x + lightDir0.y * lightDir0.y + lightDir0.z * lightDir0.z);
        lightDir0.x *= invLen;
        lightDir0.y *= invLen;
        lightDir0.z *= invLen;

        // Edge from v0 to v1
        VxVector edge01;
        edge01.x = pos1.x - pos0.x;
        edge01.y = pos1.y - pos0.y;
        edge01.z = pos1.z - pos0.z;
        invLen = 1.0f / sqrtf(edge01.x * edge01.x + edge01.y * edge01.y + edge01.z * edge01.z);
        edge01.x *= invLen;
        edge01.y *= invLen;
        edge01.z *= invLen;

        // Edge from v0 to v2
        VxVector edge02;
        edge02.x = pos2.x - pos0.x;
        edge02.y = pos2.y - pos0.y;
        edge02.z = pos2.z - pos0.z;
        invLen = 1.0f / sqrtf(edge02.x * edge02.x + edge02.y * edge02.y + edge02.z * edge02.z);
        edge02.x *= invLen;
        edge02.y *= invLen;
        edge02.z *= invLen;

        // Update UV for v0
        float dot1 = edge01.x * lightDir0.x + edge01.y * lightDir0.y + edge01.z * lightDir0.z;
        float dot2 = edge02.x * lightDir0.x + edge02.y * lightDir0.y + edge02.z * lightDir0.z;
        float u0 = storedUVs[v0 * 2] + dot1 * embossFactor;
        float v0_uv = storedUVs[v0 * 2 + 1] + dot2 * embossFactor;
        mesh->SetVertexTextureCoordinates(v0, u0, v0_uv, matChannel);

        // Calculate light direction to second vertex
        VxVector lightDir1;
        lightDir1.x = lightPos.x - pos1.x;
        lightDir1.y = lightPos.y - pos1.y;
        lightDir1.z = lightPos.z - pos1.z;
        invLen = 1.0f / sqrtf(lightDir1.x * lightDir1.x + lightDir1.y * lightDir1.y + lightDir1.z * lightDir1.z);
        lightDir1.x *= invLen;
        lightDir1.y *= invLen;
        lightDir1.z *= invLen;

        // Edge from v1 to v2
        VxVector edge12;
        edge12.x = pos2.x - pos1.x;
        edge12.y = pos2.y - pos1.y;
        edge12.z = pos2.z - pos1.z;
        invLen = 1.0f / sqrtf(edge12.x * edge12.x + edge12.y * edge12.y + edge12.z * edge12.z);
        edge12.x *= invLen;
        edge12.y *= invLen;
        edge12.z *= invLen;

        // Edge from v1 to v0
        VxVector edge10;
        edge10.x = pos0.x - pos1.x;
        edge10.y = pos0.y - pos1.y;
        edge10.z = pos0.z - pos1.z;
        invLen = 1.0f / sqrtf(edge10.x * edge10.x + edge10.y * edge10.y + edge10.z * edge10.z);
        edge10.x *= invLen;
        edge10.y *= invLen;
        edge10.z *= invLen;

        // Update UV for v2 (index in triangle)
        float dot3 = edge10.x * lightDir1.x + edge10.y * lightDir1.y + edge10.z * lightDir1.z;
        float dot4 = edge12.x * lightDir1.x + edge12.y * lightDir1.y + edge12.z * lightDir1.z;
        float u2 = storedUVs[v2 * 2] + dot4 * embossFactor;
        float v2_uv = storedUVs[v2 * 2 + 1] + dot3 * embossFactor;
        mesh->SetVertexTextureCoordinates(v2, u2, v2_uv, matChannel);

        // Calculate light direction to third vertex
        VxVector lightDir2;
        lightDir2.x = lightPos.x - pos2.x;
        lightDir2.y = lightPos.y - pos2.y;
        lightDir2.z = lightPos.z - pos2.z;
        invLen = 1.0f / sqrtf(lightDir2.x * lightDir2.x + lightDir2.y * lightDir2.y + lightDir2.z * lightDir2.z);
        lightDir2.x *= invLen;
        lightDir2.y *= invLen;
        lightDir2.z *= invLen;

        // Edge from v2 to v0
        VxVector edge20;
        edge20.x = pos0.x - pos2.x;
        edge20.y = pos0.y - pos2.y;
        edge20.z = pos0.z - pos2.z;
        invLen = 1.0f / sqrtf(edge20.x * edge20.x + edge20.y * edge20.y + edge20.z * edge20.z);
        edge20.x *= invLen;
        edge20.y *= invLen;
        edge20.z *= invLen;

        // Edge from v2 to v1
        VxVector edge21;
        edge21.x = pos1.x - pos2.x;
        edge21.y = pos1.y - pos2.y;
        edge21.z = pos1.z - pos2.z;
        invLen = 1.0f / sqrtf(edge21.x * edge21.x + edge21.y * edge21.y + edge21.z * edge21.z);
        edge21.x *= invLen;
        edge21.y *= invLen;
        edge21.z *= invLen;

        // Update UV for v1 (index in triangle)
        float dot5 = edge20.x * lightDir2.x + edge20.y * lightDir2.y + edge20.z * lightDir2.z;
        float dot6 = edge21.x * lightDir2.x + edge21.y * lightDir2.y + edge21.z * lightDir2.z;
        float u1 = storedUVs[v1 * 2] + dot5 * embossFactor;
        float v1_uv = storedUVs[v1 * 2 + 1] + dot6 * embossFactor;
        mesh->SetVertexTextureCoordinates(v1, u1, v1_uv, matChannel);
    }

    mesh->UVChanged();

    return CKBR_ACTIVATENEXTFRAME;
}