/////////////////////////////////
/////////////////////////////////
//
//        TT PushButton
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorPushButtonDecl();
CKERROR CreatePushButtonProto(CKBehaviorPrototype **pproto);
int PushButton(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPushButtonDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT PushButton");
    od->SetDescription("Transforms a 2D Frame into a push button.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4df00dac, 0x562c27da));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePushButtonProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreatePushButtonProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT PushButton");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Released");
    proto->DeclareOutput("Roll Over");
    proto->DeclareOutput("Mouse Down");
    proto->DeclareOutput("Mouse Up");

    proto->DeclareInParameter("Released Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Pressed Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("RollOver Material", CKPGUID_MATERIAL);

    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PushButton);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int PushButton(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK2dEntity *target = (CK2dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_ACTIVATENEXTFRAME;

    // Handle On input
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        int state = 0;
        beh->SetLocalParameterValue(0, &state);
        CKBOOL mouseDown = FALSE;
        beh->SetLocalParameterValue(1, &mouseDown);
    }

    // Handle Off input
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    // Get input manager
    CKInputManager *inputManager = (CKInputManager *)ctx->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!inputManager)
        return CKBR_ACTIVATENEXTFRAME;

    // Get render context
    CKRenderContext *renderCtx = ctx->GetPlayerRenderContext();
    if (!renderCtx)
        return CKBR_ACTIVATENEXTFRAME;

    int lastState = 0;
    int newState = 0;
    CKBOOL mouseDown = FALSE;
    int wasMouseDown = 0;
    CKBOOL mouseStateChanged = FALSE;

    // Get mouse position
    Vx2DVector mousePos;
    inputManager->GetMousePosition(mousePos, FALSE);

    // Get window rect and adjust mouse position
    VxRect windowRect;
    renderCtx->GetWindowRect(windowRect);
    mousePos.x += windowRect.left;
    mousePos.y += windowRect.top;

    // Pick under mouse
    CKRenderObject *pickedObject = renderCtx->Pick2D(mousePos);

    // Check if mouse is over target
    int isOver = 0;
    if ((CK2dEntity *)pickedObject == target)
        isOver = 1;

    // Check mouse button state
    mouseDown = inputManager->IsMouseButtonDown(CK_MOUSEBUTTON_LEFT);

    // Get previous states
    beh->GetLocalParameterValue(0, &lastState);
    beh->GetLocalParameterValue(1, &wasMouseDown);

    // Update over state
    if (isOver)
        newState |= 1;

    // Track mouse button state changes
    if (mouseDown)
    {
        if (!wasMouseDown)
        {
            wasMouseDown = 1;
            beh->SetLocalParameterValue(1, &wasMouseDown);
            mouseStateChanged = TRUE;
        }
    }
    else
    {
        if (wasMouseDown)
        {
            wasMouseDown = 0;
            beh->SetLocalParameterValue(1, &wasMouseDown);
            mouseStateChanged = TRUE;
        }
    }

    // Check for state changes
    if (newState != lastState)
    {
        beh->SetLocalParameterValue(0, &newState);

        // Was over, now not over - Released
        if ((lastState & 1) && !(newState & 1))
        {
            CKMaterial *releasedMat = (CKMaterial *)beh->GetInputParameterObject(0);
            target->SetMaterial(releasedMat);
            beh->ActivateOutput(0, TRUE);
        }

        // Was not over, now over - Roll Over
        if (!(lastState & 1) && (newState & 1))
        {
            CKMaterial *rollOverMat = (CKMaterial *)beh->GetInputParameterObject(2);
            target->SetMaterial(rollOverMat);
            beh->ActivateOutput(1, TRUE);
        }
    }

    // Check for mouse button events while over the button
    if (mouseStateChanged && newState == 1)
    {
        if (mouseDown)
        {
            // Mouse down
            CKMaterial *pressedMat = (CKMaterial *)beh->GetInputParameterObject(1);
            target->SetMaterial(pressedMat);
            beh->ActivateOutput(2, TRUE);
            return CKBR_ACTIVATENEXTFRAME;
        }
        else
        {
            // Mouse up
            CKMaterial *rollOverMat = (CKMaterial *)beh->GetInputParameterObject(2);
            target->SetMaterial(rollOverMat);
            beh->ActivateOutput(3, TRUE);
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}