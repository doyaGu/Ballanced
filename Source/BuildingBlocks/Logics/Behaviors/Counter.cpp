/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Counter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCounterDecl();
CKERROR CreateCounterProto(CKBehaviorPrototype **);
int Counter(const CKBehaviorContext &behcontext);
int CounterV2(const CKBehaviorContext &behcontext);
CKERROR CounterCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorCounterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Counter");
    od->SetDescription("Exits 'MaxCount' times by the 'Looping' output and when finished, exits by 'Out' output.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Max Count: </SPAN>number of steps in the count loop.<BR>
    <SPAN CLASS=pin>Start index: </SPAN>index at which the counter will start (by default 1).<BR>
    <SPAN CLASS=pin>Step: </SPAN>number to add to the counter at each loop (by default 1).<BR>
    <BR>
    <SPAN CLASS=pout>CurrentCount: </SPAN>current step in the count loop.<BR>
    <BR>
    Use this to execute a task 'Maxcount' times.<BR>
    The "In" input resets the current count to 0.<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x998f000f, 0xf000f899));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateCounterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCounterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Counter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Count", CKPGUID_INT, "10");
    proto->DeclareInParameter("Start Index", CKPGUID_INT, "1");
    proto->DeclareInParameter("Step", CKPGUID_INT, "1");

    proto->DeclareOutParameter("Value", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CounterV2);
    proto->SetBehaviorCallbackFct(CounterCallBack);

    *pproto = proto;
    return CK_OK;
}

int Counter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int maxcount = 10;
    int current;

    int step = 1;
    beh->GetInputParameterValue(2, &step);

    int startindex = 1;
    beh->GetInputParameterValue(1, &startindex);

    if (beh->IsInputActive(0))
    { // if we get in by the "In" IO ...
        current = startindex;
        beh->ActivateInput(0, FALSE);
    }
    else
    { // else, if we get in by "Loop In" IO ..
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &current);

        current += step;
    }

    beh->GetInputParameterValue(0, &maxcount);

    // test if finished
    if (step > 0)
    {
        if (current > maxcount + (startindex - 1))
        {
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
        else
            beh->ActivateOutput(1);
    }
    else
    {
        if ((startindex - current) >= maxcount * -step)
        {
            beh->ActivateOutput(0);
            return CKBR_OK;
        }
        else
            beh->ActivateOutput(1);
    }

    beh->SetOutputParameterValue(0, &current);

    return CKBR_OK;
}

int CounterV2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKParameterOut *pout = (CKParameterOut *)beh->GetOutputParameter(0);

    int maxcount = 10;
    beh->GetInputParameterValue(0, &maxcount);

    int startindex = 1;
    beh->GetInputParameterValue(1, &startindex);

    int step = 1;
    beh->GetInputParameterValue(2, &step);
    if (!step)
        ++step;

    int &currentValue = *(int *)pout->GetWriteDataPtr();

    if (beh->IsInputActive(0))
    { // if we get in by the "In" IO ...
        beh->ActivateInput(0, FALSE);
        currentValue = startindex - step;
    }
    else
    { // entered by loop in
        beh->ActivateInput(1, FALSE);
    }

    // Progression
    currentValue += step;

    // Update the parameter out
    pout->DataChanged();

    int count = (currentValue - startindex) / step;
    if (count >= maxcount)
    { // finish
        beh->ActivateOutput(0);
    }
    else
    { // not yet finish
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

CKERROR CounterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        if (beh->GetVersion() == 0x00020000)
        {
            beh->SetFunction(CounterV2);
        }
        else
        {
            beh->SetFunction(Counter);
        }
    }
    break;
    }

    return CKBR_OK;
}
