/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            StreamingEvent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorStreamingEventDecl();
CKERROR CreateStreamingEventProto(CKBehaviorPrototype **);
int StreamingEventV1(const CKBehaviorContext &behcontext);
int StreamingEventV2(const CKBehaviorContext &behcontext);

CKERROR StreamingEventCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorStreamingEventDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Streaming Event");
    od->SetDescription("Activates outputs on the input activation change.");
    /* rem:
    <SPAN CLASS=in>In 0: </SPAN>triggers the building block process.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the building block process.<BR>
    <SPAN CLASS=in>...</SPAN>.<BR>
    <BR>
    <SPAN CLASS=out>Out 0: </SPAN>is activated when the building block has been activated by "In 0" (assuming the StreamingEvent level allows it).<BR>
    <SPAN CLASS=out>Out 1: </SPAN>is activated when the building block has been activated by "In 1" (assuming the StreamingEvent level allows it).<BR>
    <BR>
    You can add as many Inputs as needed (number of Outputs will automatically be equal to the number of Inputs).<BR>
    <BR>
    This building block allows you to detect streaming events, streaming changes. When you activate this building block
    several times by the "In 0", it will activate the corresponding output only the first time and not after.
    You can "Reset" this building block by using one of the input as the reset one (its output will not be used).<BR>
    */
    /* warning:
    - only one output can be activated at same time. If two inputs are activated at same time,
    the upper one (if it is not the last activate one) will be managed.<BR>
    - Trick: in some cases (above all in script deactivation and reactivation with reseting), you may need to reset the building block.
    i.e: make him forgot which input it memorize. To do so, as said above, you should use one of the input as a reset one.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1f0b52bf, 0x4c3342dd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateStreamingEventProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateStreamingEventProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Streaming Event");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("Out 0");
    proto->DeclareOutput("Out 1");

    proto->DeclareLocalParameter("", CKPGUID_INT, "0"); // last activated outputs
    proto->DeclareOutParameter("Last Activated Output", CKPGUID_INT, "-1");

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(StreamingEventV2);

    proto->SetBehaviorCallbackFct(StreamingEventCallBack);

    *pproto = proto;
    return CK_OK;
}

int StreamingEventV1(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int lao = -1;
    beh->GetOutputParameterValue(0, &lao);

    int i, count = beh->GetInputCount();
    for (i = 0; i < count; ++i)
    {
        if (beh->IsInputActive(i))
        {
            beh->ActivateInput(i, FALSE);
            if (lao != i)
            {
                lao = i;
                beh->SetOutputParameterValue(0, &lao);
                beh->ActivateOutput(i);
                break;
            }
        }
    }
    return CKBR_OK;
}

int StreamingEventV2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int lao = 0;
    beh->GetLocalParameterValue(0, &lao);

    int i, count = beh->GetInputCount();
    for (i = 0; i < count; ++i)
    {
        if (beh->IsInputActive(i))
        {
            beh->ActivateInput(i, FALSE);
            if (!(lao & (1 << i)))
            {
                lao |= (1 << i);
                beh->SetOutputParameterValue(0, &i);
                beh->ActivateOutput(i);
            }
        }
        else
        {
            lao &= ~(1 << i);
        }
    }

    beh->SetLocalParameterValue(0, &lao);
    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR StreamingEventCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        if (beh->GetVersion() < 0x00020000)
            beh->SetFunction(StreamingEventV1);
    }
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORRESET: // this event occures also when the script is activated whith a reset flag.
    {
        int lao = -1;
        beh->SetOutputParameterValue(0, &lao);
        lao = 0;
        beh->SetLocalParameterValue(0, &lao);
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
    }
    break;
    }

    return CKBR_OK;
}
