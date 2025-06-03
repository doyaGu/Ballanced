/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetPriority
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateSetPriorityBehaviorProto(CKBehaviorPrototype **pproto);
int SetPriority(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPriorityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Priority");
    od->SetDescription("Controls the Priority of a sound, either it is playing or not.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>activates the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Priority : </SPAN>Controls the Priority. [0.0 - 1.0] with 1.0 being the higher priority.<BR>
    */
    od->SetCategory("Sounds/Properties");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x159630e5, 0x5a5a26c6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPriorityBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

CKERROR CreateSetPriorityBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Priority");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Priority", CKPGUID_FLOAT, "0.5");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetPriority);

    *pproto = proto;

    return CK_OK;
}

int SetPriority(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKWaveSound *wave = (CKWaveSound *)beh->GetTarget();
    if (!wave)
        return CKBR_OK;

    float Priority;
    beh->GetInputParameterValue(0, &Priority);

    wave->SetPriority(Priority);

    return CKBR_OK;
}
