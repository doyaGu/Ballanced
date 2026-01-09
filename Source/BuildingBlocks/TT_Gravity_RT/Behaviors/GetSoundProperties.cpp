/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT GetSoundProperties
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorGetSoundPropertiesDecl();
CKERROR CreateGetSoundPropertiesProto(CKBehaviorPrototype **pproto);
int GetSoundProperties(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetSoundPropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_GetSoundProperties");
    od->SetDescription("Get Gain, Pitch and Pan.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30fa6a70, 0x3b3f728a));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetSoundPropertiesProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    return od;
}

CKERROR CreateGetSoundPropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_GetSoundProperties");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Gain", CKPGUID_FLOAT, "0.0f");
    proto->DeclareOutParameter("Pitch", CKPGUID_FLOAT, "0.0f");
    proto->DeclareOutParameter("Pan", CKPGUID_FLOAT, "0.0f");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetSoundProperties);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetSoundProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKWaveSound *sound = (CKWaveSound *)beh->GetTarget();
    if (!sound)
        return CKBR_PARAMETERERROR;

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
