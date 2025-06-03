/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FrequencyControl
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateFrequencyControlBehaviorProto(CKBehaviorPrototype **pproto);
int FrequencyControl(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFrequencyControlDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Pitch Control");
    od->SetDescription("Sets the pitch frequency for a sound, regardless of whether it is playing or not.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Frequency: </SPAN>pitch frequency, in a normal range between 0.5 and 2.0, with 1.0 being the default frequency.<BR>
    */
    od->SetCategory("Sounds/Control");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7b7c731e, 0x175b27a3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFrequencyControlBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

int OldFrequencyControl(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetInputParameterObject(0);
    if (!wave)
        return CKBR_OK;

    float pitch;
    beh->GetInputParameterValue(1, &pitch);

    wave->SetPitch(pitch);

    return CKBR_OK;
}

CKERROR FrequencyControlCallback(const CKBehaviorContext &behcontext)
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
            beh->SetFunction(OldFrequencyControl);
        }
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateFrequencyControlBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Pitch Control");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Frequency", CKPGUID_FLOAT, "1");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FrequencyControl);
    proto->SetBehaviorCallbackFct(FrequencyControlCallback, CKCB_BEHAVIORLOAD);

    *pproto = proto;

    return CK_OK;
}

int FrequencyControl(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    float pitch;
    beh->GetInputParameterValue(0, &pitch);

    wave->SetPitch(pitch);

    return CKBR_OK;
}
