/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AllButOne
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAllButOneDecl();
CKERROR CreateAllButOneProto(CKBehaviorPrototype **);
int AllButOne(const CKBehaviorContext &behcontext);

CKERROR AllButOneCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorAllButOneDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("All But One");
    od->SetDescription("Activates all the Outputs but not the one corresponding to the activated input.");
    /* rem:
    <SPAN CLASS=in>In 0: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is activated if we enter by any Input but the 'In 0'.<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated if we enter by any Input but the 'In 1'.<BR>
    <BR>
    Activates all the Outputs but not the one corresponding to the activated input.<BR>
    <BR>
    */
    /* warning:
    - You can add as many inputs as needed (number of outputs will automatically be equal to the number of inputs).<BR>
      - Note also, there's a priority order from up to down if 2 signals occurs at same time.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7ebc16ce, 0x75ba1301));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAllButOneProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateAllButOneProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("All But One");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("Out 0");
    proto->DeclareOutput("Out 1");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AllButOne);

    proto->SetBehaviorCallbackFct(AllButOneCallBack);

    *pproto = proto;
    return CK_OK;
}

int AllButOne(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int c = beh->GetInputCount();
    int c_out = beh->GetOutputCount();

    int a, b;

    for (a = 0; a < c; a++)
    {
        if (beh->IsInputActive(a))
        {
            beh->ActivateInput(a, FALSE);
            for (b = 0; b < a; b++)
                beh->ActivateOutput(b);
            for (b = a + 1; b < c; b++)
                beh->ActivateOutput(b);
        }
    }
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR AllButOneCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIOREDITED:
    {
        int c = beh->GetInputCount();
        int c_out = beh->GetOutputCount();
        char out_str[10];

        while (c > c_out)
        { // we must add 'Outputs'
            sprintf(out_str, "Out %d", c_out);
            beh->AddOutput(out_str);
            c_out++;
        }

        while (c < c_out)
        { // we must remove 'Outputs'
            c_out--;
            beh->DeleteOutput(c_out);
        }

        break;
    }
    }

    return CKBR_OK;
}
