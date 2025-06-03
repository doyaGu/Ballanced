/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TriggerEvent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTriggerEventDecl();
CKERROR CreateTriggerEventProto(CKBehaviorPrototype **);
int TriggerEvent(const CKBehaviorContext &behcontext);

// Warning: this callback is used by other building blocks
CKERROR TriggerEventCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorTriggerEventDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Trigger Event");
    od->SetDescription("activates 'Activated' output when 'In' begins to be activated / activates 'Deactivated' output when 'In' stops being activated.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Activated: </SPAN>is activated at the activation of the "In" input.<BR>
    Since there, the building block will remain actif until "In" is deactivated, but the "Activated" output is activated only one time.<BR>
    <SPAN CLASS=out>Deactivated: </SPAN>is activated when the building block stops beeing activated by "In".<BR>
    <BR>
    This building block activates 'Activated' output when 'In' begins to be activated / activates 'Deactivated' output when 'In' stops being activated.<BR>
    */
    /* warning:
    - This building block loops internally, therefore it is activated automatically each frame.
    Beware to choose a proper priority !<BR>
    eg: if you use a "Swicth On Key" building block to activate each frame the "Trigger Event", give the "Trigger Event" a smaller priority than the one you give to the "Switch On Key".<BR>
    This is because there's no specific order in which the building block are called if they must loop internally.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3c3f7044, 0xe917d1a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00000005);
    od->SetCreationFunction(CreateTriggerEventProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateTriggerEventProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Trigger Event");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Activate");
    proto->DeclareOutput("Deactivate");

    proto->DeclareLocalParameter(NULL, CKPGUID_BOOL, "FALSE"); // old state

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TriggerEvent);
    proto->SetBehaviorCallbackFct(TriggerEventCallBack);

    *pproto = proto;
    return CK_OK;
}

int TriggerEvent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKBOOL old_state = FALSE;
    beh->GetLocalParameterValue(0, &old_state);

    if (beh->IsInputActive(0))
    {
        if (!old_state)
        {
            beh->ActivateOutput(0);
            old_state = TRUE;
            beh->SetLocalParameterValue(0, &old_state);
        }
        beh->ActivateInput(0, FALSE);
        return CKBR_ACTIVATENEXTFRAME;
    }
    else
    {
        beh->ActivateOutput(1);
        old_state = FALSE;
        beh->SetLocalParameterValue(0, &old_state);
        return CKBR_OK;
    }
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
// Warning: this callback is used by other building blocks
CKERROR TriggerEventCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORRESET:
    { // this event occures also when the script is activated whith a reset flag.
        CKBOOL state = FALSE;
        beh->SetLocalParameterValue(0, &state);
    }
    break;
    }

    return CKBR_OK;
}
