//////////////////////////////
//////////////////////////////
//
//        TT_Counter
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorCounterDecl();
CKERROR CreateCounterProto(CKBehaviorPrototype **pproto);
int Counter(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCounterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Counter");
    od->SetDescription("Counts forward and backward with loop");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x750f20fc, 0x3741186d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Klaus");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCounterProto);
    od->SetCompatibleClassId(CKCID_OBJECT);
    return od;
}

CKERROR CreateCounterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Counter");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Initialize");
    proto->DeclareInput("Count Up");
    proto->DeclareInput("Count Down");

    proto->DeclareOutput("Initialize OUT");
    proto->DeclareOutput("Count Up OUT");
    proto->DeclareOutput("Count Down OUT");

    proto->DeclareInParameter("Start", CKPGUID_INT, "0");
    proto->DeclareInParameter("Min", CKPGUID_INT, "0");
    proto->DeclareInParameter("Max", CKPGUID_INT, "9");

    proto->DeclareOutParameter("current count", CKPGUID_INT);

    proto->DeclareLocalParameter("Count", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Counter);

    *pproto = proto;
    return CK_OK;
}

int Counter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int start;
    beh->GetInputParameterValue(0, &start);
    int min;
    beh->GetInputParameterValue(1, &min);
    int max;
    beh->GetInputParameterValue(2, &max);
    int count;
    beh->GetLocalParameterValue(0, &count);

    if (start < min || start > max)
    {
        start = min;
        context->OutputToConsoleExBeep("TT_Counter: Start value invalid!");
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        count = start;
        beh->ActivateOutput(0, TRUE);
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        ++count;
        if (count > max)
            count = min;
        beh->ActivateOutput(1, TRUE);
    }

    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);
        --count;
        if (count < min)
            count = max;
        beh->ActivateOutput(2, TRUE);
    }

    beh->SetLocalParameterValue(0, &count);
    beh->SetOutputParameterValue(0, &count);

    return CKBR_OK;
}