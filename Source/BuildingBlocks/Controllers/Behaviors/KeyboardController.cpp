/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            KeyboardController
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateKeyboardControllerBehaviorProto(CKBehaviorPrototype **pproto);
int DoKeyboardController(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeyboardControllerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Keyboard Controller");
    od->SetDescription("Emulates joystick messages using the keyboard.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    Maps keys to Joystick messages, as follows:<BR>
    Num 8 -> Joy_Up<BR>
    Num 2 -> Joy_Down<BR>
    Num 4 -> Joy_Left<BR>
    Num 6 -> Joy_Right<BR>
    Insert -> Button 1<BR>
    Home -> Button 2<BR>
    Prior -> Button 3<BR>
    Delete -> Button 4<BR>
    End -> Button 5<BR>
    Next -> Button 6<BR>
    <BR>
    This building block emulates a joystick.<BR>
    <BR>
    See also: "Joystick Controller", "Character Controller".<BR>
    <BR>
    */
    od->SetCategory("Controllers/Keyboard");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x43f1d000, 0x1a2b3c4d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeyboardControllerBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR DaKeyboardCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORREADSTATE:
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

CKERROR CreateKeyboardControllerBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Keyboard Controller");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareLocalParameter("Messages", CKPGUID_VOIDBUF); //(MAC ok)

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(DoKeyboardController);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_MESSAGESENDER | CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(DaKeyboardCB);

    *pproto = proto;
    return CK_OK;
}

int DoKeyboardController(const CKBehaviorContext &behcontext)
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

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    unsigned char *keys = input->GetKeyboardState();

    int *tab = (int *)beh->GetLocalParameterReadDataPtr(0);

    CKMessageManager *mm = behcontext.MessageManager;
    if (keys[CKKEY_INSERT])
        mm->SendMessageSingle(tab[4], ent, ent);
    if (keys[CKKEY_HOME])
        mm->SendMessageSingle(tab[5], ent, ent);
    if (keys[CKKEY_PRIOR])
        mm->SendMessageSingle(tab[6], ent, ent);
    if (keys[CKKEY_DELETE])
        mm->SendMessageSingle(tab[7], ent, ent);
    if (keys[CKKEY_END])
        mm->SendMessageSingle(tab[8], ent, ent);
    if (keys[CKKEY_NEXT])
        mm->SendMessageSingle(tab[9], ent, ent);
    if (keys[CKKEY_NUMPAD8])
        mm->SendMessageSingle(tab[2], ent, ent);
    if (keys[CKKEY_NUMPAD2])
        mm->SendMessageSingle(tab[3], ent, ent);
    if (keys[CKKEY_NUMPAD4])
        mm->SendMessageSingle(tab[0], ent, ent);
    if (keys[CKKEY_NUMPAD6])
        mm->SendMessageSingle(tab[1], ent, ent);

    return CKBR_ACTIVATENEXTFRAME;
}
