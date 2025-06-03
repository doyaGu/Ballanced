/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            KeyEvent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateKeyEventBehaviorProto(CKBehaviorPrototype **pproto);
int KeyEvent(const CKBehaviorContext &behcontext);
CKERROR KeyEventCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeyEventDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Key Event");
    od->SetDescription("Activates one output when a specific key is pressed and a different output when the released.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Pressed: </SPAN>is activated when the key is pressed.<BR>
    <SPAN CLASS=out>Released: </SPAN>is activated when the key is released.<BR>
    <BR>
    <SPAN CLASS=pin>Key: </SPAN>key to wait for. This is chosen in the parameter dialog box by first
    clicking "Select Key" and then pressing the key to wait for. The desired key will then appear to
    the right of "Select Key".<BR>
    <BR>
    <SPAN CLASS=pout>Key State: </SPAN>current state of the expected key, with TRUE = pressed and FALSE = released.<BR>
    <BR>
    You can add many inputs parameters to detect combos.
    */
    od->SetCategory("Controllers/Keyboard");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1af2274b, 0x6b8c1524));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeyEventBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateKeyEventBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Key Event");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Pressed");
    proto->DeclareOutput("Released");

    proto->DeclareInParameter("Key Waited", CKPGUID_KEY);

    // Last Frame State
    proto->DeclareOutParameter("Key State", CKPGUID_BOOL);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeyEvent);
    proto->SetBehaviorCallbackFct(KeyEventCallBack);

    *pproto = proto;
    return CK_OK;
}

int KeyEvent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL state = FALSE;

    // If the Off input is active, we stop sending messages
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(0))
        {
            beh->ActivateInput(0, FALSE);
            beh->SetOutputParameterValue(0, &state);
        }
    }

    beh->GetOutputParameterValue(0, &state);

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    int i, count = beh->GetInputParameterCount();

    for (i = 0; i < count; ++i)
    {
        int key = 0;
        beh->GetInputParameterValue(i, &key);

        if (!input->IsKeyDown(key))
            break;
    }

    if ((i != count) && state)
    {
        state = FALSE;
        beh->ActivateOutput(1);
    }
    else if ((i == count) && !state)
    {
        state = TRUE;
        beh->ActivateOutput(0);
    }

    beh->SetOutputParameterValue(0, &state);

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR KeyEventCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        int c_pin = beh->GetInputParameterCount();

        char name[20];
        CKParameterIn *pin;
        for (int a = 0; a < c_pin; a++)
        {
            pin = beh->GetInputParameter(a);
            if (pin->GetGUID() != CKPGUID_KEY)
            {
                sprintf(name, "Key Waited %d", a);
                pin->SetName(name);
                pin->SetGUID(CKPGUID_KEY);
            }
        }
    }
    break;
    }

    return CKBR_OK;
}
