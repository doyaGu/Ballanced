/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetRenderMode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSetRenderModeBehaviorProto(CKBehaviorPrototype **pproto);
int SetRenderMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRenderModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Sound Type");
    od->SetDescription("Tells if a sound is to be render in background or in 3d space.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>RenderMode: </SPAN>sets the sound type:<BR>
    - Background: the sound will be rendered wherever you are, and wherever you look.<BR>
    - Point: the sound will be rendered as if it came from a 3D position.<BR>
    <BR>
    */
    od->SetCategory("Sounds/Properties");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7734470d, 0x23b221f8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRenderModeBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSetRenderModeBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Sound Type");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Sound Type", CKGUID(0x413648dd, 0x47515700), "Point");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetRenderMode);

    *pproto = proto;

    return CK_OK;
}

int SetRenderMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    CKDWORD RenderMode;
    beh->GetInputParameterValue(0, &RenderMode);

    wave->SetType((CK_WAVESOUND_TYPE)RenderMode);

    return CKBR_OK;
}
