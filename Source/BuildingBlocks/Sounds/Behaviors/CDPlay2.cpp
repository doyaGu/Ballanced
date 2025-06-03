/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CDPlayer2
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#ifdef WIN32
#include "ACD.h"

CKERROR CreateCDPlayer2BehaviorProto(CKBehaviorPrototype **pproto);
int CDPlayer2(const CKBehaviorContext &behcontext);
CKERROR CallbackCDPlayer2(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCDPlayer2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("CD Player");
    od->SetDescription("Plays soundtracks from an audio CD (v2).");
    /* rem:
    <SPAN CLASS=in>Play: </SPAN>starts the CD Player.<BR>
    <SPAN CLASS=in>Stop: </SPAN>stops the CD Player.<BR>
    <SPAN CLASS=in>Pause/Resume: </SPAN>pauses or resumes playback.<BR>
    <BR>
    <SPAN CLASS=pin>First track: </SPAN>number of the first track to be played.<BR>
    <SPAN CLASS=pin>Last track: </SPAN>number of the last track to be played.<BR>
    <SPAN CLASS=pin>Starting Time: </SPAN>time on the first track when playback should start.<BR>
    <SPAN CLASS=pin>Ending Time: </SPAN>time before the end of the last track when playback should end.<BR>
    <BR>
    The first track of a CD is numbered 1, and not 0 (zero).<BR>
    To play track 1, from beginning to end, the parameters would be as follows:<BR>
    First Track = 1<BR>
    Last Track = 1<BR>
    Starting Time = 0<BR>
    Ending Time = 0<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3dec4ae1, 0x72ae2cd6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCDPlayer2BehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Sounds/Audio CD");
    od->NeedManager(SOUND_MANAGER_GUID);

    return od;
}

/*****************************************************/
/*                     PROTO                         */
/*****************************************************/
CKERROR CreateCDPlayer2BehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("CD Player");
    if (!proto)
        return CKERR_OUTOFMEMORY;
    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);

    proto->DeclareInput("Play");
    proto->DeclareInput("Stop");
    proto->DeclareInput("Pause/Resume");

    proto->DeclareInParameter("First Track", CKPGUID_INT, "1");
    proto->DeclareInParameter("Last Track", CKPGUID_INT, "1");
    proto->DeclareInParameter("Starting Time", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareInParameter("Ending Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // ACD inner struct

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CDPlayer2);
    proto->SetBehaviorCallbackFct(CallbackCDPlayer2);

    *pproto = proto;
    return CK_OK;
}

/*****************************************************/
/*                  CALLBACK                         */
/*****************************************************/
CKERROR CallbackCDPlayer2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (behcontext.CallbackMessage == CKM_BEHAVIORATTACH || behcontext.CallbackMessage == CKM_BEHAVIORLOAD)
    {
        ACD acd;
        beh->SetLocalParameterValue(0, &acd, sizeof(acd));

        return CKBR_OK;
    }

    ACD *acd = (ACD *)beh->GetLocalParameterReadDataPtr(0);
    if (!acd)
        CKBR_PARAMETERERROR;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
        acd->Stop();
        acd->CloseDevice();
    }
    break;

    case CKM_BEHAVIORPAUSE:
    {
        acd->Pause();
    }
    break;

    case CKM_BEHAVIORRESUME:
    {
        acd->Pause();
    }
    break;
    }

    return CKBR_OK;
}

/*****************************************************/
/*             BEHAVIOR FUNCTION                     */
/*****************************************************/
int CDPlayer2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    //____________________________/ Get CD Inner struct
    ACD *acd = (ACD *)beh->GetLocalParameterReadDataPtr(0);
    if (!acd)
        return CKBR_OK;

    //____________________________/ PLAY
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);

        // read input parameters
        acd->first_track = acd->end_track = 1;
        beh->GetInputParameterValue(0, &acd->first_track); // First Track
        beh->GetInputParameterValue(1, &acd->end_track);   // Get the 'Last Track'

        // back compatibility purpose
        CKParameterIn *pin = beh->GetInputParameter(2);
        if (pin->GetGUID() == CKPGUID_INT)
        { // old version (INT for times)
            acd->start_time = acd->end_time = 0;
            beh->GetInputParameterValue(2, &acd->start_time); // Starting Time
            beh->GetInputParameterValue(3, &acd->end_time);   // Ending Time
        }
        else
        { // new version (CKPGUID_TIME for times)
            float start_timef = 0, end_timef = 0;
            beh->GetInputParameterValue(2, &start_timef); // Starting Time
            beh->GetInputParameterValue(3, &end_timef);   // Ending Time
            acd->start_time = (int)start_timef;
            acd->end_time = (int)end_timef;
        }

        // play
        acd->Play();
    }

    //____________________________/ STOP
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        acd->Stop();
        acd->CloseDevice();
    }

    //____________________________/ PAUSE
    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);

        acd->Pause();
    }

    return CKBR_OK;
}

#endif
