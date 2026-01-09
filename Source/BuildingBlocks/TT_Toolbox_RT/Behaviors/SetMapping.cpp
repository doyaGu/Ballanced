/////////////////////////////////
/////////////////////////////////
//
//        TT_SetMapping
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetMappingDecl();
CKERROR CreateSetMappingProto(CKBehaviorPrototype **pproto);
int SetMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_SetMapping");
    od->SetDescription("Change the UV coordinates for the Material Channels");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2e36319f, 0x6ec6031f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateSetMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_SetMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("New planar Mapping", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Rotate", CKPGUID_ANGLE, "0.0");
    proto->DeclareInParameter("Offset UV", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Tile U", CKPGUID_2DVECTOR, "1,1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int SetMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    // Get input parameters
    int matChannel = -1;
    beh->GetInputParameterValue(0, &matChannel);

    CKBOOL newPlanarMapping = FALSE;
    beh->GetInputParameterValue(1, &newPlanarMapping);

    float rotateAngle = 0.0f;
    beh->GetInputParameterValue(2, &rotateAngle);

    Vx2DVector offsetUV(0.0f, 0.0f);
    beh->GetInputParameterValue(3, &offsetUV);

    Vx2DVector tileUV(1.0f, 1.0f);
    beh->GetInputParameterValue(4, &tileUV);

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    int channelCount = mesh->GetChannelCount();
    if (matChannel < 0)
        matChannel = 0;
    if (matChannel >= channelCount)
        return CKBR_PARAMETERERROR;

    int vertexCount = mesh->GetVertexCount();
    CKDWORD uvStride = 0;
    Vx2DVector *uvCoords = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, matChannel);
    if (!uvCoords)
        return CKBR_PARAMETERERROR;

    float uvMinX = 0.0f;
    float uvMinY = 0.0f;
    float uvMaxX = 1.0f;
    float uvMaxY = 1.0f;

    if (newPlanarMapping)
    {
        // Generate planar mapping from object space positions
        const VxBbox &bbox = target->GetBoundingBox(TRUE);
        VxBbox localBbox;
        memcpy(&localBbox, &bbox, sizeof(VxBbox));

        CKDWORD vertStride = 0;
        VxVector *modVerts = (VxVector *)mesh->GetModifierVertices(&vertStride);

        float sizeX = localBbox.Min.x - localBbox.Max.x;
        float sizeZ = localBbox.Max.z - localBbox.Min.z;
        float invSizeX = (sizeX != 0.0f) ? 1.0f / sizeX : 0.0f;
        float invSizeZ = (sizeZ != 0.0f) ? 1.0f / sizeZ : 0.0f;

        Vx2DVector *uv = uvCoords;
        VxVector *vert = modVerts;
        for (int i = 0; i < vertexCount; i++)
        {
            uv->x = (vert->x - localBbox.Max.x) * invSizeX;
            uv->y = (vert->z - localBbox.Min.z) * invSizeZ;

            uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
            vert = (VxVector *)((CKBYTE *)vert + vertStride);
        }
        uvMaxX = 1.0f;
        uvMaxY = 1.0f;
    }
    else
    {
        // Find UV bounds
        Vx2DVector *uv = uvCoords;
        if (vertexCount > 0)
        {
            uvMinX = uv->x;
            uvMaxX = uv->x;
            uvMinY = uv->y;
            uvMaxY = uv->y;
        }

        for (int i = 1; i < vertexCount; i++)
        {
            uv = (Vx2DVector *)((CKBYTE *)uvCoords + uvStride * i);
            if (uv->x < uvMinX)
                uvMinX = uv->x;
            if (uv->x > uvMaxX)
                uvMaxX = uv->x;
            if (uv->y < uvMinY)
                uvMinY = uv->y;
            if (uv->y > uvMaxY)
                uvMaxY = uv->y;
        }
    }

    // Apply rotation and tiling
    float halfRangeX = (uvMaxX - uvMinX) * 0.5f;
    float halfRangeY = (uvMaxY - uvMinY) * 0.5f;

    Vx2DVector *uv = uvCoords;
    for (int i = 0; i < vertexCount; i++)
    {
        // Center UV around (0,0) then rotate
        float centeredX = uv->x - halfRangeX - uvMinX;
        float centeredY = uvMaxY - uv->y - halfRangeY - uvMinY;

        float cosAngle = cosf(rotateAngle);
        float sinAngle = sinf(rotateAngle);

        // Rotate
        float rotatedX = sinAngle * centeredY + cosAngle * centeredX;
        float rotatedY = cosAngle * centeredY - sinAngle * centeredX;

        // Apply tiling and recenter
        uv->x = tileUV.x * rotatedX + halfRangeX + uvMinX;
        uv->y = halfRangeY - tileUV.y * rotatedY + uvMinY;

        // Apply offset
        uv->x += offsetUV.x;
        uv->y += offsetUV.y;

        uv = (Vx2DVector *)((CKBYTE *)uv + uvStride);
    }

    mesh->UVChanged();

    return CKBR_OK;
}
