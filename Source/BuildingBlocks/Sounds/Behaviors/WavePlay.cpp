/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            WavePlay
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateWavePlayBehaviorProto(CKBehaviorPrototype **pproto);
int WavePlay(const CKBehaviorContext &behcontext);
CKERROR WavePlayCB(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWavePlayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Wave Player");
    od->SetDescription("Plays a Wave Sound File, with possible fade in and/or fade out; the fades values are done in seconds;");
    /* rem:
    <SPAN CLASS=in>Play : </SPAN>Starts the Playing a Wave Sound.<BR>
    <SPAN CLASS=in>Stop : </SPAN>Stops the Playing a Wave Sound.<BR>
    <SPAN CLASS=in>Pause/Resume : </SPAN>Pauses/Resumes the Wave Sound.<BR>
    <BR>
    <SPAN CLASS=out>Start Playing : </SPAN>is activated when the sound start playing.<BR>
    <SPAN CLASS=out>Stop Playing : </SPAN>is activated when the sound stop playing.<BR>
    <SPAN CLASS=out>Paused/Resumed : </SPAN>is activated when the sound is paused or resumed.<BR>
    <BR>
    <SPAN CLASS=pin>Fade In : </SPAN>fade in time in milliseconds.<BR>
    <SPAN CLASS=pin>Fade Out : </SPAN>fade out time in milliseconds.<BR>
    <SPAN CLASS=pin>Loop : </SPAN>loops the sound if checked
    <BR>
    Fade in is used only when we get by 'play', and fade out is used only when we get by 'stop'
    */
    od->SetCategory("Sounds/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5bde0e45, 0x2e2107d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWavePlayBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

int OldWavePlay(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get Sound
    CKWaveSound *wave = (CKWaveSound *)beh->GetInputParameterObject(0);
    if (!wave)
        return CKBR_PARAMETERERROR;

    if (beh->IsInputActive(0))
    { // Play
        beh->ActivateInput(0, FALSE);

        // Get fade in
        float fadein = 0;
        beh->GetInputParameterValue(1, &fadein);

        // loop
        CKBOOL loop = FALSE;
        beh->GetInputParameterValue(3, &loop);
        wave->SetLoopMode(loop);

        wave->Play(fadein);
    }
    else
    {
        if (beh->IsInputActive(1))
        { // Stop
            beh->ActivateInput(1, FALSE);

            // Get fade out
            float fadeout = 0;
            beh->GetInputParameterValue(2, &fadeout);

            wave->Stop(fadeout);
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR WavePlayCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        // Is this the old version
        CKParameterIn *pin = beh->GetInputParameter(0);
        if (pin && (pin->GetGUID() == CKPGUID_WAVESOUND))
        { // Old version
            beh->SetFunction(OldWavePlay);
        }
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        CKParameterIn *pin = beh->GetInputParameter(0);
        if (pin && (pin->GetGUID() == CKPGUID_WAVESOUND))
        { // Old version
            CKWaveSound *wave = (CKWaveSound *)beh->GetInputParameterObject(0);
            if (!wave)
                return CKBR_PARAMETERERROR;
            wave->Pause();
        }
        else
        { // new version
            CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
            if (!wave)
                return CKBR_PARAMETERERROR;
            wave->Pause();
        }
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateWavePlayBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Wave Player");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Play");
    proto->DeclareInput("Stop");
    proto->DeclareInput("Pause/Resume");

    proto->DeclareOutput("Start Playing");
    proto->DeclareOutput("End Playing");
    proto->DeclareOutput("Paused/Resumed");

    proto->DeclareInParameter("Fade In", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Fade Out", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Loop", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(WavePlayCallback);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WavePlay);

    *pproto = proto;

    return CK_OK;
}

int WavePlay(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get Sound
    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_PARAMETERERROR;

    if (beh->IsInputActive(0))
    { // Play
        beh->ActivateInput(0, FALSE);

        // Get fade in
        float fadein = 0;
        beh->GetInputParameterValue(0, &fadein);

        // loop
        CKBOOL loop = FALSE;
        beh->GetInputParameterValue(2, &loop);
        wave->SetLoopMode(loop);

        wave->Play(fadein, wave->GetGain());

        beh->ActivateOutput(0);

        // If we ask the sound to loop, no way it stops one day
        if (loop)
            return CKBR_OK;

        return CKBR_ACTIVATENEXTFRAME;
    }

    if (beh->GetInputCount() > 2)
        if (beh->IsInputActive(2))
        {
            beh->ActivateInput(2, FALSE);
            beh->ActivateOutput(2);

            if (wave->IsPlaying())
            {
                wave->Pause();
                return CKBR_OK;
            }

            if (wave->IsPaused())
            {
                wave->Resume();
                return CKBR_ACTIVATENEXTFRAME;
            }
        }

    if (beh->IsInputActive(1))
    { // Stop
        beh->ActivateInput(1, FALSE);

        // Get fade out
        float fadeout = 0;
        beh->GetInputParameterValue(1, &fadeout);

        wave->Stop(fadeout);
        if (fadeout == 0.0f)
        {
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        return CKBR_ACTIVATENEXTFRAME;
    }

    // we're likely being playing the sound
    if (!wave->IsPlaying())
    { // if the sound is over, we activate the output
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
