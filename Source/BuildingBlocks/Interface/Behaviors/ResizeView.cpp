/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ResizeView
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorResizeViewDecl();
CKERROR CreateResizeViewProto(CKBehaviorPrototype **);
int ResizeView(const CKBehaviorContext &behcontext);
CKERROR ResizeViewCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorResizeViewDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Resize View");
    od->SetDescription("Allows main view to be resized.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the behavior's process is started.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the behavior's process is stopped.<BR>
    <BR>
    <SPAN CLASS=pin>View Region: </SPAN>The screen region defined by a rectangle the main view will be resized.<BR>
    <BR>
    */
    od->SetCategory("Interface/Screen");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x40e1fe8, 0x79ed7c9b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateResizeViewProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateResizeViewProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Resize View");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("View Region", CKPGUID_RECT, "(0,0),(100,60)");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ResizeView);
    proto->SetBehaviorCallbackFct(ResizeViewCallback);

    *pproto = (CKBehaviorPrototype *)proto;
    return CK_OK;
}

void ResizeOn(CKBehavior *beh)
{
    CKContext *ctx = beh->GetCKContext();

    // Inputs
    VxRect rect;
    beh->GetInputParameterValue(0, &rect);

    // View Rect
    CKRenderContext *dev = ctx->GetPlayerRenderContext();
    dev->SetViewRect(rect);

    CKDWORD ro = dev->GetCurrentRenderOptions();
    dev->SetCurrentRenderOptions(ro & ~CK_RENDER_USECAMERARATIO);
}

void ResizeOff(CKContext *ctx)
{
    CKRenderContext *dev = ctx->GetPlayerRenderContext();
    if (dev)
    {
        VxRect rect;
        dev->GetWindowRect(rect);
        dev->SetViewRect(rect);

        CKDWORD ro = dev->GetCurrentRenderOptions();
        dev->SetCurrentRenderOptions(ro | CK_RENDER_USECAMERARATIO);
        dev->ForceCameraSettingsUpdate();
    }
}

int ResizeView(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // we enter by 'OFF'
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        ResizeOff(behcontext.Context);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0)) // we enter by 'ON'
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        ResizeOn(beh);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR ResizeViewCallback(const CKBehaviorContext &behcontext)
{
    CKContext *ctx = behcontext.Context;
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORNEWSCENE:
    {
        if (ctx->IsPlaying() && beh->IsActive() && beh->IsParentScriptActiveInScene(ctx->GetCurrentScene()))
        {
            ResizeOn(beh);
        }
        else
        {
            ResizeOff(ctx);
        }
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        ResizeOff(ctx);
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        if (beh->IsActive())
        {
            ResizeOff(ctx);
        }
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
        if (beh->IsActive() && beh->IsParentScriptActiveInScene(ctx->GetCurrentScene()))
        {
            ResizeOn(beh);
        }
    }
    break;
    }
    return CKBR_OK;
}
