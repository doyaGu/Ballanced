/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SynchroPulsar
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSynchroPulsarDecl();
CKERROR CreateSynchroPulsarProto(CKBehaviorPrototype **);
int SynchroPulsar(const CKBehaviorContext &context);
CKERROR SynchroPulsarCallBackObject(const CKBehaviorContext &behcontext);

// Inner struct
typedef struct
{
    float time;
    int current_line;
} A_SynchroPulsar;
//---

CKObjectDeclaration *FillBehaviorSynchroPulsarDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Synchro Player");
    od->SetDescription("Produce streaming events that match the one recorded with 'Synchro Recorder'.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the behavior.<BR>
    <BR>
    <SPAN CLASS=out>End: </SPAN>activated when synchro data array has been parsed entirely. NB: just loop the 'End' output with the 'On' input if you want to constantly loop the whole sequence.<BR>
    <SPAN CLASS=out>Impulse: </SPAN>activated each time the time a new synchro signal is found (refer to 'Synchro Recorder' building block documentation).<BR>
    <BR>
    <SPAN CLASS=pin>Synchro Array: </SPAN>data array which must has been produced with the 'Synchro Recorder' building block.<BR>
    Refer to the 'Synchro Recorder' building block documentation to see what must be the array format.<BR>
    <BR>
    <SPAN CLASS=pout>Signal Amplitude: </SPAN>amplitude value recorded in the data array for the current signal.<BR>
    <SPAN CLASS=pout>Length: </SPAN>length recorded in the data array for the current signal, ie: Duration of key pressing (in milliseconds).<BR>
    <BR>
    <SPAN CLASS=setting>Keep Activated During Delay:</SPAN>if checked, the output signal 'Impulse' will stay active during 'Length' time.<BR>
    <BR>
    <BR>
    Parse the synchronized data array, and activates the 'Impulse' every recorded synchronized time.<BR>
    In fact, all it does is repoducing the recorded sequence put in the data array with the 'Synchro Recorder' building block.<BR>
    See Also: "Synchro Recorder" BB.<BR>
    */
    /* warning:
    - Reaching the End of the array activates the 'End' output, and deactivates the building block process.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("Narratives/Streaming Recording");
    od->SetGuid(CKGUID(0x62da29d8, 0x1bbc6234));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSynchroPulsarProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSynchroPulsarProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Synchro Player");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("End");
    proto->DeclareOutput("Impulse");

    proto->DeclareInParameter("Synchro Array", CKPGUID_DATAARRAY, "Synchro Array");

    proto->DeclareOutParameter("Signal Amplitude", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Length", CKPGUID_FLOAT);

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // inner struct SynchroPulsar
    proto->DeclareSetting("Keep Activated During Delay", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SynchroPulsar);
    proto->SetBehaviorCallbackFct(SynchroPulsarCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR SynchroPulsarCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        A_SynchroPulsar *sp = new A_SynchroPulsar;
        beh->SetLocalParameterValue(0, &sp, sizeof(sp));
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        A_SynchroPulsar *sp = NULL;
        beh->GetLocalParameterValue(0, &sp);
        if (sp)
        {

            delete sp;
            sp = NULL;
        }
    }
    break;
    }

    return CKBR_OK;
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int SynchroPulsar(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKTimeManager *tm = ctx->GetTimeManager();

    CKDataArray *da = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!da)
        return CKBR_PARAMETERERROR;

    A_SynchroPulsar *sp;
    beh->GetLocalParameterValue(0, &sp);

    if (beh->IsInputActive(1))
    {
        // enter by OFF
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // enter by ON
        beh->ActivateInput(0, FALSE);

        // init inner variable
        sp->time = tm->GetAbsoluteTime();
        sp->current_line = 0;
    }

    float time = 0;
    da->GetElementValue(sp->current_line, 0, &time);

    if (tm->GetAbsoluteTime() >= sp->time + time)
    {
        if (sp->current_line == da->GetRowCount() - 1)
        {
            beh->ActivateOutput(0); // activate End Output
            return CKBR_OK;
        }

        float amplitude = 0.0f;
        da->GetElementValue(sp->current_line, 1, &amplitude);

        float delay = 0;
        da->GetElementValue(sp->current_line, 2, &delay);

        CKBOOL k = FALSE;
        beh->GetLocalParameterValue(1, &k);

        if (!k || (tm->GetAbsoluteTime() > sp->time + time + delay))
        {
            sp->current_line++;
        }

        beh->SetOutputParameterValue(0, &amplitude);
        beh->SetOutputParameterValue(1, &delay);

        beh->ActivateOutput(1); // activate Pulse Output
    }

    return CKBR_ACTIVATENEXTFRAME;
}
