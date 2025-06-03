/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DisplayProgressionBar
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDisplayProgressionBarDecl();
CKERROR CreateDisplayProgressionBarProto(CKBehaviorPrototype **pproto);
int DisplayProgressionBar(const CKBehaviorContext &behcontext);
CKERROR DisplayProgressionBarCallBack(const CKBehaviorContext &behcontext);
void ProgressionBarPostRender(CKRenderContext *rc, void *arg);

CKObjectDeclaration *FillBehaviorDisplayProgressionBarDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Display Progression Bar");
    od->SetDescription("Displays a progression bar.");
    /* rem:
    <SPAN CLASS=in>On : </SPAN>Activates the Progression Bar.<BR>
    <SPAN CLASS=in>Off : </SPAN>Deactivates the Progression Bar.<BR>
    <BR>
    <SPAN CLASS=out>Exit On : </SPAN>This output is activated after entering by 'On'.<BR>
    <SPAN CLASS=out>Exit Off : </SPAN>This output is activated after entering by 'Off'.<BR>
    <BR>
    <SPAN CLASS=pin>X : </SPAN>Horizontal Position of the first Progression Bar.<BR>
    <SPAN CLASS=pin>Y : </SPAN>Vertical Position of the first Progression Bar.<BR>
    <SPAN CLASS=pin>Bar Progression : </SPAN>Progression of the Bar in percentage (between 0 and 100%).<BR>
    <BR>
    */
    /* warning:
    - Go to the Settings of the 'Display Progression Bar' building block to specify which sprites to use to represent the 'Empty Bar' (the container) and the 'Full Bar' (the content).
    The 'Empty Bar' will allways be displayed under the 'Full Bar', and the 'Full Bar' will be resize according to the score.
    - You don't have to specify an 'Empty Bar' if you don't need one.
    - The corresponding sprites must be loaded in the system memory (you don't have to load them in the card memory like the other textures). To do so Drag'n Drop the images in the 3d view with the Shift Key.<BR>
    - This building block should only be used to display horizontal progression bar.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4cf37cd, 0x7a246b85));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDisplayProgressionBarProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/2D");
    return od;
}

CKERROR CreateDisplayProgressionBarProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Display Progression Bar");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("X", CKPGUID_INT, "0");
    proto->DeclareInParameter("Y", CKPGUID_INT, "0");
    proto->DeclareInParameter("Bar Progression", CKPGUID_PERCENTAGE, "100");

    proto->DeclareSetting("Empty Bar ()", CKPGUID_SPRITE);
    proto->DeclareSetting("Full Bar ()", CKPGUID_SPRITE);

    proto->DeclareLocalParameter("Active", CKPGUID_BOOL, "FALSE");

    proto->SetFunction(DisplayProgressionBar);
    proto->SetBehaviorCallbackFct(DisplayProgressionBarCallBack);

    *pproto = proto;
    return CK_OK;
}

int DisplayProgressionBar(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // WE GET BY THE OFF INPUT
        behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(ProgressionBarPostRender, beh);
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(0))
        { // WE GET BY THE ON INPUT
            CKBOOL active = FALSE;
            beh->GetLocalParameterValue(2, &active);
            if (active)
            {
                behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(ProgressionBarPostRender, beh);
            }
            beh->ActivateOutput(0);
            beh->ActivateInput(0, FALSE);
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR DisplayProgressionBarCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(0);
        if (sprite)
        {
            sprite->Show(CKHIDE);
        }
        sprite = (CKSprite *)beh->GetLocalParameterObject(1);
        if (!sprite)
            return CKBR_PARAMETERERROR;
        sprite->Show(CKHIDE);
        CKBOOL b = TRUE;
        beh->SetLocalParameterValue(2, &b);
    }
    break;
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        if (beh->IsActive())
        {
            behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(ProgressionBarPostRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsActive() && beh->IsParentScriptActiveInScene(scene))
            behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(ProgressionBarPostRender, beh);
        else
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(ProgressionBarPostRender, beh);
    }
    break;
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORDETACH:
    {
        if (behcontext.CurrentRenderContext)
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(ProgressionBarPostRender, beh);
    }
    break;
    }
    return CKBR_OK;
}

void ProgressionBarPostRender(CKRenderContext *rc, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;

    CKSprite *prog = (CKSprite *)beh->GetLocalParameterObject(1);
    CKSprite *prog2 = (CKSprite *)beh->GetLocalParameterObject(0);
    int x;
    beh->GetInputParameterValue(0, &x);
    int y;
    beh->GetInputParameterValue(1, &y);
    float p;
    beh->GetInputParameterValue(2, &p);
    if (p < 0.0f)
        p = 0.0f;
    if (p > 1.0f)
        p = 1.0f;

    VxRect rect;
    if (prog)
    {
        int sw = prog->GetWidth();
        int sh = prog->GetHeight();
        //	prog->GetSourceRect(&rect);
        rect.left = 0;
        rect.top = 0;
        rect.right = (float)(int)(sw * p);
        rect.bottom = (float)sh;
        prog->SetSourceRect(rect);
        prog->UseSourceRect(TRUE);
        VxRect destrect;
        destrect.left = (float)x;
        destrect.top = (float)y;
        destrect.right = x + rect.right;
        destrect.bottom = (float)(y + sh);
        prog->SetRect(destrect);
    }

    if (prog2)
    {
        prog2->Show(CKSHOW);
        prog2->RestoreInitialSize();
        prog2->UseSourceRect(FALSE);
        prog2->SetPosition(Vx2DVector((float)x, (float)y));
        prog2->Render(rc);
        prog2->Show(CKHIDE);
    }
    if (prog)
    {
        prog->Show(CKSHOW);
        prog->Render(rc);
        prog->Show(CKHIDE);
    }
}
