/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TimerMini
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTimerMiniDecl();
CKERROR CreateTimerMiniProto(CKBehaviorPrototype **);
int TimerMini(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTimerMiniDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Delayer");
    od->SetDescription("Waits for a given time to elapse using internal looping.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the time is elapsed.<BR>
    <BR>
    <SPAN CLASS=pin>Time to Wait: </SPAN>time to wait.<BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>time elapsed.<BR>
    <BR>
    This building block stay internally activated until the given lapse is finished. It just delay the activation of its output.<BR>
    <BR>
    See Also: "Timer".<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x15d472a5, 0x3bea409f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateTimerMiniProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTimerMiniProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Delayer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Time to Wait", CKPGUID_TIME, "0m 3s 0ms");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TimerMini);

    *pproto = proto;
    return CK_OK;
}

int TimerMini(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float old_elapsed = 0;

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        beh->SetOutputParameterValue(0, &old_elapsed);
    }

    float maxcount = 0;
    beh->GetInputParameterValue(0, &maxcount);

    beh->GetOutputParameterValue(0, &old_elapsed);
    float elapsed = old_elapsed + behcontext.DeltaTime;

    beh->SetOutputParameterValue(0, &elapsed);

    if (elapsed >= maxcount) // finish
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
