/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CameraView
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCameraViewDecl();
CKERROR CreateCameraViewProto(CKBehaviorPrototype **);
int CameraView(const CKBehaviorContext &context);
void CameraViewRender(CKRenderContext *dev, void *arg);

CKObjectDeclaration *FillBehaviorCameraViewDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Additional View");
    od->SetDescription("Allows additional views to be displayed on screen simultaneously.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the behavior's process is started.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the behavior's process is stopped.<BR>
    <BR>
    <SPAN CLASS=pin>View Region: </SPAN>the screen region defined by a rectangle in which the render will be done.<BR>
    <SPAN CLASS=pin>Render Options: </SPAN>options of rendering.<BR>
    <SPAN CLASS=pin>Overwrite 2D: </SPAN>if TRUE, renders the view above every 2d content.<BR>
    <BR>
    */
    od->SetCategory("Interface/Screen");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00cd159b, 0x164d010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCameraViewProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateCameraViewProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Additional View");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("View Region", CKPGUID_RECT, "(0,0),(100,60)");
    proto->DeclareInParameter("Render Options", CKPGUID_RENDEROPTIONS, "Background Sprites,Clear ZBuffer,Clear Back Buffer");
    proto->DeclareInParameter("Overwrite 2D", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CameraView);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int CameraView(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (!beh)
        return CKBR_BEHAVIORERROR;

    if (beh->IsInputActive(1)) // we enter by 'OFF'
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0)) // we enter by 'ON'
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    CKRenderContext *rcontext = ctx->GetPlayerRenderContext();
    if (rcontext)
    {
        CKBOOL o2D = TRUE;
        beh->GetInputParameterValue(2, &o2D);
        if (o2D)
            rcontext->AddPostSpriteRenderCallBack(CameraViewRender, (void *)beh->GetID(), TRUE);
        else
            rcontext->AddPostRenderCallBack(CameraViewRender, (void *)beh->GetID(), TRUE);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

void CameraViewRender(CKRenderContext *dev, void *arg)
{
    CKContext *ctx = dev->GetCKContext();

    CKBehavior *beh = (CKBehavior *)CKGetObject(dev->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;
    // CKBehavior* beh = (CKBehavior*)arg;

    // Prevent Infinite loop
    CKBOOL infunction = (CKBOOL)beh->GetAppData();
    if (infunction)
        return;
    beh->SetAppData((void *)TRUE);

    // we get the input parameters
    CKCamera *camera = (CKCamera *)beh->GetTarget();

    // Inputs
    VxRect rect;
    beh->GetInputParameterValue(0, &rect);

    CKDWORD rop = 0;
    beh->GetInputParameterValue(1, &rop);

    CKRenderContext *rcontext = ctx->GetPlayerRenderContext();

    // Save Options
    CKCamera *oldcam = rcontext->GetAttachedCamera();
    VxRect oldrect;
    rcontext->GetViewRect(oldrect);

    // Prepare Rendering
    rcontext->AttachViewpointToCamera(camera);
    rcontext->SetViewRect(rect);

    ///
    // Rendering
    rcontext->Render((CK_RENDER_FLAGS)(rop | CK_RENDER_CLEARVIEWPORT | CK_RENDER_DONOTUPDATEEXTENTS));

    // Bool for prevent from recursivity
    beh->SetAppData(FALSE);

    // Restore Options
    rcontext->AttachViewpointToCamera(oldcam);
    rcontext->ForceCameraSettingsUpdate();
    rcontext->SetViewRect(oldrect);
}
