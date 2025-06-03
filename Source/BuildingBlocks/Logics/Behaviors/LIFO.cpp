/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            LIFO
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLIFODecl();
CKERROR CreateLIFOProto(CKBehaviorPrototype **);
int LIFO(const CKBehaviorContext &behcontext);

CKERROR LIFOCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorLIFODecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("LIFO");
    od->SetDescription("Last In First Out");
    /* rem:
    <SPAN CLASS=in>Unstack: </SPAN>resets the building block to its initial state ( and process the first signal in the LIFO list).<BR>
    <SPAN CLASS=in>In 1: </SPAN>Input signals to be managed (only if the 'Reset' has been activated).<BR>
    <SPAN CLASS=in>In 2: </SPAN>Input signals to be managed (only if the 'Reset' has been activated).<BR>
    <SPAN CLASS=in>...</SPAN><BR>
    <SPAN CLASS=in>In N: </SPAN>Input signals to be managed (only if the 'Reset' has been activated).<BR>
    <BR>
    <SPAN CLASS=out>Out Unstack: </SPAN>is activated when the building block has been reset.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>Output signals to be managed (only if signals 'In 1' succeed in crossing the behavior).<BR>
    <SPAN CLASS=out>...</SPAN><BR>
    <BR>
    When the 'Unstack' signal occurs, the last signal, stored in the LIFO list, is managed and is deleted from the LIFO list. Therefore, if 'Unstack' is activated once again, the ante-last signal is managed. Actually , this behavior repeats the sequence you play in the inverted order, step by step.<BR>
    You can add as many inputs as needed (number of outputs will automatically be equal to the number of inputs).<BR>
    Note also there's a priority order from up to down if 2 signals occur at same time.<BR>
    If you ever need to change the maximum size of Stack, go to the building block settings...<BR>
    <BR>
    For example load the LIFO.CMO in technical sample.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7d037385, 0x12d62259));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLIFOProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateLIFOProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("LIFO");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Unstack");
    proto->DeclareInput("In 1");
    proto->DeclareInput("In 2");

    proto->DeclareOutput("Out Unstack");
    proto->DeclareOutput("Out 1");
    proto->DeclareOutput("Out 2");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // "LIFO"
    proto->DeclareLocalParameter(NULL, CKPGUID_INT);     // "Stack Pointer"
    proto->DeclareSetting("Stack Size", CKPGUID_INT, "8");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LIFO);

    proto->SetBehaviorCallbackFct(LIFOCallBack);

    *pproto = proto;
    return CK_OK;
}

int LIFO(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int c = beh->GetInputCount();
    int c_out = beh->GetOutputCount();

    if (c != c_out)
        throw "Wrong number of Input/Output";

    int a;

    int *buf = (int *)beh->GetLocalParameterWriteDataPtr(0);

    int sp = 0;
    beh->GetLocalParameterValue(1, &sp);

    int max_size = 256;
    beh->GetLocalParameterValue(2, &max_size);

    // if we enter by another entry
    for (a = 1; a < c; a++)
    {
        if (beh->IsInputActive(a))
        {
            beh->ActivateInput(a, FALSE);
            if (sp < max_size)
            {
                buf[sp] = a;
                sp++;
                beh->SetLocalParameterValue(1, &sp);
            }
        }
    }

    // if we enter by 'Unstack'
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        if (sp)
        {
            sp--;
            beh->SetLocalParameterValue(1, &sp);
            a = buf[sp];
            beh->ActivateOutput(a);
        }
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR LIFOCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        int sp = 0;
        beh->SetLocalParameterValue(1, &sp);

        int max_size = 8;
        beh->GetLocalParameterValue(2, &max_size);

        int *buf = new int[max_size];
        beh->SetLocalParameterValue(0, buf, max_size * sizeof(int));
        delete[] buf;

        break;
    }

    case CKM_BEHAVIOREDITED:
    {
        int c = beh->GetInputCount();
        int c_out = beh->GetOutputCount();
        char out_str[10];

        while (c > c_out) // we must add 'Outputs'
        {
            sprintf(out_str, "out%d", c_out);
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
