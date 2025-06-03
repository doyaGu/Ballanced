/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CheckForMessage
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCheckForMessageDecl();
CKERROR CreateCheckForMessageProto(CKBehaviorPrototype **);
int CheckForMessage(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCheckForMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Check For Message");
    od->SetDescription("Check the message manager to see if a specific message has been received by the object.");
    /* rem:
  <SPAN CLASS=in>In: </SPAN>triggers the building block process.<BR>
    <BR>
    <SPAN CLASS=out>No Message: </SPAN>is activated until the message is received .<BR>
    <SPAN CLASS=out>Message Arrived: </SPAN>is activated when the wanted message is received.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message to wait for.<BR>
    <BR>
    The output is activated only when the message has been received.<BR>
    */
    /* warning:
    - This building block doesn't wait a message to be received, it check for at the current frame.<BR>
    - Therefore, you need to loop this building block by using the 'No Message' output which is activate if the message doesn't arrived.<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x61226639, 0x6d390cb7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCheckForMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CheckForMessageCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORATTACH:
    {
        beo->SetAsWaitingForMessages();
        return CKBR_OK;
    }
    break;

    case CKM_BEHAVIORDETACH:
    {
        beo->SetAsWaitingForMessages(FALSE);
        return CKBR_OK;
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateCheckForMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Check For Message");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("No Message");
    proto->DeclareOutput("Message Arrived");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");

    proto->DeclareOutParameter("Message Count", CKPGUID_INT);

    proto->SetBehaviorFlags(CKBEHAVIOR_MESSAGERECEIVER);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorCallbackFct(CheckForMessageCB);
    proto->SetFunction(CheckForMessage);

    *pproto = proto;
    return CK_OK;
}

int CheckForMessage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();

    beh->ActivateInput(0, FALSE);

    int mt = -1;
    beh->GetInputParameterValue(0, &mt);

    CKMessage *msg;
    int count = beo->GetLastFrameMessageCount();
    int receivedCount = 0;
    for (int i = 0; i < count; i++)
    {
        msg = beo->GetLastFrameMessage(i);
        if (msg->GetMsgType() == mt)
        {
            ++receivedCount;
        }
    }

    if (receivedCount)
    {
        beh->ActivateOutput(1);
        beh->SetOutputParameterValue(0, &receivedCount);
    }
    else
    {
        beh->ActivateOutput(0);
    }

    return CKBR_OK;
}
