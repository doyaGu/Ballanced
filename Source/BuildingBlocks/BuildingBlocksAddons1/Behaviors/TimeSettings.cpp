/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		Time Settings
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_FRAMESYNC CKDEFINEGUID(0x5e4c1dcf, 0x662c4495)
#define CKPGUID_BEHAVSYNC CKDEFINEGUID(0x677f3843, 0x4e7a3fd0)

CKObjectDeclaration *FillBehaviorTimeSettings();
CKERROR CreateTimeSettingsProto(CKBehaviorPrototype **);
int TimeSettings(const CKBehaviorContext &behcontext);

enum {FRAME_PIN, FLIMIT_PIN, SCALE_PIN, BEHAV_PIN, BLIMIT_PIN, MINDELTA_PIN, MAXDELTA_PIN};

CKObjectDeclaration *FillBehaviorTimeSettingsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Time Settings");
    od->SetDescription("Set All Time Settings");
    od->SetCategory("Optimizations/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x60a0f8c, 0x9203b5f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTimeSettingsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTimeSettingsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Time Settings");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("out");

    proto->DeclareInParameter("Frame Rate", CKPGUID_FRAMESYNC, "2");
    proto->DeclareInParameter("Frame Limit Value", CKPGUID_INT, "60");
    proto->DeclareInParameter("Scale Time Value", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Behavior Rate", CKPGUID_BEHAVSYNC, "1");
    proto->DeclareInParameter("Behavior Limit Value", CKPGUID_INT, "60");
    proto->DeclareInParameter("Min Delta Time", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Max Delta Time", CKPGUID_FLOAT, "200");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TimeSettings);

    *pproto = proto;
    return CK_OK;
}

int TimeSettings(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    int frame, behav;
    float scale;
    int flimit, blimit;
    float minDelta, maxDelta;

    CKTimeManager *tm = behcontext.TimeManager;
    CK_FRAMERATE_LIMITS flim;
    CK_FRAMERATE_LIMITS blim;

    // pIn check out (if no pIn we get current values)
    if (!(beh->GetInputParameter(FRAME_PIN)->GetRealSource()))
    {
        flim = (CK_FRAMERATE_LIMITS)(tm->GetLimitOptions() & CK_FRAMERATE_MASK);
    }
    else
    {
        beh->GetInputParameterValue(FRAME_PIN, &frame);
    }
    if (!(beh->GetInputParameter(FLIMIT_PIN)->GetRealSource()))
    {
        flimit = (int)tm->GetFrameRateLimit();
    }
    else
    {
        beh->GetInputParameterValue(FLIMIT_PIN, &flimit);
    }
    flimit = XAbs(flimit);

    if (!(beh->GetInputParameter(SCALE_PIN)->GetRealSource()))
    {
        scale = tm->GetTimeScaleFactor();
    }
    else
    {
        beh->GetInputParameterValue(SCALE_PIN, &scale);
    }
    scale = XAbs(scale);

    if (!(beh->GetInputParameter(BLIMIT_PIN)->GetRealSource()))
    {
        blimit = (int)tm->GetBehavioralRateLimit();
    }
    else
    {
        beh->GetInputParameterValue(BLIMIT_PIN, &blimit);
    }
    blimit = XAbs(blimit);

    if (!(beh->GetInputParameter(MINDELTA_PIN)->GetRealSource()))
    {
        minDelta = tm->GetMinimumDeltaTime();
    }
    else
    {
        beh->GetInputParameterValue(MINDELTA_PIN, &minDelta);
    }
    minDelta = XAbs(minDelta);

    if (!(beh->GetInputParameter(MAXDELTA_PIN)->GetRealSource()))
    {
        maxDelta = tm->GetMaximumDeltaTime();
    }
    else
    {
        beh->GetInputParameterValue(MAXDELTA_PIN, &maxDelta);
    }
    maxDelta = XAbs(maxDelta);

    // Frame
    if (frame == 1)
        flim = CK_FRAMERATE_FREE;
    else if (frame == 2)
        flim = CK_FRAMERATE_SYNC;
    else
    {
        flim = CK_FRAMERATE_LIMIT;
        tm->SetFrameRateLimit((float)flimit);
    }
    tm->SetTimeScaleFactor(scale);

    // Behavior
    if (!(beh->GetInputParameter(BEHAV_PIN)->GetRealSource()))
    {
        blim = (CK_FRAMERATE_LIMITS)(tm->GetLimitOptions() & CK_BEHRATE_MASK);
    }
    else
    {
        beh->GetInputParameterValue(BEHAV_PIN, &behav);
        if (behav == 1)
            blim = CK_BEHRATE_SYNC;
        else
        {
            blim = CK_BEHRATE_LIMIT;
            tm->SetBehavioralRateLimit((float)blimit);
        }
    }

    tm->ChangeLimitOptions(flim, blim);
    tm->SetMinimumDeltaTime(minDelta);
    tm->SetMaximumDeltaTime(maxDelta);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    return CKBR_OK;
}
