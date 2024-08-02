//////////////////////////////////
//////////////////////////////////
//
//        TT PushButton2
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorPushButton2Decl();
CKERROR CreatePushButton2Proto(CKBehaviorPrototype **pproto);
int PushButton2(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPushButton2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PushButton2");
    od->SetDescription("Push button without material changes");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x14d325d1, 0x6748654e));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePushButton2Proto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreatePushButton2Proto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PushButton2");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Released");
    proto->DeclareOutput("Roll Over");
    proto->DeclareOutput("Mouse Down");

    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PushButton2);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

#define RELEASED 0
#define ROLLOVER 1
#define PRESSED  2
#define PUSHED   3

int PushButton2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK2dEntity *target = (CK2dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    int state = 0;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->SetLocalParameterValue(0, &state);
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    int oldState = 0;
    beh->GetLocalParameterValue(0, &oldState);

    CKInputManager *im = (CKInputManager *)context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!im)
        return CKBR_ACTIVATENEXTFRAME;

    CKRenderContext *rc = context->GetPlayerRenderContext();
    if (!rc)
        return CKBR_ACTIVATENEXTFRAME;

    Vx2DVector mousePos;
    im->GetMousePosition(mousePos, FALSE);

    VxRect screen;
    rc->GetWindowRect(screen, FALSE);
    mousePos += screen.GetTopLeft();

    CK2dEntity *picked = rc->Pick2D(mousePos);
    if (target == picked && im->GetCursorVisibility())
        state |= ROLLOVER;

    if (im->IsMouseButtonDown(CK_MOUSEBUTTON_LEFT))
        state |= PRESSED;

    if (state != oldState)
    {
        beh->SetLocalParameterValue(0, &state);

        if ((oldState & ROLLOVER) && !(state & ROLLOVER))
            beh->ActivateOutput(RELEASED);

        if (!(oldState & ROLLOVER) && (state & ROLLOVER))
            beh->ActivateOutput(ROLLOVER);

        if ((oldState == PUSHED) && (state == ROLLOVER))
            beh->ActivateOutput(PRESSED);
    }

    return CKBR_ACTIVATENEXTFRAME;
}