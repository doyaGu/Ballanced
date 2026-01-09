/////////////////////////////////////
/////////////////////////////////////
//
//        TT MagnetMeshForm
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>

CKObjectDeclaration *FillBehaviorMagnetMeshFormDecl();
CKERROR CreateMagnetMeshFormProto(CKBehaviorPrototype **pproto);
int MagnetMeshForm(const CKBehaviorContext &behcontext);
CKERROR MagnetMeshFormCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMagnetMeshFormDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT MagnetMeshForm");
    od->SetDescription("Object is magnetically attracted to another");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x8e53929, 0x4c9c5205));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMagnetMeshFormProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateMagnetMeshFormProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT MagnetMeshForm");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Magnet-Entity", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Max Distance", CKPGUID_FLOAT, "5.0");
    proto->DeclareInParameter("Min Distance", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Strength", CKPGUID_FLOAT, "1");

    proto->DeclareSetting("Sinus-Deform?", CKPGUID_BOOL, "false");

    proto->DeclareLocalParameter("VArray", CKPGUID_POINTER, "NULL");
    proto->DeclareLocalParameter("Time", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MagnetMeshForm);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(MagnetMeshFormCallBack);

    *pproto = proto;
    return CK_OK;
}

int MagnetMeshForm(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    CK3dEntity *magnet = (CK3dEntity *)beh->GetInputParameterObject(0);

    float maxDistance = 0.0f;
    beh->GetInputParameterValue(1, &maxDistance);

    float minDistance = 0.0f;
    beh->GetInputParameterValue(2, &minDistance);

    float strength = 0.0f;
    beh->GetInputParameterValue(3, &strength);

    // Get sinus deform setting
    CKBOOL sinusDeform = FALSE;
    beh->GetLocalParameterValue(0, &sinusDeform);

    float time = 0.0f;
    if (sinusDeform)
    {
        beh->GetLocalParameterValue(2, &time);
        time += behcontext.DeltaTime * 0.01f;
        beh->SetLocalParameterValue(2, &time);
    }

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    CKDWORD vertexStride = 0;
    mesh->GetModifierVertices(&vertexStride);
    int vertexCount = mesh->GetModifierVertexCount();

    // Get stored original vertices
    VxVector *originalVertices = (VxVector *)beh->GetLocalParameterReadDataPtr(1);

    VxVector targetPos;
    target->GetPosition(&targetPos);

    VxVector magnetPos;
    magnet->GetPosition(&magnetPos, target);

    float magnetRadius = magnet->GetRadius();

    // Precompute strength factor
    float strengthFactor = strength / (minDistance - maxDistance);

    // Check for Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Check for On input
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Reset time
        float resetTime = 0.0f;
        beh->SetLocalParameterValue(2, &resetTime);
    }

    // Process each vertex
    for (int i = 0; i < vertexCount; i++)
    {
        VxVector *origPos = &originalVertices[i];

        // Calculate direction to magnet
        VxVector dir;
        dir.x = magnetPos.x - origPos->x;
        dir.y = magnetPos.y - origPos->y;
        dir.z = magnetPos.z - origPos->z;

        // Calculate distance to magnet
        float distance = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z) - magnetRadius;

        float deformAmount = 0.0f;

        if (distance >= maxDistance)
        {
            // Outside max distance - no deformation
            mesh->SetVertexPosition(i, origPos);
            continue;
        }
        else if (distance <= minDistance)
        {
            // Inside min distance - full attraction
            deformAmount = (distance + magnetRadius) / (magnetRadius + minDistance) * strength;
        }
        else
        {
            // Between min and max - interpolate
            deformAmount = (distance - maxDistance) * strengthFactor;
        }

        // Apply sinus modulation if enabled
        if (sinusDeform)
        {
            int sinIndex = abs((int)(fabsf(origPos->z - time) * 50.0f)) % 360;
            float sinValue = sinf(sinIndex * 3.14159265f / 180.0f);
            deformAmount *= (sinValue * 0.3f + 1.0f);
        }

        // Normalize direction
        float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (length > 0.0001f)
        {
            dir.x /= length;
            dir.y /= length;
            dir.z /= length;
        }

        // Apply deformation
        VxVector newPos;
        newPos.x = origPos->x + dir.x * deformAmount;
        newPos.y = origPos->y + dir.y * deformAmount;
        newPos.z = origPos->z + dir.z * deformAmount;

        mesh->SetVertexPosition(i, &newPos);
    }

    mesh->ModifierVertexMove(TRUE, TRUE);
    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR MagnetMeshFormCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
        {
            // Restore original vertices
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetModifierVertexCount();
            CKDWORD vertexStride = 0;
            VxVector *vertices = (VxVector *)mesh->GetModifierVertices(&vertexStride);

            VxVector *originalVertices = (VxVector *)beh->GetLocalParameterReadDataPtr(1);
            if (!originalVertices)
                return CKBR_OK;

            // Restore each vertex
            for (int i = 0; i < vertexCount; i++)
            {
                VxVector *vertex = (VxVector *)((CKBYTE *)vertices + i * vertexStride);
                *vertex = originalVertices[i];
            }

            mesh->ModifierVertexMove(TRUE, TRUE);
        }
        break;

    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
        {
            // Store original vertices
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetModifierVertexCount();
            CKDWORD vertexStride = 0;
            VxVector *vertices = (VxVector *)mesh->GetModifierVertices(&vertexStride);

            // Allocate buffer for original vertices
            int bufferSize = vertexCount * sizeof(VxVector);
            VxVector *originalVertices = new VxVector[vertexCount];

            // Copy vertices
            for (int i = 0; i < vertexCount; i++)
            {
                VxVector *vertex = (VxVector *)((CKBYTE *)vertices + i * vertexStride);
                originalVertices[i] = *vertex;
            }

            beh->SetLocalParameterValue(1, originalVertices, bufferSize);
            delete[] originalVertices;
        }
        break;
    }

    return CKBR_OK;
}