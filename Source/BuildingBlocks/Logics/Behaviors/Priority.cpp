/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Priority
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPriorityDecl();
CKERROR CreatePriorityProto(CKBehaviorPrototype **);
int Priority(const CKBehaviorContext &behcontext);

CKERROR PriorityCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorPriorityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Priority");
    od->SetDescription("Sets priorities between several signals (Inputs Activation).");
    /* rem:
      <SPAN CLASS=in>Reset: </SPAN>resets the building block to its initial state.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the building block process.<BR>
    <SPAN CLASS=in>In 2: </SPAN>triggers the building block process.<BR>
    <BR>
    <SPAN CLASS=out>Out Reset: </SPAN>is activated when the building block has been reset.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated when the building block has been activated by In1 (assuming the priority level allows it).<BR>
    <SPAN CLASS=out>Out 2: </SPAN>is activated when the building block has been activated by In2 (assuming the priority level allows it).<BR>
    <BR>
      You can add as many inputs as needed (number of outputs will automatically be equal to the number of inputs).<BR>
    <BR>
    When reseted, the building block sets the priority level to 0.<BR>
    An incoming signal in "In 1" sets the priority to 1, and activates the Out1 output.<BR>
    An incoming signal in "In 2" raises the priority to 2, activates the "Out 2" output, and disables "In 1" (resulting in the impossibility for "In 1" signals to activate "Out 1").<BR>
    Generally speaking, an incoming signal in "In K" activates "Out K" only if priority<=K. If so, priority is raised to K, and any incoming signal in a lower input can't activate the corresponding output.<BR>
    The Reset input resets the priority to 0 and unlocks all inputs.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xffb40d68, 0x45455444));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePriorityProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreatePriorityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Priority");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("In 1");
    proto->DeclareInput("In 2");

    proto->DeclareOutput("OutReset");
    proto->DeclareOutput("Out 1");
    proto->DeclareOutput("Out 2");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT); // "io_stop"

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Priority);

    proto->SetBehaviorCallbackFct(PriorityCallBack);

    *pproto = proto;
    return CK_OK;
}

int Priority(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL AllTrue = TRUE;

    int a;
    CKBOOL flag = FALSE;

    int io_stop = 0;
    if (beh->IsInputActive(0)) // reset active
    {
        io_stop = 0;
        beh->SetLocalParameterValue(0, &io_stop);

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    beh->GetLocalParameterValue(0, &io_stop);
    int c = beh->GetInputCount();

    for (a = c - 1; a >= io_stop; --a) // We Scan the Inputs, from Down to Up
    {

        if (beh->IsInputActive(a))
        {
            beh->ActivateOutput(a); // Activation of corresponding Output
            break;
        }
    }

    if (a < io_stop)
        a = io_stop; // if no input are activated, we let the priority where it was.

    for (int b = 0; b <= a; b++) // Desactivation of all other Inputs
    {
        beh->ActivateInput(b, FALSE);
    }

    io_stop = a;
    beh->SetLocalParameterValue(0, &io_stop); // we set the value of 'io_stop'

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR PriorityCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORRESET:
    {
        int io_stop = 0;
        beh->SetLocalParameterValue(0, &io_stop);
    }
    break;

    case CKM_BEHAVIOREDITED:
    {
        int c = beh->GetInputCount();
        int c_out = beh->GetOutputCount();
        char out_str[10];

        while (c > c_out) // we must add 'Outputs'
        {
            sprintf(out_str, "Out %d", c_out);
            beh->AddOutput(out_str);
            c_out++;
        }

        while (c < c_out) // we must remove 'Outputs'
        {
            c_out--;
            beh->DeleteOutput(c_out);
        }

        break;
    }
    }

    return CKBR_OK;
}
