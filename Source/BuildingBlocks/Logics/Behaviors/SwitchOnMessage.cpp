/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SwitchOnMessage
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSwitchOnMessageDecl();
CKERROR CreateSwitchOnMessageProto(CKBehaviorPrototype **);
int SwitchOnMessage(const CKBehaviorContext &behcontext);

CKERROR SwitchOnMessageCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorSwitchOnMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Switch On Message");
    od->SetDescription("Activates the appropriate output when receiving a message.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the behavior.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is activated when Message 0 is received.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated when Message 1 is received.<BR>
    <SPAN CLASS=in>...</SPAN>etc.<BR>
    <BR>
    <SPAN CLASS=pin>Message 0: </SPAN>first awaited message.<BR>
    <SPAN CLASS=pin>Message 1: </SPAN>second awaited message.<BR>
    <SPAN CLASS=pin>...</SPAN>etc.<BR>
    <BR>
    You can add as many 'Outputs' as you want, the 'Input Parameters' will be added automatically.<BR>
    This behavior doesn't need to loop (it'll wait message until it is deactivated by the 'Off' input).<BR>
    <BR>
    See also : WaitMessage.<BR>
    */
    /* warning:
    - The building block remains active after a message has been received.<BR>
    If you want to deactivated it, you should enter by <FONT COLOR=#a03030>Off</FONT>.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1bb23f1d, 0x17ff14b9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchOnMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Message");
    return od;
}

CKERROR CreateSwitchOnMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Switch On Message");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Received 0");
    proto->DeclareOutput("Received 1");

    proto->DeclareInParameter("Message 0", CKPGUID_MESSAGE);
    proto->DeclareInParameter("Message 1", CKPGUID_MESSAGE);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGERECEIVER | CKBEHAVIOR_VARIABLEOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchOnMessage);

    proto->SetBehaviorCallbackFct(SwitchOnMessageCallBack);

    *pproto = proto;
    return CK_OK;
}

int SwitchOnMessage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetOwner();

    int count_param_in = beh->GetInputParameterCount();

    if (beh->IsInputActive(1)) // We get by 'OFF'
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
        beh->ActivateInput(0, FALSE);

    int count_message = beo->GetLastFrameMessageCount();
    if (count_message)
    {
        int a, i, MsgType;
        CKMessage *message;
        for (i = 0; i < count_param_in; i++) // For each message to look for ( in the 'pin's )
        {
            beh->GetInputParameterValue(i, &MsgType);
            for (a = 0; a < count_message; a++) // For each message retrieved this frame (send to our 'beo')
            {

                message = beo->GetLastFrameMessage(a);
                if (MsgType == message->GetMsgType())
                    beh->ActivateOutput(i);
            }
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SwitchOnMessageCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    {
        CKBeObject *beo = (CKBeObject *)beh->GetOwner();
        if (beo)
            beo->SetAsWaitingForMessages(TRUE);
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        CKBeObject *beo = (CKBeObject *)beh->GetOwner();
        if (beo)
            beo->SetAsWaitingForMessages(FALSE);
    }
    break;
    case CKM_BEHAVIOREDITED:
    {
        int c_out = beh->GetOutputCount();
        int c_pin = beh->GetInputParameterCount();

        char pin_str[10];

        while (c_pin < c_out)
        { // we must add 'Input Param'
            sprintf(pin_str, "Message %d", c_pin);
            beh->CreateInputParameter(pin_str, CKPGUID_MESSAGE);
            ++c_pin;
        }

        while (c_pin > c_out)
        { // we must remove 'Input Param'
            --c_pin;
            CKDestroyObject(beh->RemoveInputParameter(c_pin));
        }
    }
    break;
    }

    return CKBR_OK;
}
