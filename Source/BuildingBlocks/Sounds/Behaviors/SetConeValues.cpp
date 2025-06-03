/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetCone
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSetConeBehaviorProto(CKBehaviorPrototype **pproto);
int SetCone(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetConeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Sound Cone Values");
    od->SetDescription("Control the cone shape of the 3D sound.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Inside Angle: </SPAN>This is the angle within which the sound is at its normal volume.<BR>
    <SPAN CLASS=pin>OutSide Angle: </SPAN>This is the angle outside of which the sound is at its outside volume..<BR>
    <SPAN CLASS=pin>Out of Cone Volume: </SPAN>Cone outside volume for this sound.<BR>
    */
    od->SetCategory("Sounds/3D Properties");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xabf60a8c, 0xff91d0d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetConeBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSetConeBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Sound Cone Values");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Inside Angle", CKPGUID_ANGLE, "15.0");
    proto->DeclareInParameter("Outside Angle", CKPGUID_ANGLE, "30.0");
    proto->DeclareInParameter("Outside Cone Volume", CKPGUID_FLOAT, "0.0");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCone);

    *pproto = proto;

    return CK_OK;
}

int SetCone(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    float mina, maxa, outvolume;
    wave->GetCone(&mina, &maxa, &outvolume);
    beh->GetInputParameterValue(0, &mina);
    beh->GetInputParameterValue(1, &maxa);
    beh->GetInputParameterValue(2, &outvolume);
    wave->SetCone(mina * 180.0f / PI, maxa * 180.0f / PI, outvolume);

    return CKBR_OK;
}
