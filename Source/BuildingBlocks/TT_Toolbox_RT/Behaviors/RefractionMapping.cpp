////////////////////////////////////////
////////////////////////////////////////
//
//        TT_RefractionMapping
//
////////////////////////////////////////
////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorRefractionMappingDecl();
CKERROR CreateRefractionMappingProto(CKBehaviorPrototype **pproto);
int RefractionMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRefractionMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RefractionMapping");
    od->SetDescription("Creates refraction mapping when changing the mappings channel normals");
    od->SetCategory("TT Toolbox/Mapping");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53024810, 0x22ed280f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRefractionMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateRefractionMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RefractionMapping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareInParameter("Camera", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Depth", CKPGUID_FLOAT, "10.0");
    proto->DeclareInParameter("Refractionfactor", CKPGUID_FLOAT, "0.95");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RefractionMapping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));

    *pproto = proto;
    return CK_OK;
}

int RefractionMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    // Handle Off input - stop processing
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Handle On input
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

    CK3dEntity *camera = (CK3dEntity *)beh->GetInputParameterObject(1);

    float depth = 10.0f;
    beh->GetInputParameterValue(2, &depth);

    float refractionFactor = 0.95f;
    beh->GetInputParameterValue(3, &refractionFactor);

    // Get camera position in target's local space
    VxVector cameraPos;
    camera->GetPosition(&cameraPos, target);

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    // Get bounding box
    const VxBbox &bbox = target->GetBoundingBox(TRUE);
    VxBbox localBbox;
    memcpy(&localBbox, &bbox, sizeof(VxBbox));

    float invSizeX = 1.0f / (localBbox.Min.x - localBbox.Max.x);
    float invSizeZ = 1.0f / (localBbox.Min.z - localBbox.Max.z);

    // Get vertex data
    CKDWORD uvStride = 0;
    Vx2DVector *uvCoords = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&uvStride, matChannel);

    CKDWORD vertStride = 0;
    VxVector *vertices = (VxVector *)mesh->GetModifierVertices(&vertStride);

    CKDWORD normStride = 0;
    VxVector *normals = (VxVector *)mesh->GetNormalsPtr(&normStride);

    int vertexCount = mesh->GetModifierVertexCount();

    // Process each vertex
    for (int i = 0; i < vertexCount; i++)
    {
        // Calculate direction from vertex to camera
        VxVector toCamera;
        toCamera.x = cameraPos.x - vertices->x;
        toCamera.y = cameraPos.y - vertices->y;
        toCamera.z = cameraPos.z - vertices->z;
        toCamera.Normalize();

        // Apply refraction using normal
        VxVector scaledNormal;
        scaledNormal.x = refractionFactor * normals->x;
        scaledNormal.y = refractionFactor * normals->y;
        scaledNormal.z = refractionFactor * normals->z;

        // Calculate refracted direction
        VxVector refracted;
        refracted.x = -(toCamera.x + scaledNormal.x);
        refracted.y = -(toCamera.y + scaledNormal.y);
        refracted.z = -(toCamera.z + scaledNormal.z);
        refracted.Normalize();

        // Project onto plane at depth
        float t = (vertices->y - depth) / refracted.y;
        float projX = refracted.x * t + vertices->x;
        float projZ = refracted.z * t + vertices->z;

        // Convert to UV coordinates
        uvCoords->x = (projX - localBbox.Max.x) * invSizeX;
        uvCoords->y = (projZ - localBbox.Max.z) * invSizeZ;

        // Advance pointers
        uvCoords = (Vx2DVector *)((CKBYTE *)uvCoords + uvStride);
        vertices = (VxVector *)((CKBYTE *)vertices + vertStride);
        normals = (VxVector *)((CKBYTE *)normals + normStride);
    }

    mesh->UVChanged();

    return CKBR_ACTIVATENEXTFRAME;
}