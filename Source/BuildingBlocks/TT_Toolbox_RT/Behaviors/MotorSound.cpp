/////////////////////////////////
/////////////////////////////////
//
//        TT_MotorSound
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "MotorSettings.h"

CKObjectDeclaration *FillBehaviorMotorSoundDecl();
CKERROR CreateMotorSoundProto(CKBehaviorPrototype **pproto);
int MotorSound(const CKBehaviorContext &behcontext);
CKERROR MotorSoundCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorMotorSoundDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_MotorSound");
    od->SetDescription("Plays different samples according to a given vehicle velocity");
    od->SetCategory("TT Toolbox/Sounds");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1a1a67aa, 0x664108ed));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMotorSoundProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);
    return od;
}

CKERROR CreateMotorSoundProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_MotorSound");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Start");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Start Playing");
    proto->DeclareOutput("End Playing");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Speed", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Sound1", CKPGUID_WAVESOUND);
    proto->DeclareInParameter("Sound2", CKPGUID_WAVESOUND);
    proto->DeclareInParameter("Sound3", CKPGUID_WAVESOUND);
    proto->DeclareInParameter("GlobalVolume", CKPGUID_FLOAT);

    proto->DeclareOutParameter("CurrentPitchValue1", CKPGUID_FLOAT);
    proto->DeclareOutParameter("CurrentVolume1", CKPGUID_FLOAT);
    proto->DeclareOutParameter("CurrentPitchValue2", CKPGUID_FLOAT);
    proto->DeclareOutParameter("CurrentVolume2", CKPGUID_FLOAT);
    proto->DeclareOutParameter("CurrentPitchValue3", CKPGUID_FLOAT);
    proto->DeclareOutParameter("CurrentVolume3", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("FirstTime", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MotorSound);

    proto->SetBehaviorCallbackFct(MotorSoundCallBack);

    *pproto = proto;
    return CK_OK;
}

int MotorSound(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float speed = 0.0f;
    float pitch1 = 0.0f, volume1 = 0.0f;
    float pitch2 = 0.0f, volume2 = 0.0f;
    float pitch3 = 0.0f, volume3 = 0.0f;
    int firstTime = 0;

    // Check for Stop input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        CKWaveSound *sound1 = (CKWaveSound *)beh->GetInputParameterObject(1);
        CKWaveSound *sound2 = (CKWaveSound *)beh->GetInputParameterObject(2);
        CKWaveSound *sound3 = (CKWaveSound *)beh->GetInputParameterObject(3);

        if (sound1) sound1->Stop();
        if (sound2) sound2->Stop();
        if (sound3) sound3->Stop();

        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Start/Continue input
    beh->ActivateInput(0, FALSE);
    beh->GetLocalParameterValue(0, &firstTime);
    beh->GetInputParameterValue(0, &speed);
    beh->GetInputParameterValue(4, &g_MotorGlobalVolume);

    CKWaveSound *sound1 = (CKWaveSound *)beh->GetInputParameterObject(1);
    CKWaveSound *sound2 = (CKWaveSound *)beh->GetInputParameterObject(2);
    CKWaveSound *sound3 = (CKWaveSound *)beh->GetInputParameterObject(3);

    if (!sound1 || !sound2 || !sound3)
        return CKBR_PARAMETERERROR;

    // First time initialization
    if (firstTime)
    {
        sound1->SetLoopMode(TRUE);
        sound2->SetLoopMode(TRUE);
        sound3->SetLoopMode(TRUE);
        sound1->SetGain(0.0f);
        sound2->SetGain(0.0f);
        sound3->SetGain(0.0f);
        sound1->Play();
        sound2->Play();
        sound3->Play();
        firstTime = 0;
        beh->SetLocalParameterValue(0, &firstTime);
    }

    // Channel 1: Low speed range
    if (speed >= g_MotorChannel1.vMin && speed < g_MotorChannel1.vMax)
    {
        pitch1 = MotorLerp(speed, g_MotorChannel1.pitchMax, g_MotorChannel1.pitchMin, g_MotorChannel1.vMax, g_MotorChannel1.vMin);
        sound1->SetPitch(pitch1);

        // Calculate volume with fade in/out
        if (speed < g_MotorChannel1.vFadeIn)
            volume1 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel1.vMin, g_MotorChannel1.vFadeIn);
        else if (speed > g_MotorChannel1.vFadeOut)
            volume1 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel1.vMax, g_MotorChannel1.vFadeOut);
        else
            volume1 = 1.0f;

        sound1->SetGain(g_MotorVolume * g_MotorGlobalVolume * volume1);
        if (!sound1->IsPlaying())
            sound1->Play();
    }
    else
    {
        if (sound1->IsPlaying())
            sound1->Stop();
    }

    // Channel 2: Mid speed range
    if (speed >= g_MotorChannel2.vMin && speed < g_MotorChannel2.vMax)
    {
        pitch2 = MotorLerp(speed, g_MotorChannel2.pitchMax, g_MotorChannel2.pitchMin, g_MotorChannel2.vMax, g_MotorChannel2.vMin);
        sound2->SetPitch(pitch2);

        // Calculate volume with fade in/out
        if (speed < g_MotorChannel2.vFadeIn)
            volume2 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel2.vMin, g_MotorChannel2.vFadeIn);
        else if (speed > g_MotorChannel2.vFadeOut)
            volume2 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel2.vMax, g_MotorChannel2.vFadeOut);
        else
            volume2 = 1.0f;

        sound2->SetGain(g_MotorVolume * g_MotorGlobalVolume * volume2);
        if (!sound2->IsPlaying())
            sound2->Play();
    }
    else
    {
        if (sound2->IsPlaying())
            sound2->Stop();
    }

    // Channel 3: High speed range
    if (speed >= g_MotorChannel3.vMin && speed < g_MotorChannel3.vMax)
    {
        pitch3 = MotorLerp(speed, g_MotorChannel3.pitchMax, g_MotorChannel3.pitchMin, g_MotorChannel3.vMax, g_MotorChannel3.vMin);
        sound3->SetPitch(pitch3);

        // Calculate volume with fade in/out
        if (speed < g_MotorChannel3.vFadeIn)
            volume3 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel3.vMin, g_MotorChannel3.vFadeIn);
        else if (speed > g_MotorChannel3.vFadeOut)
            volume3 = MotorLerp(speed, 0.0f, 1.0f, g_MotorChannel3.vMax, g_MotorChannel3.vFadeOut);
        else
            volume3 = 1.0f;

        sound3->SetGain(g_MotorVolume * g_MotorGlobalVolume * volume3);
        if (!sound3->IsPlaying())
            sound3->Play();
    }
    else
    {
        if (sound3->IsPlaying())
            sound3->Stop();
    }

    // Set output parameters
    beh->SetOutputParameterValue(0, &pitch1);
    beh->SetOutputParameterValue(1, &volume1);
    beh->SetOutputParameterValue(2, &pitch2);
    beh->SetOutputParameterValue(3, &volume2);
    beh->SetOutputParameterValue(4, &pitch3);
    beh->SetOutputParameterValue(5, &volume3);

    beh->ActivateOutput(0, TRUE);
    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR MotorSoundCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORRESET:
        {
            int firstTime = 1;
            beh->SetLocalParameterValue(0, &firstTime);
        }
        break;

    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
        {
            CKWaveSound *sound1 = (CKWaveSound *)beh->GetInputParameterObject(1);
            CKWaveSound *sound2 = (CKWaveSound *)beh->GetInputParameterObject(2);
            CKWaveSound *sound3 = (CKWaveSound *)beh->GetInputParameterObject(3);

            if (sound1) sound1->Stop();
            if (sound2) sound2->Stop();
            if (sound3) sound3->Stop();
        }
        break;
    }

    return CKBR_OK;
}