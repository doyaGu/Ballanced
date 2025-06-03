/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GetData Message
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetDataMessageDecl();
CKERROR CreateGetDataMessageProto(CKBehaviorPrototype **);
int GetDataMessage(const CKBehaviorContext &behcontext);
CKERROR GetDataMessageCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKObjectDeclaration *FillBehaviorGetDataMessageDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Message Data");
    od->SetDescription("Reads the data bound with the selected message.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Message: </SPAN>message with which the datas are bound.<BR>
    <SPAN CLASS=pin>Sender: </SPAN>object which sent the message.<BR>
    <BR>
    This building block should be linked to the output of the 'WaitMessage' (or similar one), with a link delay=0.<BR>
    Simply add Output Parameters of the same type and in the same order as the Input Parameters you added to the corresponding SendMessage.<BR>
    <BR>
    This building block were useful when it wasn't possible to share parameter between two scripts.<BR>
    But now, it is not so useful as it was, because with shortcut parameters (Ctrl C ... Ctrl+Shift V), you can pass parameters from one script to another.<BR>
    It's still useful if you use the Dev version, and intend to make your own player, which will load different levels, and try to share informations
    between them (like the score, or the found bonus ... or for example the name of the new level to load). This communication can be done with messages sent to the level.<BR>
    See also : Send Message, Wait Message, Send Message To Group.<BR>
    */
    /* warning:
    - You have to link this building block with a link delay=0, to "Wait Message" (or a similar behavior).<BR>
    */
    od->SetCategory("Logics/Message");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45875fee, 0x45875fdd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetDataMessageProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetDataMessageProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Message Data");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Message", CKPGUID_MESSAGE, "OnClick");

    proto->DeclareOutParameter("Sender", CKPGUID_BEOBJECT);

    proto->DeclareSetting("Can get several Message per Frame", CKPGUID_BOOL);
    proto->DeclareLocalParameter(NULL, CKPGUID_INT); // MultiMessage Index (if we receiv several message in the same frame)

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGERECEIVER | CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTS));
    proto->SetFunction(GetDataMessage);
    proto->SetBehaviorCallbackFct(GetDataMessageCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR GetDataMessageCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // Can Get several Messages ?
        CKBOOL canGetSeveralMsg = FALSE;
        beh->GetLocalParameterValue(0, &canGetSeveralMsg);

        if (canGetSeveralMsg)
        {
            if (beh->GetInputCount() == 1)
            { // must add output/input
                beh->AddOutput("Data Received");
                beh->AddInput("Next Data");
            }
        }
        else
        {
            if (beh->GetInputCount() == 2)
            { // must remove output/input
                beh->DeleteOutput(1);
                beh->DeleteInput(1);
            }
        }
    }

    break;
    }

    return CKBR_OK;
}

/*******************************************************/
/*             BUILDING BLOCK FUNCTION                 */
/*******************************************************/
int GetDataMessage(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Get Owner
    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    //--- Enter by In
    if (beh->IsInputActive(0))
    {

        beh->ActivateInput(0, FALSE);
        int multiMessageIndex = 0;
        beh->SetLocalParameterValue(1, &multiMessageIndex);

        //--- Enter by Next Data
    }
    else
    {

        beh->ActivateInput(1, FALSE);
    }

    // Get Message
    int Msg = -1;
    beh->GetInputParameterValue(0, &Msg);

    // Get Input Data Count
    int pinDataCount = beh->GetOutputParameterCount() - 1;

    // Get Last Frame Message Count
    int msgc = beo->GetLastFrameMessageCount();

    // Can Get several Messages ?
    CKBOOL canGetSeveralMsg = FALSE;
    beh->GetLocalParameterValue(0, &canGetSeveralMsg);

    int outputToActivate = canGetSeveralMsg ? 1 : 0;

    // Get multiMessageIndex
    int multiMessageIndex = 0;
    beh->GetLocalParameterValue(1, &multiMessageIndex);

    // For All Received Message
    for (int i = multiMessageIndex; i < msgc; ++i)
    {
        CKMessage *msg = beo->GetLastFrameMessage(i);
        if (msg->GetMsgType() == Msg)
        {
            beh->SetOutputParameterObject(0, msg->GetSender());

            CKParameter *tpout, *pout;
            if (pinDataCount == msg->GetParameterCount())
            {
                for (int j = 0; j < pinDataCount; j++)
                {
                    tpout = msg->GetParameter(j);
                    pout = beh->GetOutputParameter(j + 1);
                    pout->CopyValue(tpout);
                }

                ++multiMessageIndex;
                beh->SetLocalParameterValue(1, &multiMessageIndex);

                beh->ActivateOutput(outputToActivate);
                return CKBR_OK;
            }
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}
