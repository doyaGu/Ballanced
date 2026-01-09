/////////////////////////////////
/////////////////////////////////
//
//        TT_RippleWave
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>
#include <stdlib.h>

// Random in range helper
static float RippleRandomRange(float min, float max)
{
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

CKObjectDeclaration *FillBehaviorRippleWaveDecl();
CKERROR CreateRippleWaveProto(CKBehaviorPrototype **pproto);
int RippleWave(const CKBehaviorContext &behcontext);
CKERROR RippleWaveCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRippleWaveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RippleWave");
    od->SetDescription("Creates wavy rings");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2f6a47b3, 0x35f4755c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRippleWaveProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateRippleWaveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RippleWave");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Drop");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit In");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("DropStrength", CKPGUID_FLOAT, "1.0f");
    proto->DeclareInParameter("Viscosity", CKPGUID_FLOAT, "0.98f");
    proto->DeclareInParameter("Speed", CKPGUID_FLOAT, "0.25f");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RippleWave);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(RippleWaveCallBack);

    *pproto = proto;
    return CK_OK;
}

int RippleWave(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_PARAMETERERROR;

    int vertexCount = mesh->GetVertexCount();

    // Check if mesh is square (n x n grid)
    int gridSize = (int)sqrtf((float)vertexCount);
    if (gridSize * gridSize != vertexCount)
    {
        ctx->OutputToConsole("Mesh hat keine quadratische Topologie!", TRUE);
        return CKBR_PARAMETERERROR;
    }

    // Get parameters
    float dropStrength = 0.0f;
    beh->GetInputParameterValue(0, &dropStrength);

    float viscosity = 0.0f;
    beh->GetInputParameterValue(1, &viscosity);

    float speed = 0.0f;
    beh->GetInputParameterValue(1, &speed);  // Note: original code gets param 1 twice, likely a bug

    // Handle Off input
    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Handle In input (initialization)
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Get current vertex positions
        CKDWORD stride = 0;
        VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&stride);

        // Allocate original vertices buffer
        VxVector *origVertices = new VxVector[vertexCount];
        for (int i = 0; i < vertexCount; i++)
        {
            VxVector *pos = (VxVector *)((CKBYTE *)positions + i * stride);
            origVertices[i] = *pos;
        }
        beh->SetLocalParameterValue(1, origVertices, sizeof(VxVector) * vertexCount);
        delete[] origVertices;

        // Allocate wave buffer (2 floats per vertex: current height and velocity)
        int bufferSize = vertexCount * 2;
        float *waveBuffer = new float[bufferSize];
        memset(waveBuffer, 0, bufferSize * sizeof(float));
        beh->SetLocalParameterValue(0, &waveBuffer, sizeof(float*));
        // Note: buffer is stored as pointer, will need to free later
    }

    // Get wave buffer
    float **waveBufferPtr = (float **)beh->GetLocalParameterReadDataPtr(0);
    float *waveBuffer = *waveBufferPtr;

    // Get original vertices
    VxVector *origVertices = (VxVector *)beh->GetLocalParameterReadDataPtr(1);

    // Handle Drop input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        // Create a drop at random position (avoiding edges)
        float maxPos = (float)(gridSize - 1);
        int dropX = (int)RippleRandomRange(1.0f, maxPos);
        int dropY = (int)RippleRandomRange(1.0f, maxPos);

        int centerIdx = dropX + gridSize * dropY;

        // Apply drop impulse (3x3 pattern)
        waveBuffer[2 * centerIdx + 1] -= dropStrength * 0.25f;

        // Adjacent cells
        waveBuffer[2 * centerIdx - 1] -= dropStrength * 0.125f;
        waveBuffer[2 * centerIdx + 3] -= dropStrength * 0.125f;
        waveBuffer[2 * (centerIdx - gridSize) + 1] -= dropStrength * 0.125f;
        waveBuffer[2 * (centerIdx + gridSize) + 1] -= dropStrength * 0.125f;

        // Diagonal cells
        waveBuffer[2 * (centerIdx - gridSize) - 1] -= dropStrength * 0.0625f;
        waveBuffer[2 * (centerIdx - gridSize) + 3] -= dropStrength * 0.0625f;
        waveBuffer[2 * (centerIdx + gridSize) - 1] -= dropStrength * 0.0625f;
        waveBuffer[2 * (centerIdx + gridSize) + 3] -= dropStrength * 0.0625f;
    }

    // Wave simulation - update each cell
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            int idx = x + y * gridSize;
            float *cell = &waveBuffer[2 * idx];

            float laplacian = 0.0f;

            // Only process interior cells (avoid edges)
            if (x > 1 && x < gridSize - 1 && y > 1 && y < gridSize - 1)
            {
                // Get neighbor heights
                float left = waveBuffer[2 * (idx - 1)];
                float right = waveBuffer[2 * (idx + 1)];
                float up = waveBuffer[2 * (idx - gridSize)];
                float down = waveBuffer[2 * (idx + gridSize)];

                float upLeft = waveBuffer[2 * (idx - gridSize - 1)];
                float upRight = waveBuffer[2 * (idx - gridSize + 1)];
                float downLeft = waveBuffer[2 * (idx + gridSize - 1)];
                float downRight = waveBuffer[2 * (idx + gridSize + 1)];

                // Laplacian with diagonal weighting
                laplacian = (left + right + up + down) * 0.166f +
                            (upLeft + upRight + downLeft + downRight) * 0.083f;
            }
            else
            {
                laplacian = 0.0f;
                cell[0] = 0.0f;
                cell[1] = 0.0f;
            }

            // Update velocity with damping
            cell[1] = viscosity * cell[1];

            // Update velocity based on wave equation
            cell[1] = cell[1] + (laplacian - cell[0]) * speed;
        }
    }

    // Apply wave heights to mesh
    CKDWORD stride = 0;
    VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&stride);

    for (int i = 0; i < vertexCount; i++)
    {
        // Update wave height: add velocity to position
        float newHeight = waveBuffer[2 * i] + waveBuffer[2 * i + 1];
        waveBuffer[2 * i] = newHeight;

        // Set vertex position
        VxVector *pos = (VxVector *)((CKBYTE *)positions + i * stride);
        pos->x = origVertices[i].x;
        pos->y = newHeight;
        pos->z = origVertices[i].z;
    }

    mesh->ModifyObjectFlags(CK_3DENTITY_UPDATELASTFRAME, 0);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR RippleWaveCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Callback for cleanup - in full implementation would free wave buffer
    return CKBR_OK;
}