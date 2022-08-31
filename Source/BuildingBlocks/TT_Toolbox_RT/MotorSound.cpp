/////////////////////////////////
/////////////////////////////////
//
//        TT_MotorSound
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTMotorSoundBehaviorProto(CKBehaviorPrototype **pproto);
int TTMotorSound(const CKBehaviorContext& behcontext);
CKERROR TTMotorSoundCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTMotorSoundDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_MotorSound");
    od->SetDescription("plays different samples according to a given vehicle velocity");
    od->SetCategory("TT Toolbox/Sounds");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1a1a67aa,0x664108ed));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTMotorSoundBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTMotorSoundBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_MotorSound");
    if(!proto) return CKERR_OUTOFMEMORY;

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
    proto->SetFunction(TTMotorSound);

    proto->SetBehaviorCallbackFct(TTMotorSoundCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTMotorSound(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTMotorSoundCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}