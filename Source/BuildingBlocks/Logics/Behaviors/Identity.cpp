/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Identity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorIdentityDecl();
CKERROR CreateIdentityProto(CKBehaviorPrototype **);
int Identity(const CKBehaviorContext &behcontext);
CKERROR IdentityCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorIdentityDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Identity");
    od->SetDescription("Outputs the Input Parameter(s) when activated.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    This building block is usefull to put a value into a local parameter, at runt-time.<BR>
    Other ways to perform that kind of thing is to use a "Parameter Selector" building block.<BR>
    Sometimes, using a Parameter Operation in conjunction with the "Identity" building block can be replaced with a single "Op" building block.<BR>
    You can add as many Inputs Parameters as you want ( from any type ), the Output Parameters will be added automtically.<BR>
    */
    od->SetCategory("Logics/Calculator");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x15151652, 0xaeefffd5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIdentityProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIdentityProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Identity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("pIn 0", CKPGUID_FLOAT);
    proto->DeclareOutParameter("pOut 0", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetFunction(Identity);
    proto->SetBehaviorCallbackFct(IdentityCallBack);

    *pproto = proto;
    return CK_OK;
}

int Identity(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int count_in = beh->GetInputParameterCount();
    if (count_in != beh->GetOutputParameterCount())
        throw "Wrong number of Input/Output parameters";

    CKParameterIn *pin;
    CKParameterOut *pout;

    for (int i = 0; i < count_in; ++i)
    {
        pin = beh->GetInputParameter(i);
        pout = beh->GetOutputParameter(i);

        pout->CopyValue(pin->GetRealSource());
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR IdentityCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        CKParameterOut *pout;

        char str[16];
        int i;
        for (i = 0; i < beh->GetInputParameterCount(); i++)
        {
            pin = beh->GetInputParameter(i);
            pout = beh->GetOutputParameter(i);
            if (pout) // the output exist, we only have to check its type
            {
                if (pout->GetType() != pin->GetType())
                {
                    pout->SetType(pin->GetType());
                    sprintf(str, "pOut %d", i);
                    pout->SetName(str);
                }
            }
            else // we have to create the output
            {
                sprintf(str, "pOut %d", i);
                beh->CreateOutputParameter(str, pin->GetType());
            }
        }
        for (; i < beh->GetOutputParameterCount(); i++)
        {
            CKDestroyObject(beh->RemoveOutputParameter(i));
        }
    }
    }

    return CKBR_OK;
}
