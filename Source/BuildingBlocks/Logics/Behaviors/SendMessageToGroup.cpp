/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         Send Message To Group
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSendMessageToGroupDecl();
CKERROR CreateSendMessageToGroupProto(CKBehaviorPrototype **);
int SendMessageToGroup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSendMessageToGroupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Send Message To Group");
    od->SetDescription("Sends a Message to each Object of a Group.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message to send.<BR>
    <SPAN CLASS=pin>Group: </SPAN>group of objects to send the message to.<BR>
    <BR>
    You can bind data with your message, by adding Input Parameters to the SendMessage Behavior.
    These data will be retrieved by using the GetDataMessage behavior.<BR>
    <BR>
    Useful to trigger building blocks on all the group's objects, when some special event occurs on a different script than the current one. <BR>
    <BR>
    See also: Broadcast Message, Get Message Data, Send Message, Wait Message.<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5f906952, 0x6df11649));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSendMessageToGroupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSendMessageToGroupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Send Message To Group");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");
    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_TARGETABLE));
    proto->SetFunction(SendMessageToGroup);

    *pproto = proto;
    return CK_OK;
}

int SendMessageToGroup(const CKBehaviorContext &behcontext)
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

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(1);
    if (!group)
        return CKBR_PARAMETERERROR;

    CKMessage *msg = mm->SendMessageGroup(Msg, group, sender);
    if (!msg)
        throw "Message wasn't send normally";

    CKParameterIn *tpin;

    int startindex = 2;
    if (beh->GetVersion() < 0x00020000)
        startindex = 1;

    int count = beh->GetInputParameterCount();
    for (int i = startindex; i < count; i++)
    {
        tpin = (CKParameterIn *)beh->GetInputParameter(i);
        if (tpin)
            msg->AddParameter(tpin->GetRealSource());
    }

    return CKBR_OK;
}
