/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Parameter Selector
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorParameterSelectorDecl();
CKERROR CreateParameterSelectorProto(CKBehaviorPrototype **);
int ParameterSelector(const CKBehaviorContext &behcontext);

CKERROR ParameterSelectorCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorParameterSelectorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Parameter Selector");
    od->SetDescription("According to the Input activated, routes the corresponding Input Parameter to the Output Parameter.");
    /* rem:
    <SPAN CLASS=in>In 0: </SPAN>triggers the building block process.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the building block process.<BR>
    <SPAN CLASS=in>...etc</SPAN>.
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the building block process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>pIn 0: </SPAN>first parameter.<BR>
    <SPAN CLASS=pin>pIn 1: </SPAN>second parameter.<BR>
    <SPAN CLASS=pin>...etc</SPAN>.
    <BR>
    <SPAN CLASS=pout>Selected: </SPAN>selected parameter.<BR>
    <BR>
    If "In 0" is activated, "Selected" parameter will be "pIn 0".<BR>
    If "In 1" is activated, "Selected" parameter will be "pIn 1".<BR>
    ...etc.<BR>
    Generally speaking, if "In K" is activated, Selected parameter will be "pIn K".<BR>
    You can add as many 'Inputs' as needed (number of 'Input Parameters' will automatically be equal to the number of 'Inputs').<BR>
    Note: if you change the type of parameter out 'Selected', the 'input parameters' will transform there type to match it.<BR>
    */
    od->SetCategory("Logics/Streaming");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x63eb2b0c, 0x27e2767e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateParameterSelectorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateParameterSelectorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Parameter Selector");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("pIn 0", CKPGUID_FLOAT);
    proto->DeclareInParameter("pIn 1", CKPGUID_FLOAT);

    proto->DeclareOutParameter("Selected", CKPGUID_FLOAT);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ParameterSelector);

    proto->SetBehaviorCallbackFct(ParameterSelectorCallBack);

    *pproto = proto;
    return CK_OK;
}

int ParameterSelector(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int count_param_in = beh->GetInputParameterCount();
    int count_in = beh->GetInputCount();

    CKParameterOut *pout = beh->GetOutputParameter(0);

    CKParameterIn *pin;
    CKParameter *real;

    for (int i = 0; i < count_in; i++)
    {
        if (beh->IsInputActive(i))
        {
            beh->ActivateInput(i, FALSE);
            pin = beh->GetInputParameter(i);
            real = pin->GetRealSource();
            pout->CopyValue(real);
            break;
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR ParameterSelectorCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIOREDITED:
    {
        int c_in = beh->GetInputCount();
        int c_pin = beh->GetInputParameterCount();

        CKParameterIn *pin;
        CKParameterOut *pout;

        if (pout = beh->GetOutputParameter(0))
        {

            int type_out = pout->GetType();

            char in_str[10];

            while (c_in > c_pin) // we must add 'Input Parameters'
            {
                sprintf(in_str, "pIn %d", c_pin);
                CKParameterIn *pin = beh->CreateInputParameter(in_str, type_out);
                c_pin++;
            }

            while (c_in < c_pin) // we must remove 'Input Parameters'
            {
                CKDestroyObject(beh->RemoveInputParameter(--c_pin));
            }

            while (c_pin) // we check the type of each 'Input Parameter'
            {
                c_pin--;
                pin = beh->GetInputParameter(c_pin);
                if (pin->GetType() != type_out)
                {
                    pin->SetType(type_out, TRUE);
                }
            }
        }
    }
    break;
    }

    return CKBR_OK;
}
