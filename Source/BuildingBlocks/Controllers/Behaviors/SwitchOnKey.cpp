/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SwitchOnKey
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSwitchOnKeyDecl();
CKERROR CreateSwitchOnKeyProto(CKBehaviorPrototype **pproto);
int SwitchOnKey(const CKBehaviorContext &behcontext);
CKERROR SwitchOnKeyCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorSwitchOnKeyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Switch On Key");
    od->SetDescription("Activates the appropriate output when receiving a Key.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is activated when Key1 is pressed.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated when Key2 in pressed.<BR>
    ...etc.<BR>
    <BR>
    <SPAN CLASS=pin>Key 0: </SPAN>first awaited Key.<BR>
    <SPAN CLASS=pin>Key 1: </SPAN>second awaited Key.<BR>
    ...etc.<BR>
    <BR>
    You can add as many 'Output' as you want, the 'Input Parameters' will be added automatically.<BR>
    This building block doesn't need to loop.<BR>
    <BR>
    See also : WaitKey .
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1f49612e, 0x6cd63de0));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchOnKeyProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Controllers/Keyboard");
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateSwitchOnKeyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Switch On Key");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out 0");
    proto->DeclareOutput("Out 1");

    proto->DeclareInParameter("Key 0", CKPGUID_KEY);
    proto->DeclareInParameter("Key 1", CKPGUID_KEY);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchOnKey);

    proto->SetBehaviorCallbackFct(SwitchOnKeyCallBack);

    *pproto = proto;
    return CK_OK;
}

int SwitchOnKey(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // We get by 'OFF'
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }
    int count_param_in = beh->GetInputParameterCount();

    int count_out = beh->GetOutputCount();

    if (count_param_in != count_out)
        throw "Input parameter / Output mismatch";

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    int key = 0;
    for (int i = 0; i < count_param_in; ++i)
    { // For each Key to look for ( in the 'pin's )
        // we get the input key
        beh->GetInputParameterValue(i, &key);
        // if the key is down, we activate the output
        if (input->IsKeyDown(key))
            beh->ActivateOutput(i);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SwitchOnKeyCallBack(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        int c_out = beh->GetOutputCount();
        int c_pin = beh->GetInputParameterCount();

        char pin_str[10];

        while (c_pin < c_out)
        { // we must add Input Params
            sprintf(pin_str, "Key %d", c_pin);
            beh->CreateInputParameter(pin_str, CKPGUID_KEY);
            ++c_pin;
        }

        while (c_pin > c_out)
        { // we must remove Input Params
            --c_pin;
            behcontext.Context->DestroyObject(beh->RemoveInputParameter(c_pin));
        }
    }
    break;
    }

    return CKBR_OK;
}
