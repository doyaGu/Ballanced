/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RandomSwitch
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRandomSwitchDecl();
CKERROR CreateRandomSwitchProto(CKBehaviorPrototype **);
int RandomSwitch(const CKBehaviorContext &behcontext);

CKERROR RandomSwitchCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorRandomSwitchDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Random Switch");
    od->SetDescription("Switches the sequential stream to a randomly chosen ouput according to each coef.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is randomly activated with a probabilty of Coef 0 (assuming the sum of all coeficient is 1).<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is randomly activated with a probabilty of Coef 1 (assuming the sum of all coeficient is 1).<BR>
    ...etc.<BR>
    <BR>
    <SPAN CLASS=pin>Coef 0: </SPAN>coefficient of propability which correspond to the activation of the output "Out 0".<BR>
    <SPAN CLASS=pin>Coef 1: </SPAN>coefficient of propability which correspond to the activation of the output "Out 1".<BR>
    ...etc.<BR>
    Setting:
    <SPAN CLASS=setting>Forbid twice the same: </SPAN>forbid the redundancy of twice the same activation (if you want to prevent the current activated output to be activated next time).<BR>
    <BR>
    E.g.: if "Coef 0" is twice bigger than "Coef 1", then "Out 0" has twice the probability than "Out 1" to be activated.<BR>
    You can add as many Outputs as you want, the Input Parameters will be added automtically.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x79d72fde, 0x2e9d0912));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRandomSwitchProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateRandomSwitchProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Random Switch");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    int In, Out;
    In = proto->DeclareInput("In");

    Out = proto->DeclareOutput("Out 1");
    Out = proto->DeclareOutput("Out 2");

    proto->DeclareInParameter("Coef 1", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Coef 2", CKPGUID_FLOAT, "1");

    proto->DeclareSetting("Forbid twice the same", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter(NULL, CKPGUID_INT); // memorize the last output activated

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_VARIABLEOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RandomSwitch);

    proto->SetBehaviorCallbackFct(RandomSwitchCallBack);

    *pproto = proto;
    return CK_OK;
}

int RandomSwitch(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    // get last activated
    CKBOOL b = FALSE;
    beh->GetLocalParameterValue(0, &b);
    int old_out;
    if (b) // Forbid redundancy ?
    {
        beh->GetLocalParameterValue(1, &old_out);
    }
    else
    {
        old_out = -1;
    }

    int c, input_count = beh->GetInputParameterCount();
    float *coef = new float[input_count];
    float sum = 0.0f;

    for (c = 0; c < input_count; ++c) // get coefs
    {
        if (c != old_out)
        {
            coef[c] = 1.0f;
            beh->GetInputParameterValue(c, &coef[c]);
            sum += coef[c];
        }
    }

    float res = sum * rand() / RAND_MAX;

    sum = 0.0f;
    for (c = 0; c < input_count; ++c)
    {
        if (c != old_out)
        {
            sum += coef[c];
            if (res <= sum)
            {
                beh->ActivateOutput(c);
                if (b)
                    beh->SetLocalParameterValue(1, &c);
                break;
            }
        }
    }

    delete[] coef;

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR RandomSwitchCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        int c_out = beh->GetOutputCount();
        int c_pin = beh->GetInputParameterCount();

        char pin_str[10];

        while (c_pin < c_out) // we must add 'Input Param'
        {
            sprintf(pin_str, "Coef %d", c_pin);
            beh->CreateInputParameter(pin_str, CKPGUID_FLOAT);
            ++c_pin;
        }

        while (c_pin > c_out) // we must remove 'Input Param'
        {
            --c_pin;
            CKDestroyObject(beh->RemoveInputParameter(c_pin));
        }
    }
    break;
    }

    return CKBR_OK;
}
