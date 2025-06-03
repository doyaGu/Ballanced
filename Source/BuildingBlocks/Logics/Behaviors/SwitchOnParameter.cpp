/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SwitchOnParameter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSwitchOnParameterDecl();
CKERROR CreateSwitchOnParameterProto(CKBehaviorPrototype **);
int SwitchOnParameter(const CKBehaviorContext &behcontext);

CKERROR SwitchOnParameterCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorSwitchOnParameterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Switch On Parameter");
    od->SetDescription("Switches the sequential stream to the corresponding ouput, following the result of comparison between 'Test' and the others input parameters");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>None: </SPAN>is activated if Test is not identical to any other input parameter.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated if Test is identical to Param 1.<BR>
    <SPAN CLASS=out>Out 2: </SPAN>is activated if Test is identical to Param 2.<BR>
    ...etc.<BR>
    <BR>
    <SPAN CLASS=pin>Test: </SPAN>parameter to test.<BR>
    <SPAN CLASS=pin>Pin 1: </SPAN>the first reference parameter.<BR>
    <SPAN CLASS=pin>Pin 2: </SPAN>the second reference parameter.<BR>
    ...etc.<BR>
    <BR>
    This building block switch the sequential stream to the "Out K" if "Test"="Pin K".<BR>
    <BR>
    You can add as many 'Outputs' as needed (the number of 'Input Parameters' will automatically be equal to the number of 'Inputs').<BR>
        You can change the type of the "Test" input parameter (FLOAT, VECTOR, MESH, COLOR ... anything !).
    The type of all other input parameters will be cast to the type of "Test".<BR>
    This building block is massevly used (especially with the "Parameter Selection").<BR>
    */
    /*
    - Do not change the type of another Input Parameter than the "Test" one, because its type will automatically match the type of "Test".<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c42aace, 0x1da45635));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSwitchOnParameterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateSwitchOnParameterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Switch On Parameter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("None");
    proto->DeclareOutput("Out 1");

    proto->DeclareInParameter("Test", CKPGUID_FLOAT);
    proto->DeclareInParameter("Pin 1", CKPGUID_FLOAT);

    proto->DeclareSetting("Float epsilon", CKPGUID_FLOAT, "0.001f");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_VARIABLEPARAMETERINPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SwitchOnParameter);

    proto->SetBehaviorCallbackFct(SwitchOnParameterCallBack);

    *pproto = proto;
    return CK_OK;
}

int SwitchOnParameter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int count_param_in = beh->GetInputParameterCount();
    int count_out = beh->GetOutputCount();

    CKParameterIn *pin = NULL;
    pin = beh->GetInputParameter(0);
    int tester_type = pin->GetType(); // we get the Type of the tester
    CKParameter *real = NULL;
    real = pin->GetRealSource();
    void *tester_ptr = NULL; // we get the Ptr  of the tester
    tester_ptr = real->GetReadDataPtr();
    int tester_size = real->GetDataSize(); // we get the Size of the tester

    float epsilon = 0.0f;
    beh->GetLocalParameterValue(0, &epsilon);

    CKParameterType stringType = behcontext.ParameterManager->ParameterGuidToType(CKPGUID_STRING);
    CKParameterType floatType = behcontext.ParameterManager->ParameterGuidToType(CKPGUID_FLOAT);
    CKParameterType boolType = behcontext.ParameterManager->ParameterGuidToType(CKPGUID_BOOL);
    CKBOOL derivedFromFloat = behcontext.ParameterManager->IsDerivedFrom(tester_type, floatType);

    void *param_ptr;
    for (int i = 1; i < count_param_in; ++i)
    {
        pin = NULL;
        if (pin = beh->GetInputParameter(i))
            ;
        else
            return 0;

        if (pin->GetType() == tester_type)
        {
            real = NULL;
            if (real = pin->GetRealSource())
            {
                param_ptr = real->GetReadDataPtr();

                if (derivedFromFloat && (epsilon != 0.0f))
                {
                    float t = *(float *)tester_ptr;
                    float p = *(float *)param_ptr;

                    if ((p >= (t - epsilon)) && (p < (t + epsilon)))
                    {
                        beh->ActivateInput(0, FALSE);
                        beh->ActivateOutput(i);
                        return CKBR_OK;
                    }
                }
                else
                {
                    if (tester_type == stringType) // Particular case : string (the buffer can be longer than the string itself)
                    {
                        if (!strcmp((CKSTRING)param_ptr, (CKSTRING)tester_ptr))
                        {
                            beh->ActivateInput(0, FALSE);
                            beh->ActivateOutput(i);
                            return CKBR_OK;
                        }
                    }
                    else
                    {
                        if (tester_type == boolType)
                        {
                            CKBOOL t = *(CKBOOL *)tester_ptr;
                            CKBOOL p = *(CKBOOL *)param_ptr;
                            if (p)
                                p = TRUE;
                            if (t)
                                t = TRUE;

                            if (t == p)
                            {
                                beh->ActivateInput(0, FALSE);
                                beh->ActivateOutput(i);
                                return CKBR_OK;
                            }
                        }
                        else
                        {
                            if (!memcmp(param_ptr, tester_ptr, tester_size))
                            {
                                beh->ActivateInput(0, FALSE);
                                beh->ActivateOutput(i);
                                return CKBR_OK;
                            }
                        }
                    }
                }
            }
        }
    }

    // None have been chosen
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SwitchOnParameterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIOREDITED:
    {
        int c_out = beh->GetOutputCount();
        int c_pin = beh->GetInputParameterCount();

        char pin_str[10];

        CKParameterIn *pin1 = beh->GetInputParameter(0);
        if (!pin1)
            return CKBR_OK;
        int type_1 = pin1->GetType();

        while (c_pin < c_out) // we must add 'Input Param'
        {
            sprintf(pin_str, "Pin %d", c_pin);
            beh->CreateInputParameter(pin_str, type_1);
            ++c_pin;
        }

        while (c_pin > c_out) // we must remove 'Input Param'
        {
            --c_pin;
            CKDestroyObject(beh->RemoveInputParameter(c_pin));
        }

        // we check the type of each 'Input Parameter'
        CKParameterIn *pin;
        while (c_pin)
        {
            c_pin--;
            pin = beh->GetInputParameter(c_pin);
            if (pin->GetType() != type_1)
            {
                pin->SetType(type_1, TRUE);
            }
        }
    }
    break;
    }

    return CKBR_OK;
}
