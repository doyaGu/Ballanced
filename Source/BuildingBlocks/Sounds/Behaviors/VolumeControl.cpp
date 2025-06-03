/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            VolumeControl
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateVolumeControlBehaviorProto(CKBehaviorPrototype **pproto);
int VolumeControl(const CKBehaviorContext &behcontext);
int OldVolumeControl(const CKBehaviorContext &behcontext);
CKERROR VolumeControlCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorVolumeControlDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Volume Control");
    od->SetDescription("Controls the volume of a sound, either it is playing or not.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Wave Sound: </SPAN>wave sound to control.<BR>
    <SPAN CLASS=pin>Volume: </SPAN>controls the volume. 100% means full volume.<BR>
    <BR>
    */
    od->SetCategory("Sounds/Control");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x33613f21, 0x776b185b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateVolumeControlBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateVolumeControlBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Volume Control");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Volume", CKPGUID_FLOAT, "1");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorCallbackFct(VolumeControlCallback, CKCB_BEHAVIORLOAD);
    proto->SetFunction(VolumeControl);

    *pproto = proto;

    return CK_OK;
}

CKERROR VolumeControlCallback(const CKBehaviorContext &behcontext)
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
            beh->SetFunction(OldVolumeControl);
        }
    }
    break;
    }
    return CKBR_OK;
}

int OldVolumeControl(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetInputParameterObject(0);
    if (!wave)
        return CKBR_OK;

    float volume = 1.0f;
    beh->GetInputParameterValue(1, &volume);

    wave->SetGain(volume);

    return CKBR_OK;
}

int VolumeControl(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    float volume = 1.0f;
    beh->GetInputParameterValue(0, &volume);

    wave->SetGain(volume);

    return CKBR_OK;
}
