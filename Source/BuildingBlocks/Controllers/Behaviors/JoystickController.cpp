/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            JoystickController
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateJoytickControllerBehaviorProto(CKBehaviorPrototype **pproto);
int DoJoytickController(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorJoystickControllerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Joystick Controller");
    od->SetDescription("Sends standard joystick messages to the building block owner.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    Messages sent are Joy_Up / Joy_Down / Joy_Left / Joy_Right / Joy_ButtonN, N ranging from 1 to 6.<BR>
    These messages are sent automatically on joystick commands.<BR>
  This building block is meant to be used in association with 'Character Controller'. This combination allow you to use and test quite quickly a character with its animations.<BR>
  <BR>
    See also: "Keyboard Controller", "Character Controller", "Joystick Mapper", "Joystick Waiter".<BR>
    */
    od->SetCategory("Controllers/Joystick");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x85ebc1, 0x21935422));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateJoytickControllerBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR DaJoystickCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        int tab[10];

        CKMessageManager *mm = behcontext.MessageManager;
        tab[0] = mm->AddMessageType("Joy_Left");
        tab[1] = mm->AddMessageType("Joy_Right");
        tab[2] = mm->AddMessageType("Joy_Up");
        tab[3] = mm->AddMessageType("Joy_Down");
        tab[4] = mm->AddMessageType("Joy_Button1");
        tab[5] = mm->AddMessageType("Joy_Button2");
        tab[6] = mm->AddMessageType("Joy_Button3");
        tab[7] = mm->AddMessageType("Joy_Button4");
        tab[8] = mm->AddMessageType("Joy_Button5");
        tab[9] = mm->AddMessageType("Joy_Button6");
        beh->SetLocalParameterValue(0, (void *)tab, 10 * sizeof(int));

        return 0;
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateJoytickControllerBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Joystick Controller");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Joystick Number", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Analog Position", CKPGUID_VECTOR, "0,0,0");

    // Messages
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // Mac Safe
    proto->DeclareSetting("Axis Sensitivity", CKPGUID_VECTOR, "0.5,0.5,0.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(DoJoytickController);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(DaJoystickCB);

    *pproto = proto;
    return CK_OK;
}

int DoJoytickController(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // The Off Input is Active
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CKBeObject *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKBeObject *sender = (CKBeObject *)beh->GetOwner();

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    int joy = 0;
    beh->GetInputParameterValue(0, &joy);

    if (!input->IsJoystickAttached(joy))
    {
        behcontext.Context->OutputToConsoleEx("Joystick not attached");
        return CKBR_GENERICERROR;
    }

    VxVector sensitivity(0.5f, 0.5f, 0.5f);
    beh->GetLocalParameterValue(1, &sensitivity);

    VxVector pos;
    input->GetJoystickPosition(joy, &pos);
    CKDWORD joybut = input->GetJoystickButtonsState(joy);

    int *tab = (int *)beh->GetLocalParameterReadDataPtr(0);

    CKMessageManager *mm = behcontext.Context->GetMessageManager();
    if (joybut & 1)
        mm->SendMessageSingle(tab[4], ent, sender);
    if (joybut & 2)
        mm->SendMessageSingle(tab[5], ent, sender);
    if (joybut & 4)
        mm->SendMessageSingle(tab[6], ent, sender);
    if (joybut & 8)
        mm->SendMessageSingle(tab[7], ent, sender);
    if (joybut & 16)
        mm->SendMessageSingle(tab[8], ent, sender);
    if (joybut & 32)
        mm->SendMessageSingle(tab[9], ent, sender);
    if (pos.y < -sensitivity.y)
        mm->SendMessageSingle(tab[2], ent, sender);
    if (pos.y > sensitivity.y)
        mm->SendMessageSingle(tab[3], ent, sender);
    if (pos.x < -sensitivity.x)
        mm->SendMessageSingle(tab[0], ent, sender);
    if (pos.x > sensitivity.x)
        mm->SendMessageSingle(tab[1], ent, sender);

    beh->SetOutputParameterValue(0, &pos);

    return CKBR_ACTIVATENEXTFRAME;
}
