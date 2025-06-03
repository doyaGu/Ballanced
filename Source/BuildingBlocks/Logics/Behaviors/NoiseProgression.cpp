/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Noise Progression
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "perlin.h"

CKObjectDeclaration *FillBehaviorNoiseProgressionDecl();
CKERROR CreateNoiseProgressionProto(CKBehaviorPrototype **);
int NoiseProgression(const CKBehaviorContext &behcontext);
int NoiseProgressionTimeBased(const CKBehaviorContext &behcontext);
CKERROR NoiseProgressionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorNoiseProgressionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Noise Progression");
    od->SetDescription("Interpolates a float according to a Perlin noise function in the [Min,Max] range, in a given number of milliseconds (or frames).");
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x9424ff80, 0x9751bc39));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateNoiseProgressionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateNoiseProgressionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Noise Progression");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Time", CKPGUID_TIME, "0m 3s 0ms");
    proto->DeclareInParameter("A", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("B", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Alpha", CKPGUID_FLOAT, "2.0");
    proto->DeclareInParameter("Beta", CKPGUID_FLOAT, "2.0");
    proto->DeclareInParameter("Harmonics", CKPGUID_INT, "4");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareOutParameter("Value", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Delta", CKPGUID_FLOAT, "0.0");
    proto->DeclareOutParameter("Progression", CKPGUID_PERCENTAGE, "0");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(NoiseProgressionTimeBased);
    proto->SetBehaviorCallbackFct(NoiseProgressionCallBack);

    *pproto = proto;
    return CK_OK;
}

enum
{
    PIN_TIME,
    PIN_A,
    PIN_B,
    PIN_ALPHA,
    PIN_BETA,
    PIN_HARMONICS
};

/*******************************************************/
/*     Noise Progression Not Time Based               */
/*******************************************************/
int NoiseProgression(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int maxcount = 0;
    int current = 0;

    float a = 0.0f;
    beh->GetInputParameterValue(1, &a);

    float b = 1.0f;
    beh->GetInputParameterValue(2, &b);

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        // we initialize the old value
        beh->SetOutputParameterValue(1, &a);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);
    }

    float alpha = 2.0f;
    beh->GetInputParameterValue(PIN_ALPHA, &alpha);

    float beta = 2.0f;
    beh->GetInputParameterValue(PIN_BETA, &beta);

    int harmonics = 4;
    beh->GetInputParameterValue(PIN_HARMONICS, &harmonics);

    ++current;
    beh->SetOutputParameterValue(0, &current);

    beh->GetInputParameterValue(0, &maxcount);

    float oldPos;
    beh->GetOutputParameterValue(1, &oldPos);

    float Pos = oldPos;
    float progression = (float)current / maxcount;

    if (current > maxcount)
        beh->ActivateOutput(0);
    else
    { // not yet finish...
        // the noise call
        float value = 0.5f + 0.5f * PerlinNoise1D(progression, alpha, beta, harmonics);

        Pos = a + (b - a) * value;

        beh->ActivateOutput(1);
    }

    oldPos = Pos - oldPos;

    beh->SetOutputParameterValue(1, &Pos);
    beh->SetOutputParameterValue(2, &oldPos);
    beh->SetOutputParameterValue(3, &progression);

    return CKBR_OK;
}

/*******************************************************/
/* Noise Progression (TIME Based new version (float)) */
/*******************************************************/
int NoiseProgressionTimeBased(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float maxcount = 0.0f;
    float current = 0.0f;

    float a = 0.0f;
    beh->GetInputParameterValue(1, &a);

    float b = 1.0f;
    beh->GetInputParameterValue(2, &b);

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        // we initialize the old value
        beh->SetOutputParameterValue(1, &a);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);
    }

    float alpha = 2.0f;
    beh->GetInputParameterValue(PIN_ALPHA, &alpha);

    float beta = 2.0f;
    beh->GetInputParameterValue(PIN_BETA, &beta);

    int harmonics = 4;
    beh->GetInputParameterValue(PIN_HARMONICS, &harmonics);

    current += behcontext.DeltaTime;
    beh->SetOutputParameterValue(0, &current);

    beh->GetInputParameterValue(0, &maxcount);

    float oldPos;
    beh->GetOutputParameterValue(1, &oldPos);

    float Pos = oldPos;

    float progression = current / maxcount;

    if (current > maxcount)
        beh->ActivateOutput(0);
    else // not yet finish...
    {
        // the noise call
        float value = 0.5f + 0.5f * PerlinNoise1D(progression, alpha, beta, harmonics);

        Pos = a + (b - a) * value;

        beh->ActivateOutput(1);
    }

    oldPos = Pos - oldPos;

    beh->SetOutputParameterValue(1, &Pos);
    beh->SetOutputParameterValue(2, &oldPos);
    beh->SetOutputParameterValue(3, &progression);

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR NoiseProgressionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBOOL time = FALSE;
        beh->GetLocalParameterValue(0, &time);

        CKParameterIn *pin = beh->GetInputParameter(0);
        CKParameterOut *pout = beh->GetOutputParameter(0);

        CKGUID currentGuid = pin->GetGUID();

        // Time based or not ?
        if (time)
        {
            beh->SetFunction(NoiseProgressionTimeBased);
            pin->SetGUID(CKPGUID_TIME, TRUE, "Duration");
            if (pout)
            {
                pout->SetName("Elapsed Time");
                pout->SetGUID(CKPGUID_TIME);
            }
        }
        else
        {
            beh->SetFunction(NoiseProgression);
            pin->SetGUID(CKPGUID_INT, TRUE, "Number of Frame");
            if (pout)
            {
                pout->SetName("Current Frame");
                pout->SetGUID(CKPGUID_INT);
            }
        }
    }
    }

    return CKBR_OK;
}
