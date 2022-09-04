////////////////////////////
////////////////////////////
//
//        TT_Timer
//
////////////////////////////
////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

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
    od->SetAuthorGuid(CKGUID(0x53c80889, 0x57f84916));
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
    // TODO: To be finished.
    return CKBR_OK;
}