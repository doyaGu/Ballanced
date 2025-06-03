/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Wait Message
//
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorWaitMessageDecl();
CKERROR CreateWaitMessageProto(CKBehaviorPrototype **);
int WaitMessage(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWaitMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Wait Message");
    od->SetDescription("Waits the receipt of a Message.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>Starts waiting for a specific message.<BR>
    <SPAN CLASS=in>Off: </SPAN>Stops waiting for a specific message.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the awaited message is received.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message to wait for.<BR>
    <BR>
    The output is activated only when the message has been received.<BR>
    */
    /* warning:
    - this is a special building block which actually does not stay active, but gives the opportunity
    to the message manager to activate the output of "Wait Message" when the message is received.
    Therefore don't be surprised if during a Trace Mode, you can't detect the activation of this building block.<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4587ffee, 0x4587ffdd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWaitMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWaitMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Wait Message");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Stop");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGERECEIVER | CKBEHAVIOR_TARGETABLE));
    proto->SetFunction(WaitMessage);

    *pproto = proto;
    return CK_OK;
}

int WaitMessage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKMessageManager *mm = behcontext.MessageManager;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    int Msg = -1;
    beh->GetInputParameterValue(0, &Msg);
    if (Msg < 0)
        return CKBR_OK;

    // Enter by on, we register
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);

        // We register the output of the behavior to wait for a specific message
        mm->RegisterWait(Msg, beh, 0, beo);
    }

    // Enter by off, we unregister
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        // We unregister the output of the behavior to wait for a specific message
        mm->UnRegisterWait(Msg, beh, 0);
    }

    return CKBR_OK;
}
