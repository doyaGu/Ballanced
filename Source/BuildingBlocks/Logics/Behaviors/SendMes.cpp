/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Send Message
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSendMessageDecl();
CKERROR CreateSendMessageProto(CKBehaviorPrototype **);
int SendMessage(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSendMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Send Message");
    od->SetDescription("Sends a Message to a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message to send. You can create your own message (enter any string you want)<BR>
    <SPAN CLASS=pin>Dest: </SPAN>3D Entity to send the message to.<BR>
    <BR>
    You can bind data with your message, by adding Input Parameters to the "Send Message" building block.<BR>
    These data will be retrieved by using the "Get Message Data" building block.<BR>
    <BR>
    Useful to trigger building blocks when some special event occurs on a different script than the current one.<BR>
    <BR>
    See also: Broadcast Message, Get Message Data, Send Message To Group, Wait Message.<BR>
    */
    /* warning:
    - Message are received one behavioral frame after being sent.<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xa20e8d5b, 0xdf002150));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSendMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSendMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Send Message");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");
    proto->DeclareInParameter("Dest", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_TARGETABLE));
    proto->SetFunction(SendMessage);

    *pproto = proto;
    return CK_OK;
}

int SendMessage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKMessageManager *mm = behcontext.MessageManager;

    CKBeObject *sender = (CKBeObject *)beh->GetTarget();
    if (!sender)
        return CKBR_OWNERERROR;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    int Msg = -1;
    beh->GetInputParameterValue(0, &Msg);

    CKBeObject *beo_dest = (CKBeObject *)beh->GetInputParameterObject(1);

    CKMessage *msg = mm->SendMessageSingle(Msg, beo_dest, sender);
    /// if(!msg) throw "Message wasn't send normally";
    if (!msg)
        return CKBR_OK;

    int count = beh->GetInputParameterCount();
    CKParameterIn *tpin;
    for (int i = 2; i < count; i++)
    {
        tpin = (CKParameterIn *)beh->GetInputParameter(i);
        if (tpin)
            msg->AddParameter(tpin->GetRealSource());
    }

    return CKBR_OK;
}
