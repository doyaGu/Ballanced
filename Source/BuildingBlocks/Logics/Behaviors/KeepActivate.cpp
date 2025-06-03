/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            KeepActivate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorKeepActivateDecl();
CKERROR CreateKeepActivateProto(CKBehaviorPrototype **);
int KeepActivate(const CKBehaviorContext &behcontext);

CKERROR KeepActivateCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorKeepActivateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Keep Active");
    od->SetDescription("Keeps streaming activated even if inputs has been activate only once.");
    /* rem:
    <SPAN CLASS=in>Reset: </SPAN>resets the building block to its initial state (deactivates all inputs).<BR>
    <SPAN CLASS=in>In 1: </SPAN>Input signals to be memorised.<BR>
    <SPAN CLASS=in>...</SPAN><BR>
    <BR>
    <SPAN CLASS=in>OutReset: </SPAN>is activated when the building block has been reset.<BR>
    <SPAN CLASS=in>Out 1: </SPAN>Constante output signal corresponding to the input memorised signal.<BR>
    <SPAN CLASS=in>...</SPAN><BR>
    <BR>
    <SPAN CLASS=setting>One at a time: </SPAN>specify whether several outputs can be activated simultaneously, or not.
    If only one output can be activated, then it is the one corresponding to the last activated input.<BR>
    <BR>
    This building block memorizes the input streaming(s), and activate outputs at each frame, even if the input signal(s) stops.<BR>
    */
    /* warning:
    - you can add as many Inputs as you want, the corresponding Outputs will be added automatically.<BR>
    - if two inputs are activated at same time, AND the "One at a time" setting is set to TRUE, then the highest input is the only one to be concidered.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7160133a, 0x1f2532fe));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateKeepActivateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateKeepActivateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Keep Active");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("Out Reset");
    proto->DeclareOutput("Out 1");

    proto->DeclareSetting("One at a time ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0"); // "Last Activated Input"

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(KeepActivate);

    proto->SetBehaviorCallbackFct(KeepActivateCallBack);

    *pproto = proto;
    return CK_OK;
}

int KeepActivate(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int last = 0;
    int count = beh->GetInputCount();

    if (beh->IsInputActive(0)) // enter by Reset
    {
        beh->SetLocalParameterValue(1, &last);

        for (int a = 0; a < count; a++)
        {
            beh->ActivateInput(a, FALSE);
        }
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    CKBOOL one_at_time = FALSE; // "one at a time" param
    beh->GetLocalParameterValue(0, &one_at_time);

    if (!one_at_time) // !one_at_time
    {
        for (int a = 1; a < count; a++)
        {
            if (beh->IsInputActive(a))
            {
                beh->ActivateOutput(a);
            }
        }
    }
    else
    {                                          // one_at_time
        beh->GetLocalParameterValue(1, &last); // last activated input
        for (int a = 1; a < count; a++)
        {
            if (beh->IsInputActive(a) && (a != last))
            {
                beh->ActivateInput(last, FALSE);
                last = a;
                beh->SetLocalParameterValue(1, &last);
                break;
            }
        }
        beh->ActivateOutput(last);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR KeepActivateCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

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
