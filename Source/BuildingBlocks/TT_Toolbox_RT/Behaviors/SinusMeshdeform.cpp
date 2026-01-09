//////////////////////////////////////
//////////////////////////////////////
//
//        TT SinusMeshdeform
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSinusMeshdeformDecl();
CKERROR CreateSinusMeshdeformProto(CKBehaviorPrototype **pproto);
int SinusMeshdeform(const CKBehaviorContext &behcontext);
CKERROR SinusMeshdeformCallBack(const CKBehaviorContext &behcontext);

// Precomputed sine table (361 entries for 0-360 degrees)
static float g_SinusTable[361];
static bool g_SinusTableInitialized = false;

static void InitSinusTable()
{
    if (g_SinusTableInitialized)
        return;
    for (int i = 0; i <= 360; i++)
    {
        g_SinusTable[i] = sinf((float)i / 360.0f * 6.2831855f);
    }
    g_SinusTableInitialized = true;
}

// Structure to hold sinus wave parameters
struct SinusWaveParams
{
    VxVector position;     // Wave center position (offset 0)
    float waveK;           // 2*PI*diagonal / wavelength (offset 8)
    float unused;          // (offset 12)
    float height;          // Height percentage (offset 20)
    float phase;           // Current phase (offset 24)
    float time;            // Accumulated time (offset 28)
    float timeFactor;      // Time factor (offset 32)
};

CKObjectDeclaration *FillBehaviorSinusMeshdeformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SinusMeshdeform");
    od->SetDescription("Generates sine waves");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f907f12, 0x16af15d6));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSinusMeshdeformProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateSinusMeshdeformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SinusMeshdeform");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("1. Timefactor", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1. Heigth", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1. Wavelength", CKPGUID_FLOAT, "20");
    proto->DeclareInParameter("1. Sinpos", CKPGUID_VECTOR, "0.0,0.0,0.0");

    proto->DeclareLocalParameter("SinusCount", CKPGUID_INT, "1");
    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Wave Params", CKPGUID_POINTER);

    proto->DeclareSetting("SinusCount", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SinusMeshdeform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SinusMeshdeformCallBack);

    *pproto = proto;
    return CK_OK;
}

int SinusMeshdeform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (!beh)
        return CKBR_OK;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_PARAMETERERROR;

    int sinusCount = 0;
    beh->GetLocalParameterValue(0, &sinusCount);

    // Handle Off input - stop processing
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Handle On input - initialize
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        // Initialize sine table
        InitSinusTable();

        // Calculate mesh bounding diagonal
        const VxBbox &bbox = target->GetBoundingBox(TRUE);
        float dx = bbox.Min.x - bbox.Max.x;
        float dy = bbox.Min.y - bbox.Max.y;
        float dz = bbox.Min.z - bbox.Max.z;
        float diagonal = sqrtf(dx * dx + dy * dy + dz * dz);

        // Allocate wave parameters array (36 bytes per wave = 9 floats)
        SinusWaveParams *params = new SinusWaveParams[sinusCount];
        if (params)
        {
            memset(params, 0, sizeof(SinusWaveParams) * sinusCount);
        }

        // Initialize wave parameters from input parameters
        int inputCount = beh->GetInputParameterCount();
        int baseParam = inputCount - 4 * sinusCount;
        float waveK = diagonal * 6.28f;

        for (int i = 0; i < sinusCount; i++)
        {
            float heightPercent = 0.0f;
            beh->GetInputParameterValue(baseParam, &heightPercent);
            params[i].height = heightPercent * 0.01f;

            beh->GetInputParameterValue(baseParam + 1, &params[i].timeFactor);

            float wavelength = 1.0f;
            beh->GetInputParameterValue(baseParam + 2, &wavelength);
            params[i].waveK = waveK / wavelength;

            beh->GetInputParameterValue(baseParam + 3, &params[i].position);

            params[i].time = 0.0f;
            params[i].phase = 0.0f;

            baseParam += 4;
        }

        beh->SetLocalParameterValue(2, &params, sizeof(void *));
    }

    // Get wave parameters pointer
    SinusWaveParams *waveParams = *(SinusWaveParams **)beh->GetLocalParameterReadDataPtr(2);

    // Update time and phase for each wave
    for (int i = 0; i < sinusCount; i++)
    {
        waveParams[i].time += waveParams[i].waveK * behcontext.DeltaTime;
        waveParams[i].phase = waveParams[i].time * waveParams[i].timeFactor;
    }

    // Get mesh and vertices
    CKMesh *mesh = target->GetCurrentMesh();
    CKDWORD stride = 0;
    VxVector *vertices = (VxVector *)mesh->GetModifierVertices(&stride);
    int vertexCount = mesh->GetModifierVertexCount();

    // Process each vertex
    for (int v = 0; v < vertexCount; v++)
    {
        VxVector pos = *vertices;
        float height = 0.0f;

        // Sum contributions from all waves
        for (int w = 0; w < sinusCount; w++)
        {
            float dx = pos.x - waveParams[w].position.x;
            float dy = 0.0f - waveParams[w].position.y;
            float dz = pos.z - waveParams[w].position.z;
            float distance = sqrtf(dx * dx + dy * dy + dz * dz);

            float phaseOffset = distance * waveParams[w].timeFactor - waveParams[w].phase;
            int tableIndex = (int)fabsf(phaseOffset) % 360;
            height += g_SinusTable[tableIndex] * waveParams[w].height;
        }

        vertices->x = pos.x;
        vertices->y = height;
        vertices->z = pos.z;

        vertices = (VxVector *)((CKBYTE *)vertices + stride);
    }

    mesh->ModifierVertexMove(TRUE, TRUE);
    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR SinusMeshdeformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORRESET:
        {
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetModifierVertexCount();
            CKDWORD stride = 0;
            VxVector *modVerts = (VxVector *)mesh->GetModifierVertices(&stride);

            // Allocate buffer and copy original vertices
            VxVector *vertexBuffer = new VxVector[vertexCount];
            if (vertexBuffer)
            {
                memset(vertexBuffer, 0, sizeof(VxVector) * vertexCount);
            }

            for (int i = 0; i < vertexCount; i++)
            {
                vertexBuffer[i] = *modVerts;
                modVerts = (VxVector *)((CKBYTE *)modVerts + stride);
            }

            beh->SetLocalParameterValue(1, vertexBuffer, sizeof(VxVector) * vertexCount);
            delete[] vertexBuffer;
        }
        break;

    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
        {
            CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
            if (!target)
                return CKBR_OK;

            CKMesh *mesh = target->GetCurrentMesh();
            if (!mesh)
                return CKBR_OK;

            int vertexCount = mesh->GetModifierVertexCount();
            CKDWORD stride = 0;
            VxVector *modVerts = (VxVector *)mesh->GetModifierVertices(&stride);

            VxVector *savedVerts = (VxVector *)beh->GetLocalParameterReadDataPtr(1);
            if (!savedVerts)
                return CKBR_OK;

            // Restore original vertices
            for (int i = 0; i < vertexCount; i++)
            {
                *modVerts = savedVerts[i];
                modVerts = (VxVector *)((CKBYTE *)modVerts + stride);
            }

            mesh->ModifierVertexMove(TRUE, TRUE);
        }
        break;

    case CKM_BEHAVIORSETTINGSEDITED:
        {
            int sinusCount = 1;
            beh->GetLocalParameterValue(0, &sinusCount);
            if (sinusCount < 1)
            {
                sinusCount = 1;
                beh->SetLocalParameterValue(0, &sinusCount);
            }

            int inputCount = beh->GetInputParameterCount();
            int currentCount = inputCount / 4;
            char paramName[32];

            // Remove excess parameters
            while (currentCount > sinusCount)
            {
                int idx = inputCount - 1;
                CKParameterIn *pin = beh->RemoveInputParameter(idx);
                CKDestroyObject(pin);
                pin = beh->RemoveInputParameter(--idx);
                CKDestroyObject(pin);
                pin = beh->RemoveInputParameter(--idx);
                CKDestroyObject(pin);
                pin = beh->RemoveInputParameter(--idx);
                CKDestroyObject(pin);
                inputCount -= 4;
                currentCount--;
            }

            // Add missing parameters
            while (currentCount < sinusCount)
            {
                int num = currentCount + 1;
                sprintf(paramName, "%d. Timefactor", num);
                beh->CreateInputParameter(paramName, CKPGUID_FLOAT);
                sprintf(paramName, "%d. Heigth", num);
                beh->CreateInputParameter(paramName, CKPGUID_FLOAT);
                sprintf(paramName, "%d. Wavelength", num);
                beh->CreateInputParameter(paramName, CKPGUID_FLOAT);
                sprintf(paramName, "%d. SinPos", num);
                beh->CreateInputParameter(paramName, CKPGUID_VECTOR);
                currentCount++;
            }
        }
        break;
    }

    return CKBR_OK;
}