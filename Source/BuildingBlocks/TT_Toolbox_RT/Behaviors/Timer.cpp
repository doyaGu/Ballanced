////////////////////////////
////////////////////////////
//
//        TT_Timer
//
////////////////////////////
////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"
#include "TimeManager.h"

CKObjectDeclaration *FillBehaviorTimerDecl();
CKERROR CreateTimerProto(CKBehaviorPrototype **pproto);
int Timer(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTimerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Timer");
    od->SetDescription("Chronometer (ON=Start, OFF=Stop).");
    od->SetCategory("TT Toolbox/Time");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6ac67901, 0x7d2a6059));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTimerProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTimerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Timer");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On/Reset");
    proto->DeclareInput("Pause");
    proto->DeclareInput("Play");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Pause");
    proto->DeclareOutput("Exit Play");
    proto->DeclareOutput("Exit Off");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Timer);

    *pproto = proto;
    return CK_OK;
}

int Timer(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    TimeManager *tm = TimeManager::GetManager(context);
    if (!tm)
        return CKBR_OWNERERROR;

    if (beh->IsInputActive(0))
    {
        tm->Reset();
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    if (beh->IsInputActive(1))
    {
        tm->Pause();
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        return CKBR_ACTIVATENEXTFRAME;
    }

    if (beh->IsInputActive(2))
    {
        tm->Play();
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(2, TRUE);
        return CKBR_ACTIVATENEXTFRAME;
    }

    if (beh->IsInputActive(3))
    {
        tm->Reset();
        beh->ActivateInput(3, FALSE);
        beh->ActivateOutput(3, TRUE);
        return CKBR_OK;
    }

    float elapsedTime = tm->Now() + behcontext.DeltaTime;
    tm->Elapse(behcontext.DeltaTime);
    beh->SetOutputParameterValue(0, &elapsedTime);
    return CKBR_ACTIVATENEXTFRAME;
}