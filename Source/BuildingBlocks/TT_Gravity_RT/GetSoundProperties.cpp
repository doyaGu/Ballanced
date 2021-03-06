/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT GetSoundProperties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorGetSoundPropertiesDecl();
CKERROR CreateGetSoundPropertiesProto(CKBehaviorPrototype **);
int GetSoundProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetSoundPropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetSoundProperties");
    od->SetDescription("Get Gain, Pitch and Pan.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30FA6A70, 0x3B3F728A));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Mirco Nierenz");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetSoundPropertiesProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    return od;
}

CKERROR CreateGetSoundPropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetSoundProperties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Gain", CKPGUID_FLOAT, "0.0f");
    proto->DeclareOutParameter("Pitch", CKPGUID_FLOAT, "0.0f");
    proto->DeclareOutParameter("Pan", CKPGUID_FLOAT, "0.0f");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetSoundProperties);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetSoundProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKWaveSound *sound = (CKWaveSound *)beh->GetTarget();

    float gain = sound->GetGain();
    float pitch = sound->GetPitch();
    float pan = sound->GetPan();

    beh->SetOutputParameterValue(0, &gain);
    beh->SetOutputParameterValue(1, &pitch);
    beh->SetOutputParameterValue(2, &pan);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}