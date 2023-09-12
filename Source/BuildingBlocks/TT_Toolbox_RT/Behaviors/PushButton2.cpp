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

int PushButton2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK2dEntity *target = (CK2dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        int state = 0;
        beh->SetLocalParameterValue(0, &state);
        CKBOOL mouseDown = FALSE;
        beh->SetLocalParameterValue(1, &mouseDown);
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CKInputManager *im = (CKInputManager *)context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!im)
        return CKBR_ACTIVATENEXTFRAME;

    CKRenderContext *dev = context->GetPlayerRenderContext();
    if (!dev)
        return CKBR_ACTIVATENEXTFRAME;

    Vx2DVector mousePos;
    im->GetMousePosition(mousePos, FALSE);

    VxRect winRect;
    dev->GetWindowRect(winRect, FALSE);

    CKBOOL mouseStateChanged = FALSE;
    int state = 0;
    int oldState = 0;
    beh->GetLocalParameterValue(0, &oldState);
    CKBOOL mouseDown = FALSE;
    beh->GetLocalParameterValue(1, &mouseDown);

    CK2dEntity *picked = dev->Pick2D(mousePos);
    if (target == picked && im->GetCursorVisibility())
        state |= 1;

    CKBOOL isClicked = im->IsMouseButtonDown(CK_MOUSEBUTTON_LEFT);
    if (isClicked && !mouseDown)
    {
        mouseDown = TRUE;
        beh->SetLocalParameterValue(1, &mouseDown);
        mouseStateChanged = TRUE;
    }
    else if (!isClicked && mouseDown)
    {
        mouseDown = FALSE;
        beh->SetLocalParameterValue(1, &mouseDown);
        mouseStateChanged = TRUE;
    }

    if (state != oldState)
    {
        beh->SetLocalParameterValue(0, &state);
        if ((oldState & 1) == 0 && (state & 1) != 0)
            beh->ActivateOutput(1, TRUE);
        else if ((oldState & 1) != 0 && (state & 1) == 0)
            beh->ActivateOutput(0, TRUE);
    }

    if (mouseStateChanged && state == 1 && isClicked)
        beh->ActivateOutput(2, TRUE);

    return CKBR_OK;
}