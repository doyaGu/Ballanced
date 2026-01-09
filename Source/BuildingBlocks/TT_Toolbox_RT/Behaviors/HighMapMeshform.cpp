//////////////////////////////////////
//////////////////////////////////////
//
//        TT_HighMapMeshform
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorHighMapMeshformDecl();
CKERROR CreateHighMapMeshformProto(CKBehaviorPrototype **pproto);
int HighMapMeshform(const CKBehaviorContext &behcontext);
CKERROR HighMapMeshformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHighMapMeshformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_HighMapMeshform");
    od->SetDescription("Deforms the mesh based on the texture data");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e3c2d7e, 0x7be7234e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHighMapMeshformProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateHighMapMeshformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_HighMapMeshform");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Height", CKPGUID_FLOAT, "10");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(HighMapMeshform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(HighMapMeshformCallBack);

    *pproto = proto;
    return CK_OK;
}

int HighMapMeshform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OK;

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    // Get material channel parameter (-1 = default/first)
    int matChannel = -1;
    beh->GetInputParameterValue(0, &matChannel);

    // Get the texture from the material on the specified channel
    CKMaterial *material = mesh->GetFaceMaterial(0);
    if (!material)
        return CKBR_OK;

    CKTexture *texture = material->GetTexture(matChannel < 0 ? 0 : matChannel);
    if (!texture)
        return CKBR_OK;

    // Get height factor
    float heightFactor = 1.0f;
    beh->GetInputParameterValue(1, &heightFactor);

    // Get vertex data
    CKDWORD vertexStride = 0;
    VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&vertexStride);

    // Get UV data
    CKDWORD uvStride = 0;
    Vx2DVector *uvs = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride);

    int vertexCount = mesh->GetVertexCount();

    // Get texture surface info
    VxImageDescEx imageDesc;
    texture->GetSystemTextureDesc(imageDesc);

    XBYTE *surfacePtr = texture->LockSurfacePtr();
    if (!surfacePtr)
        return CKBR_OK;

    int texWidth = imageDesc.Width;
    int texHeight = imageDesc.Height;

    // Read the stored original vertex positions
    VxVector *originalPositions = (VxVector *)beh->GetLocalParameterReadDataPtr(0);

    // Process each vertex
    for (int i = 0; i < vertexCount; i++)
    {
        VxVector *pos = (VxVector *)((CKBYTE *)positions + i * vertexStride);
        Vx2DVector *uv = (Vx2DVector *)((CKBYTE *)uvs + i * uvStride);

        // Get UV coordinates and wrap to texture bounds
        int u = abs((int)(uv->x * texWidth)) % (texWidth + 1);
        int v = texHeight - abs((int)(uv->y * texHeight)) % (texHeight + 1);

        // Get pixel value (alpha channel from ARGB)
        int pixelOffset = u * 4 + v * texWidth * 4;
        int alphaValue = surfacePtr[pixelOffset + 3];  // HIBYTE of ARGB = Alpha

        // Restore original position
        pos->x = originalPositions[i].x;
        pos->z = originalPositions[i].z;

        // Modify Y based on texture alpha and height factor
        pos->y = (float)alphaValue * heightFactor * 0.01f;
    }

    // Notify mesh that vertices changed
    mesh->ModifyObjectFlags(CK_3DENTITY_UPDATELASTFRAME, 0);

    texture->ReleaseSurfacePtr();

    return CKBR_OK;
}

CKERROR HighMapMeshformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDETACH:
        {
            // Restore original vertex positions
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetVertexCount();
            CKDWORD vertexStride = 0;
            VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&vertexStride);

            VxVector *originalPositions = (VxVector *)beh->GetLocalParameterReadDataPtr(0);
            if (!originalPositions)
                return CKBR_OK;

            // Restore positions
            for (int i = 0; i < vertexCount; i++)
            {
                VxVector *pos = (VxVector *)((CKBYTE *)positions + i * vertexStride);
                pos->x = originalPositions[i].x;
                pos->y = originalPositions[i].y;
                pos->z = originalPositions[i].z;
            }

            mesh->ModifyObjectFlags(CK_3DENTITY_UPDATELASTFRAME, 0);
        }
        break;

    case CKM_BEHAVIORATTACH:
        {
            // Store original vertex positions
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetVertexCount();
            CKDWORD vertexStride = 0;
            VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&vertexStride);

            // Allocate buffer for original positions
            int bufferSize = vertexCount * sizeof(VxVector);
            VxVector *originalPositions = new VxVector[vertexCount];

            // Copy original positions
            for (int i = 0; i < vertexCount; i++)
            {
                VxVector *pos = (VxVector *)((CKBYTE *)positions + i * vertexStride);
                originalPositions[i] = *pos;
            }

            // Store in local parameter
            beh->SetLocalParameterValue(0, originalPositions, bufferSize);
            delete[] originalPositions;
        }
        break;
    }

    return CKBR_OK;
}