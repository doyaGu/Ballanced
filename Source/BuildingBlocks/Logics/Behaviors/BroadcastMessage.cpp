/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Broadcast Message
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBroadcastMessageDecl();
CKERROR CreateBroadcastMessageProto(CKBehaviorPrototype **);
int BroadcastMessage(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBroadcastMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Broadcast Message");
    od->SetDescription("Sends a Message to every objects waiting for it.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message to broadcast.<BR>
    <BR>
    You can bind data with your message simply by adding Input Parameters to the BroadcastMessage building block.
    These data can then be retrieved by using a "GetDataMessage" building block with the "WaitMessage" building block .<BR>
    If an object from another scene was waiting for the message with a "Wait For Message" building block,
    the output of this building block will be activated when launching its scene (with no reset).
    <BR>
    See also : WaitMessage, GetDataMessage.<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d6c4ae1, 0x72ae2cd6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateBroadcastMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBroadcastMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Broadcast Message");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");
    proto->DeclareInParameter("Type", CKPGUID_CLASSID, "Behavioral Object");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BroadcastMessage);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_TARGETABLE));

    *pproto = proto;
    return CK_OK;
}

int BroadcastMessage(const CKBehaviorContext &behcontext)
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

    int pinOffset = 1;
    int type = CKCID_BEOBJECT; // Type

    if (beh->GetVersion() > 0x00010000)
    {
        beh->GetInputParameterValue(1, &type);
        pinOffset = 2;
    }

    CKMessage *msg = mm->SendMessageBroadcast(Msg, type, sender);
    if (!msg)
        throw "Message wasn't send normally";

    CKParameterIn *tpin;
    for (int i = pinOffset; i < beh->GetInputParameterCount(); ++i)
    {
        tpin = (CKParameterIn *)beh->GetInputParameter(i);
        if (tpin)
            msg->AddParameter(tpin->GetRealSource());
    }

    return CKBR_OK;
}
