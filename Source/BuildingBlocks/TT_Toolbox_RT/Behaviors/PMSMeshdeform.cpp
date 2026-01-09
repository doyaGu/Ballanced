/////////////////////////////////////
/////////////////////////////////////
//
//        TT PMS_Meshdeform
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

#include <math.h>
#include <stdlib.h>

// Wave parameters structure
struct WaveParams
{
    float wavelength;       // Wave length
    float k;                // Wave number (2*PI/wavelength)
    float phase;            // Initial phase
    float amplitude;        // Wave amplitude
    float frequency;        // Angular frequency
    float speed;            // Phase velocity
    float currentPhase;     // Current phase for animation
};

// Helper function for random in range
static float RandomRange(float min, float max)
{
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

CKObjectDeclaration *FillBehaviorPMSMeshdeformDecl();
CKERROR CreatePMSMeshdeformProto(CKBehaviorPrototype **pproto);
int PMSMeshdeform(const CKBehaviorContext &behcontext);
CKERROR PMSMeshdeformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPMSMeshdeformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PMS_Meshdeform");
    od->SetDescription("Generates waves via Pierson-Moskowitz spectrum");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38b87d43, 0x149b755b));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePMSMeshdeformProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreatePMSMeshdeformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PMS_Meshdeform");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Winddirection", CKPGUID_ANGLE, "0");
    proto->DeclareInParameter("Max. Amplitude", CKPGUID_FLOAT, "0.6");
    proto->DeclareInParameter("WaveScale", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("TimeScale", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1.WaveLength in Y", CKPGUID_FLOAT, "24");
    proto->DeclareInParameter("1.WaveLength in X", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("1.Amplitude in X", CKPGUID_FLOAT, "0.5");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Time", CKPGUID_FLOAT);
    proto->DeclareLocalParameter("WaveX", CKPGUID_POINTER);
    proto->DeclareLocalParameter("WaveY", CKPGUID_POINTER);
    proto->DeclareLocalParameter("WindMatrix", CKPGUID_MATRIX);

    proto->DeclareSetting("Wellenanzahl", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PMSMeshdeform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(PMSMeshdeformCallBack);

    *pproto = proto;
    return CK_OK;
}

int PMSMeshdeform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    CKMesh *mesh = target->GetCurrentMesh();
    if (!mesh)
        return CKBR_PARAMETERERROR;

    // Get wave count from settings
    int waveCount = 1;
    beh->GetLocalParameterValue(0, &waveCount);

    // Get input parameters
    float windDirection = 0.0f;
    beh->GetInputParameterValue(0, &windDirection);

    float maxAmplitude = 0.6f;
    beh->GetInputParameterValue(1, &maxAmplitude);

    float waveScale = 1.0f;
    beh->GetInputParameterValue(2, &waveScale);
    waveScale *= 0.2f;  // Scale factor from original

    Vx2DVector timeScale;
    beh->GetInputParameterValue(3, &timeScale);

    // Handle Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Handle On input (initialization)
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        srand(10);

        // Allocate wave Y parameters
        WaveParams *waveY = new WaveParams[waveCount];
        memset(waveY, 0, sizeof(WaveParams) * waveCount);

        // Allocate wave X parameters
        WaveParams *waveX = new WaveParams[waveCount];
        memset(waveX, 0, sizeof(WaveParams) * waveCount);

        const float PI2 = 6.283185f;
        const float gravity = 9.81f;
        float pi2_4 = powf(PI2, 4.0f);

        int paramIndex = 4;
        for (int i = 0; i < waveCount; i++)
        {
            // Get wavelength Y
            beh->GetInputParameterValue(paramIndex, &waveY[i].wavelength);
            waveY[i].k = PI2 / waveY[i].wavelength;
            waveY[i].frequency = sqrtf(waveY[i].k * gravity) * 0.1591549f;

            // Pierson-Moskowitz spectrum calculation
            waveY[i].amplitude = 0.7698888f / (powf(waveY[i].frequency, 5.0f) * pi2_4);
            float expFactor = -1.0f * powf(waveY[i].frequency * 3.9216f, -4.0f);
            waveY[i].amplitude *= expf(expFactor);
            waveY[i].amplitude /= (0.16f / maxAmplitude);
            waveY[i].amplitude *= RandomRange(-1.0f, 1.0f);

            waveY[i].phase = RandomRange(0.0f, PI2);
            waveY[i].speed = sqrtf(gravity / waveY[i].k) * 5.0f;

            // Get wavelength X
            beh->GetInputParameterValue(paramIndex + 1, &waveX[i].wavelength);
            if (waveX[i].wavelength != 0.0f)
                waveX[i].k = PI2 / waveX[i].wavelength;
            else
                waveX[i].k = 0.0f;

            // Get amplitude X
            beh->GetInputParameterValue(paramIndex + 2, &waveX[i].amplitude);
            waveX[i].phase = RandomRange(0.0f, PI2);

            paramIndex += 3;
        }

        // Store wave parameters
        beh->SetLocalParameterValue(3, waveY, sizeof(WaveParams) * waveCount);
        beh->SetLocalParameterValue(4, waveX, sizeof(WaveParams) * waveCount);

        // Create wind rotation matrix
        VxVector axis = {0.0f, 1.0f, 0.0f};
        VxMatrix windMatrix;
        Vx3DMatrixFromRotation(windMatrix, axis, windDirection);
        beh->SetLocalParameterValue(5, &windMatrix);

        delete[] waveY;
        delete[] waveX;
    }

    // Get time and update
    float time = 0.0f;
    beh->GetLocalParameterValue(2, &time);
    time += timeScale.x * behcontext.DeltaTime * 0.0001f;
    beh->SetLocalParameterValue(2, &time);

    // Get wave parameters
    WaveParams *waveY = (WaveParams *)beh->GetLocalParameterReadDataPtr(3);
    WaveParams *waveX = (WaveParams *)beh->GetLocalParameterReadDataPtr(4);

    // Get wind matrix
    VxMatrix windMatrix;
    beh->GetLocalParameterValue(5, &windMatrix);

    // Update wave phases
    for (int i = 0; i < waveCount; i++)
    {
        waveY[i].currentPhase = waveY[i].phase - time * waveY[i].speed;
    }

    // Get vertex data
    CKDWORD stride = 0;
    VxVector *positions = (VxVector *)mesh->GetPositionsPtr(&stride);
    int vertexCount = mesh->GetVertexCount();

    // Process each vertex
    for (int v = 0; v < vertexCount; v++)
    {
        VxVector *pos = (VxVector *)((CKBYTE *)positions + v * stride);
        VxVector originalPos = *pos;

        // Rotate by wind direction
        VxVector rotatedPos;
        Vx3DRotateVector(&rotatedPos, windMatrix, &originalPos);
        rotatedPos.x *= waveScale;
        rotatedPos.y *= waveScale;
        rotatedPos.z *= waveScale;

        // Calculate Y displacement from wave Y
        float dispY = 0.0f;
        for (int w = 0; w < waveCount; w++)
        {
            float phase = rotatedPos.z * waveY[w].k - waveY[w].currentPhase;
            dispY += sinf(phase) * waveY[w].amplitude;
        }

        // Calculate additional displacement from wave X
        rotatedPos.x += dispY;
        float dispX = 0.0f;
        for (int w = 0; w < waveCount; w++)
        {
            float phase = rotatedPos.x * waveX[w].k - waveX[w].phase;
            dispX += sinf(phase) * waveX[w].amplitude;

            // Add harmonic
            float phase2 = rotatedPos.x * waveX[w].k * 0.5f - waveX[w].phase;
            dispX += sinf(phase2) * waveX[w].amplitude * 0.8f;
        }

        // Apply displacement
        originalPos.y = dispX;
        *pos = originalPos;
    }

    mesh->ModifyObjectFlags(CK_3DENTITY_UPDATELASTFRAME, 0);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR PMSMeshdeformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Callback not needed for this implementation
    return CKBR_OK;
}