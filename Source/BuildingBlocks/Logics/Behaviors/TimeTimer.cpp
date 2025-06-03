/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TimeTimer
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTimeTimerDecl();
CKERROR CreateTimeTimerProto(CKBehaviorPrototype **);
int TimeTimer(const CKBehaviorContext &behcontext);
int TimeTimerOld(const CKBehaviorContext &behcontext);
CKERROR TimeTimerCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTimeTimerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Timer");
    od->SetDescription("Waits (throw looping) for a given time to has elapsed.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Duration: </SPAN>how long the whole process should last.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>current time elapsed.<BR>
    Exit by 'Out' if the given time to wait has elapsed, else exit by 'Loop Out'; You have to enter by 'In' to trigger (and reset) the time process.<BR>
    <SPAN CLASS=pout>Delta Time: </SPAN>time variation between two behavioral process.<BR>
    */
    /* warning:
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    <BR>
    See Also: "Delay" BB, "Chrono" BB.<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xa2a5a63a, 0xe4e7e8e5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010005);
    od->SetCreationFunction(CreateTimeTimerProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTimeTimerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Timer");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Duration", CKPGUID_TIME, "0m 3s 0ms");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareOutParameter("Delta Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TimeTimer);
    proto->SetBehaviorCallbackFct(TimeTimerCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/* TimeTimer (TIME Based new version (float)) */
/*******************************************************/
int TimeTimer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float old_elapsed = 0;

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        beh->SetOutputParameterValue(0, &old_elapsed);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &old_elapsed);
    }

    float timetowait = 0;
    beh->GetInputParameterValue(0, &timetowait);

    float elapsed = old_elapsed + behcontext.DeltaTime;

    beh->SetOutputParameterValue(0, &elapsed);

    beh->SetOutputParameterValue(1, &behcontext.DeltaTime);

    if (elapsed >= timetowait) // finish
    {
        beh->ActivateOutput(0);
    }
    else // not yet ...
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

/*******************************************************/
/*  TimeTimer (TIME Based old version (int))  */
/*******************************************************/
int TimeTimerOld(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int old_elapsed = 0;

    if (beh->IsInputActive(0)) // if we get in by the "In" IO ...
    {
        beh->ActivateInput(0, FALSE);
        beh->SetOutputParameterValue(0, &old_elapsed);
    }
    else // else, if we get in by "Loop In" IO ..
    {
        beh->ActivateInput(1, FALSE);
        beh->GetOutputParameterValue(0, &old_elapsed);
    }

    int timetowait = 0;
    beh->GetInputParameterValue(0, &timetowait);

    int elapsed = old_elapsed + (int)behcontext.DeltaTime;

    beh->SetOutputParameterValue(0, &elapsed);

    int deltatime_int = (int)behcontext.DeltaTime;
    beh->SetOutputParameterValue(1, &deltatime_int);

    if (elapsed >= timetowait) // finish
    {
        beh->ActivateOutput(0);
    }
    else // not yet ...
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR TimeTimerCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
        //--- PATCH TIME PURPOSE
    case CKM_BEHAVIORLOAD:
    {
        CKParameterIn *pin = beh->GetInputParameter(0);

        CKGUID currentGuid = pin->GetGUID();
        CKBOOL isOldVersion = (beh->GetVersion() < 0x00010005);

        if (isOldVersion && (currentGuid == CKPGUID_INT))
        {
            beh->SetFunction(TimeTimerOld);
        }
        else
        {
            beh->SetFunction(TimeTimer);
            if (isOldVersion)
                beh->SetVersion(0x00010005);
        }
    }
    }

    return CKBR_OK;
}
