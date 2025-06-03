/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlaySoundInstance
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreatePlaySoundInstanceBehaviorProto(CKBehaviorPrototype **pproto);
int DoPlaySoundInstance(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPlaySoundInstanceDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Play Sound Instance");
    od->SetDescription("Plays the totality of a sound once, optionally attaching it to an object.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>2D: </SPAN>is the sound is to be played in 2D. If so, the subsequent input will be of no use.<BR>
    <SPAN CLASS=pin>Object: </SPAN>object to which the sound is to be attached.<BR>
    <SPAN CLASS=pin>Position: </SPAN>position of the sound, in the referential of the attached object, or in the world.<BR>
    <SPAN CLASS=pin>Direction: </SPAN>Not Yet Implemented.<BR>
    <SPAN CLASS=pin>Min Delay(ms): </SPAN>Minimum delay between twice the same sound.<BR>
    <SPAN CLASS=pin>Volume: </SPAN>Controls the volume. 100% means full volume.<BR>
    <BR>
    This behavior allows a new instance of the same sound to be played whilst the original
    sound is still playing, making the two sounds overlap.<BR>
    This building block is typically used for repeated sounds, such as an echo or gun shots.<BR>
    */
    /* warning:
    The sound to be played must not be streamed.<BR>
    */
    od->SetCategory("Sounds/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x283a35, 0x38ef6b48));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlaySoundInstanceBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreatePlaySoundInstanceBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Play Sound Instance");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("2D", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Min Delay", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Volume", CKPGUID_FLOAT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DoPlaySoundInstance);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int DoPlaySoundInstance(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    CKBOOL mode = FALSE;
    beh->GetInputParameterValue(0, &mode);

    // get object to attach sound to
    CK3dEntity *object = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Position
    VxVector pos(0, 0, 0);
    beh->GetInputParameterValue(2, &pos);

    // Direction (Not Yet Implemented)
    VxVector dir(0, 0, 1);
    beh->GetInputParameterValue(3, &dir);

    // Minimum Delay between twice the same sound
    float minDelay = 0.0f;
    beh->GetInputParameterValue(4, &minDelay);

    //--- Play Sound Instance
    CKSOUNDHANDLE shnd = wave->PlayMinion(mode, object, &pos, &dir, minDelay);

    CKSoundManager *sm = (CKSoundManager *)behcontext.Context->GetManagerByGuid(SOUND_MANAGER_GUID);
    if (!sm)
        return CKBR_OK;

    // get volume
    float gain = 1.0f;
    beh->GetInputParameterValue(5, &gain);

    // set sound volume
    CKWaveSoundSettings setting;
    setting.m_Gain = gain;
    sm->UpdateSettings(shnd, CK_WAVESOUND_SETTINGS_GAIN, setting);

    return CKBR_OK;
}
