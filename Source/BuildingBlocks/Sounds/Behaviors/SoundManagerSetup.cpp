/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SoundManagerSetup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSoundManagerSetupBehaviorProto(CKBehaviorPrototype **pproto);
int SoundManagerSetup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSoundManagerSetupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sound Manager Setup");
    od->SetDescription("Controls the listener properties.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Distance Factor: </SPAN>Virtools Sound System (DirectSound) uses meters as the default unit of distance measurements.
    If your content does not use meters, it can set a distance factor, which is the number of meters
    in a vector unit. If you want to work in feet, set 0.3048 to this value. The distance factor can range from 0 to 10.
    1 is the default.<BR>

    <SPAN CLASS=pin>RollOff Factor: </SPAN>Rolloff is the amount of attenuation that is applied to sounds, based on the listener's distance from
    the sound source. Virtools Sound System (DirectSound) can ignore rolloff, exaggerate it, or give it the same effect as in
    the real world, depending on a variable called the rolloff factor. The rolloff factor can range from 0 to 10.
    A value of 0 means no rolloff is applied to a sound. Every other value represents a multiple of the real-world rolloff.
    1 is the default.<BR>

    <SPAN CLASS=pin>Global Tone: </SPAN>sets the tonal equalization for the sound system, in a range of 0.0 to 1.0, with 1.0 being the highest.<BR>
    <SPAN CLASS=pin>Global gain: </SPAN>sets the global volume of the sound system, in a range of 0.0 to 1.0, with 1.0 being the highest.<BR>

    <SPAN CLASS=pin>Doppler Factor: </SPAN>Virtools Sound System (DirectSound) automatically creates Doppler shift effects for any buffer or listener
    that has a velocity. Effects are cumulative: if the listener and the sound buffer are both moving,
    the system automatically calculates the relative velocity and adjusts the Doppler effect accordingly.
    The Doppler factor can range from 0 to 10. A value of 0 means no Doppler shift is applied to a sound.
    Every other value represents a multiple of the real-world Doppler shift. 1 is the default.<BR>

    */
    od->SetCategory("Sounds/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c05c6a, 0x6454792b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSoundManagerSetupBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSoundManagerSetupBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sound Manager Setup");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Distance Factor", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("RollOff Factor", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Global Tone", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Global Gain", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Doppler Factor", CKPGUID_FLOAT, "1.0");
    //	proto->DeclareInParameter("Priority Bias",CKPGUID_FLOAT,"0.5");

    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SoundManagerSetup);

    *pproto = proto;

    return CK_OK;
}

int SoundManagerSetup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSoundManager *sm = (CKSoundManager *)behcontext.Context->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!sm)
        return CKBR_OK;

    // Set 3D Listener Factors
    CKListenerSettings set;
    CKDWORD states = 0;
    if (!beh->GetInputParameterValue(0, &set.m_DistanceFactor)) states |= CK_LISTENERSETTINGS_DISTANCE;
    //	if(!beh->GetInputParameterValue(1,&set.m_UnitsPerMeter))	states |= CK_LISTENERSETTINGS_UNITS;
    //	if(!beh->GetInputParameterValue(2,&set.m_GlobalEq))			states |= CK_LISTENERSETTINGS_EQ;
    if (!beh->GetInputParameterValue(3, &set.m_GlobalGain)) states |= CK_LISTENERSETTINGS_GAIN;
    if (!beh->GetInputParameterValue(4, &set.m_DopplerFactor)) states |= CK_LISTENERSETTINGS_DOPPLER;
    //	if(!beh->GetInputParameterValue(5,&set.m_PriorityBias))		states |= CK_LISTENERSETTINGS_PRIORITY;
    if (states) sm->UpdateListenerSettings((CK_SOUNDMANAGER_CAPS)states, set);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
