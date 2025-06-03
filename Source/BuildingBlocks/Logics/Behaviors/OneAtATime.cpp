/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            OneAtATime
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorOneAtATimeDecl();
CKERROR CreateOneAtATimeProto(CKBehaviorPrototype **);
int OneAtATime(const CKBehaviorContext &behcontext);

CKERROR OneAtATimeCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorOneAtATimeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("One At A Time");
    od->SetDescription("When reseted, this building block activates a Output corresponding to the last activated Input.");
    /* rem:
    <SPAN CLASS=in>Reset: </SPAN>Activates the output corresponding to the last activated input.<BR>
    <SPAN CLASS=in>In 1: </SPAN>Input signals to be memorized.<BR>
    <SPAN CLASS=in>In 2: </SPAN>Input signals to be memorized.<BR>
    <SPAN CLASS=in>...</SPAN><BR>
    <BR>
    <SPAN CLASS=out>Out Reset: </SPAN>is activated when the building block has been reseted.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>activated if "In 1" is the last input activated just before the Reset signal.<BR>
    <SPAN CLASS=out>...</SPAN><BR>
    <BR>
    While the <SPAN CLASS=in>'Reset'</SPAN> signal isn't activated, none of the inputs signal are processed.<BR>
    You can add as many inputs as needed (number of outputs will automatically be equal to the number of inputs).<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x271b61af, 0x41b44d43));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateOneAtATimeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateOneAtATimeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("One At A Time");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("In 1");
    proto->DeclareInput("In 2");

    proto->DeclareOutput("Out Reset");
    proto->DeclareOutput("Out 1");
    proto->DeclareOutput("Out 2");

    proto->DeclareLocalParameter(NULL, CKPGUID_INT); // "last actif"

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(OneAtATime);

    proto->SetBehaviorCallbackFct(OneAtATimeCallBack);

    *pproto = proto;
    return CK_OK;
}

int OneAtATime(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int a = 1, c = beh->GetInputCount();

    while (a < c) // we put in the 'local param', the active input
    {
        if (beh->IsInputActive(a))
        {
            beh->SetLocalParameterValue(0, &a);
            break;
        }
        a++;
    }

    while (a < c) // and we deactivate all the other inputs
    {
        beh->ActivateInput(a, FALSE);
        a++;
    }

    if (beh->IsInputActive(0)) // if 'Reset' is active ...
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        beh->GetLocalParameterValue(0, &a);

        beh->ActivateOutput(a);
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR OneAtATimeCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORRESET:
    {
        int a = 0;
        beh->SetLocalParameterValue(0, &a);
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
            c_out++;
            beh->DeleteOutput(c_out);
        }

        break;
    }
    }

    return CKBR_OK;
}
