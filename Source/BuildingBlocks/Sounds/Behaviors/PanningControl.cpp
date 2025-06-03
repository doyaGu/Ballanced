/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PanningControl
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreatePanningControlBehaviorProto(CKBehaviorPrototype **pproto);
int PanningControl(const CKBehaviorContext& behcontext);

CKObjectDeclaration	*FillBehaviorPanningControlDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Panning Control");
    od->SetDescription("Sets the panning for a sound, regardless of whether it is playing or not.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>range is between -100 (left) and 100 (right), with 0 (zero) being the center.<BR> 
    */
    od->SetCategory("Sounds/Control");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6a5f17af,0x63480864));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePanningControlBehaviorProto);
    od->SetCompatibleClassId(CKCID_WAVESOUND);
    od->NeedManager(SOUND_MANAGER_GUID);
    
    return od;
}


int OldPanningControl(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;

  beh->ActivateInput(0,FALSE);
    beh->ActivateOutput(0);

    CKWaveSound* wave=(CKWaveSound*)beh->GetInputParameterObject(0);
    if( !wave) return CKBR_OK;
    
    float Panning;
    beh->GetInputParameterValue(1,&Panning);

    wave->SetPan(Panning);
    
    return CKBR_OK;
}

CKERROR PanningControlCallback(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;

    switch(behcontext.CallbackMessage) {
    case CKM_BEHAVIORLOAD:
        {
            // Is this the old version
            CKParameterIn* pin = beh->GetInputParameter(0);
            if(pin && (pin->GetGUID() == CKPGUID_WAVESOUND)) { // Old version
                beh->SetFunction(OldPanningControl);
            }
        }
        break;
    }
  return CKBR_OK;
}


CKERROR CreatePanningControlBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Panning Control");
    if(!proto) return CKERR_OUTOFMEMORY;
    
    proto->DeclareInput("In");
    proto->DeclareOutput("Out");
    
    proto->DeclareInParameter("Position",CKPGUID_PERCENTAGE,"0");
    
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PanningControl);
    proto->SetBehaviorCallbackFct(PanningControlCallback,CKCB_BEHAVIORLOAD);
    
    *pproto = proto;
    
    return CK_OK;
}


int PanningControl(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;

    beh->ActivateInput(0,FALSE);
    beh->ActivateOutput(0);

    CKWaveSound* wave=(CKWaveSound*)beh->GetTarget();
    if( !wave) return CKBR_OK;
    
    float Panning=0;
    beh->GetInputParameterValue(0,&Panning);

    wave->SetPan(Panning);
    
    return CKBR_OK;
}
