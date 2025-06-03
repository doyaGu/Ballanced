/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       DigitalJoystickMapper
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ControllersParams.h"

#define JOYUP    1008
#define JOYDOWN  1009
#define JOYLEFT  1010
#define JOYRIGHT 1011
#define JOYB     1012

// When loading on a Big-Endian processor 
// VOIDBUF Parameters need to be converted
// only the owner knows the format of the underlying variables
#ifdef macintosh
    #include <Endian.h>
    #define SWAP16(x)	x = Endian16_Swap(x)
    #define SWAP32(x)	x = Endian32_Swap(x)
#else
    #define SWAP16(x)
    #define SWAP32(x)
#endif

CKERROR CreateDigitalJoystickMapperBehaviorProto(CKBehaviorPrototype **pproto);
int DigitalJoystickMapper(const CKBehaviorContext &behcontext);
CKERROR DigitalJoystickMapperCB(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDigitalJoystickMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Joystick Mapper");
    od->SetDescription("Binds specific messages to joystick commands.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=pout>Analog Position: </SPAN>vector containing joystick's X-axis, Y-axis and Z-axis values respectively.<BR>
    <BR>
    <SPAN CLASS=setting>Axis Sensitivity: </SPAN>threshold in percentages of 3 axes after which the messages are triggered. 1 means sensitive response and 99 demands maximum effort to get a response.<BR>
    <BR>
    This building block allows specific messages to be sent when receiving a joystick command.
    The object receiving the message can be of any type, therefore the color of a light could
    be controlled via joystick just as much as the position of a 3D Entity, for example.<BR>
    Messages are bound to joystick commands in the custom dialog box by clicking on the desired command
    in the dialog box and then selecting (or creating) the message to be sent to the object, and then
    clicking on 'Add'. Selection can also be done by pressing the joystick buttons directly.<BR>
    <BR>
    If the message 'DEAD' was meant to be sent to the character when pressing UP,
    the way to do it would be to apply the Joystick Mapper building block to the character
    (or to specify the character as the building block's Target, i.e. message receiver). Via the custom
    dialog box, UP would be pressed, 'DEAD' would be entered as the message to be sent, and 'Add'
    would be clicked. "JOY_UP bound with message DEAD" would then appear in the bottom half of the box
    indicating that this message had successfully been bound to the appropriate joystick command.<BR>
  <BR>
  See also : Joystick Waiter.<BR>
    */
    /* warning:
    - Bound messages can only be sent to the the target object of the building block 'Joystick Mapper'.<BR>
    */
    od->SetCategory("Controllers/Joystick");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3ed1bea, 0x25692ecc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDigitalJoystickMapperBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateDigitalJoystickMapperBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Joystick Mapper");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Joystick Number", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Analog Position", CKPGUID_VECTOR, "0,0,0");

    // "data"
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // MAc OK
    // "bindedNumber"
    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0");
    proto->DeclareSetting("Axis Sensitivity", CKPGUID_VECTOR, "0.5,0.5,0.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_CUSTOMEDITDIALOG | CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_TARGETABLE));
    proto->SetFunction(DigitalJoystickMapper);
    proto->SetBehaviorCallbackFct(DigitalJoystickMapperCB);

    *pproto = proto;
    return CK_OK;
}

int DigitalJoystickMapper(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // If the Off input is active, we stop sending messages
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }
    else
    {
        beh->ActivateInput(0, FALSE);
    }

    CKBeObject *sender = beh->GetOwner();
    if (!sender) return CKBR_OWNERERROR;

    CKBeObject *beo = beh->GetTarget();
    if (!beo) return CKBR_OWNERERROR;

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        if (!beo) return CKBR_GENERICERROR;
    }

    mappedJoy *joyBinded = (mappedJoy *)beh->GetLocalParameterWriteDataPtr(0);
    int joyBindedNumber = 0;
    beh->GetLocalParameterValue(1, &joyBindedNumber);

    if (!input->IsJoystickAttached(joyBindedNumber))
    {
        behcontext.Context->OutputToConsoleEx("Joystick not attached");
        if (!beo) return CKBR_GENERICERROR;
    }

    VxVector sensitivity(0.5f, 0.5f, 0.5f);
    beh->GetLocalParameterValue(2, &sensitivity);

    int joy = 0;
    beh->GetInputParameterValue(0, &joy);

    VxVector pos;
    input->GetJoystickPosition(joy, &pos);
    CKDWORD joybut = input->GetJoystickButtonsState(joy);

    for (int i = 0; i < joyBindedNumber; i++)
    {
        int ind = joyBinded[i].joy; // ? => Resource IDC_JOYUP=1008
        switch (ind)
        {
        case JOYUP:
            if (pos.y < -sensitivity.y) behcontext.MessageManager->SendMessageSingle(joyBinded[i].messageType, beo, sender);
            break;
        case JOYDOWN:
            if (pos.y > sensitivity.y) behcontext.MessageManager->SendMessageSingle(joyBinded[i].messageType, beo, sender);
            break;
        case JOYLEFT:
            if (pos.x < -sensitivity.x) behcontext.MessageManager->SendMessageSingle(joyBinded[i].messageType, beo, sender);
            break;
        case JOYRIGHT:
            if (pos.x > sensitivity.x) behcontext.MessageManager->SendMessageSingle(joyBinded[i].messageType, beo, sender);
            break;
        default:
            if (joybut & (1 << (ind - JOYB))) behcontext.MessageManager->SendMessageSingle(joyBinded[i].messageType, beo, sender);
            break;
        }
    }

    beh->SetOutputParameterValue(0, &pos);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR DigitalJoystickMapperCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKInterfaceManager *UIMan = (CKInterfaceManager *)behcontext.Context->GetManagerByGuid(INTERFACE_MANAGER_GUID);
        if (UIMan)
            UIMan->CallBehaviorEditionFunction(beh, 0);
    }
    break;

    case CKM_BEHAVIORLOAD:
    {
        mappedJoy *joyBinded = (mappedJoy *)beh->GetLocalParameterWriteDataPtr(0);
        int joyBindedNumber;
        beh->GetLocalParameterValue(1, &joyBindedNumber);
        for (int i = 0; i < joyBindedNumber; i++)
        {
            SWAP32(joyBinded[i].joy);
            joyBinded[i].messageType = behcontext.MessageManager->AddMessageType(joyBinded[i].messageName);
        }
    }
    break;
    }
    return CKBR_OK;
}
