/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetRange
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSetRangeBehaviorProto(CKBehaviorPrototype **pproto);
int SetRange(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRangeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Sound Range");
    od->SetDescription("Control the range of the 3D sound.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Min Distance: </SPAN>Distance from the viewer below which the sound will be at full volume.<BR>
    <SPAN CLASS=pin>Max Distance: </SPAN>Distance from the viewer after which the sound will be at zero volume.<BR>
    <SPAN CLASS=pin>Distance Model Scale: </SPAN>multiplier for the sound attenuation.<BR>
    */
    od->SetCategory("Sounds/3D Properties");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x27f50a7c, 0xee91d0e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRangeBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSetRangeBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Sound Range");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Min distance", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Max distance", CKPGUID_FLOAT, "100.0");
    // proto->DeclareInParameter("Distance Model Scale",CKPGUID_FLOAT,"1.0");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetRange);

    *pproto = proto;

    return CK_OK;
}

int SetRange(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    float mind = 1.0f, maxd = 100.0f;
    beh->GetInputParameterValue(0, &mind);
    beh->GetInputParameterValue(1, &maxd);

    wave->SetMinMaxDistance(mind, maxd);

    float scale = 1.0f;
    beh->GetInputParameterValue(2, &scale);

    // wave->SetDistanceFactor(scale);

    return CKBR_OK;
}
