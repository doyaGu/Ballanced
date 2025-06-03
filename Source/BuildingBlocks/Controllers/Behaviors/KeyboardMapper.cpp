/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            KeyboardMapper
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "../ControllersParams.h"

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

CKERROR CreateKeyboardMapperBehaviorProto(CKBehaviorPrototype **pproto);
int KeyboardMapper(const CKBehaviorContext &behcontext);
CKERROR KeyboardMapperCB(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeyboardMapperDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Keyboard Mapper");
    od->SetDescription("Binds specific messages to keyboard keys.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    This building block allows specific messages to be sent when certain keys on the keyboard are pressed.<BR>
    The object receiving the message can be of any type, therefore the color of a light could be controlled
    via the keyboard just as much as the position of a 3D Entity, for example. Messages are bound to keys
    in the custom dialog box by clicking on the desired key in the dialog box and then selecting (or
    creating) the message to be sent to the object, and then clicking on 'Add'.<BR>
    <BR>
    If the message 'DEAD' was meant to be sent to the character when pressing the D key,
    the way to do it would be to apply the Keyboard Mapper building block to the character
    (or to specify the character as the building block's Target, i.e. message receiver). Via the custom
    dialog box, the D key would be pressed, 'DEAD' would be entered as the message to be sent, and 'Add'
    would be clicked. "D bound with message DEAD" would then appear in the bottom half of the box
    indicating that this message had successfully been bound to the appropriate keyboard key.<BR>
    */
    /* warning:
    - Bound messages can only be sent to the the target object of the building block 'Keyboard Mapper'.<BR>
    */
    od->SetCategory("Controllers/Keyboard");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53b2bde, 0x2f8f1d74));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeyboardMapperBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateKeyboardMapperBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Keyboard Mapper");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    // data
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);
    // "keyBindedNumber"
    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeyboardMapper);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_CUSTOMEDITDIALOG | CKBEHAVIOR_MESSAGESENDER));
    proto->SetBehaviorCallbackFct(KeyboardMapperCB);

    *pproto = proto;
    return CK_OK;
}

int KeyboardMapper(const CKBehaviorContext &behcontext)
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
        if (beh->IsInputActive(0))
            beh->ActivateInput(0, FALSE);
    }

    CKBeObject *sender = beh->GetOwner();
    if (!sender)
        return CKBR_OWNERERROR;

    CKBeObject *beo = beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        if (!beo)
            return CKBR_GENERICERROR;
    }

    mappedKey *keyBinded = (mappedKey *)beh->GetLocalParameterWriteDataPtr(0);
    int keyBindedNumber = 0;
    beh->GetLocalParameterValue(1, &keyBindedNumber);

    unsigned char *keys = input->GetKeyboardState();

    CKMessageManager *mm = behcontext.MessageManager;

    for (int i = 0; i < keyBindedNumber; i++)
    {
        if (keys[keyBinded[i].key])
        {
            mm->SendMessageSingle(keyBinded[i].messageType, beo, sender);
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR KeyboardMapperCB(const CKBehaviorContext &behcontext)
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
        mappedKey *keyBinded = (mappedKey *)beh->GetLocalParameterWriteDataPtr(0);
        int keyBindedNumber;
        beh->GetLocalParameterValue(1, &keyBindedNumber);
        for (int i = 0; i < keyBindedNumber; i++)
        {
            SWAP32(keyBinded[i].key);
            keyBinded[i].messageType = behcontext.MessageManager->AddMessageType(keyBinded[i].messageName);
        }
    }
    break;
    }
    return CKBR_OK;
}
