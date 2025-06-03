/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DisplayScore
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDisplayScoreDecl();
CKERROR CreateDisplayScoreProto(CKBehaviorPrototype **pproto);
int DisplayScore(const CKBehaviorContext &behcontext);
CKERROR DisplayScoreCallBack(const CKBehaviorContext &behcontext);
void ScorePostRender(CKRenderContext *rc, void *arg);

CKObjectDeclaration *FillBehaviorDisplayScoreDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Display Score");
    od->SetDescription("Displays a score in 2d");
    /* rem:
    <SPAN CLASS=in>On : </SPAN>Activates the Score Display.<BR>
    <SPAN CLASS=in>Off : </SPAN>Deactivates the Score Display.<BR>
    <BR>
    <SPAN CLASS=out>Exit On : </SPAN>This output is activated after entering by 'On'.<BR>
    <SPAN CLASS=out>Exit Off : </SPAN>This output is activated after entering by 'Off'.<BR>
    <BR>
    <SPAN CLASS=pin>X : </SPAN>Horizontal Position of the first score digits.<BR>
    <SPAN CLASS=pin>Y : </SPAN>Vertical Position of the first score digits.<BR>
    <SPAN CLASS=pin>Score : </SPAN>Score to be displayed (eg: 100, 300 or even 100000 !).<BR>
    <BR>
    */
    /* warning:
    - All the digits must be loaded in the system memory (you don't have to load them in the card memory like the other textures). To do so Drag'n Drop the image in the 3d view with the Shift Key.<BR>
    - The digits have to contain the corresponding index. (eg: score_0, score_1, score_2, ... etc or 0score, 1score, 2score).
    - You don't have to specify all the digits ... just give the name of the first sprite (the Zero one) in the settings of 'Display Score'.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfc45543, 0x55b012c3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDisplayScoreProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Visuals/Text");
    return od;
}

CKERROR CreateDisplayScoreProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Display Score");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("X", CKPGUID_INT, "0");
    proto->DeclareInParameter("Y", CKPGUID_INT, "0");
    proto->DeclareInParameter("Score", CKPGUID_INT, "0");

    proto->DeclareSetting("Number 0", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);
    proto->DeclareLocalParameter("DisplayScore", CKPGUID_SPRITE);

    proto->DeclareLocalParameter("Active", CKPGUID_BOOL, "FALSE");

    proto->SetFunction(DisplayScore);
    proto->SetBehaviorCallbackFct(DisplayScoreCallBack);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);

    *pproto = proto;
    return CK_OK;
}

int DisplayScore(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // OFF
        behcontext.CurrentRenderContext->RemovePostRenderCallBack(ScorePostRender, beh);
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
    }
    else
    {
        if (beh->IsInputActive(0))
        { // ON
            CKBOOL active = FALSE;
            beh->GetLocalParameterValue(10, &active);
            if (active)
            {
                behcontext.CurrentRenderContext->AddPostRenderCallBack(ScorePostRender, beh);
            }
            beh->ActivateOutput(0);
            beh->ActivateInput(0, FALSE);
        }
    }

    return CKBR_OK;
}

CKERROR DisplayScoreCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(0);
        if (!sprite)
            return CKBR_PARAMETERERROR;
        sprite->Show(CKHIDE);

        char name[256] = "";
        if (sprite->GetName())
            strcpy(name, sprite->GetName());
        //---------------------------------------------
        char *number = strchr(name, '0');
        if (!number)
        {
            ctx->OutputToConsoleEx("You didn't provide a valid sprite (0 in the name)");
            return CKBR_PARAMETERERROR;
        }
        int i;
        for (i = 1; i < 10; i++)
        {
            sprintf(number, "%d", i);
            sprite = (CKSprite *)ctx->GetObjectByNameAndClass(name, CKCID_SPRITE);
            if (!sprite)
            {
                ctx->OutputToConsoleEx("You didn't provide sprite %s", name);
                return CKBR_PARAMETERERROR;
            }
            beh->SetLocalParameterObject(i, sprite);
            sprite->Show(CKHIDE);
        }
        CKBOOL b = TRUE;
        beh->SetLocalParameterValue(10, &b);
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        if (behcontext.CurrentRenderContext)
            behcontext.CurrentRenderContext->RemovePostRenderCallBack(ScorePostRender, beh);
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        CKBOOL active;
        beh->GetLocalParameterValue(10, &active);
        if (active && behcontext.CurrentRenderContext)
        {
            behcontext.CurrentRenderContext->RemovePostRenderCallBack(ScorePostRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        CKBOOL active;
        beh->GetLocalParameterValue(10, &active);
        if (active && behcontext.CurrentRenderContext)
        {
            behcontext.CurrentRenderContext->AddPostRenderCallBack(ScorePostRender, beh);
        }
    }
    break;
    }
    return CKBR_OK;
}

void ScorePostRender(CKRenderContext *rc, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;

    int score;
    beh->GetInputParameterValue(2, &score);
    if (score < 0)
        score = 0;
    int x;
    beh->GetInputParameterValue(0, &x);
    int y;
    beh->GetInputParameterValue(1, &y);
    static char buffer[64];
    sprintf(buffer, "%d", score);
    int s;
    Vx2DVector size;
    CKSprite *sprite;
    int i;
    for (i = 0; i < (int)strlen(buffer); i++)
    {
        s = buffer[i] - '0';
        sprite = (CKSprite *)beh->GetLocalParameterObject(s);
        if (!sprite)
            return;
        sprite->SetPosition(Vx2DVector((float)x, (float)y));
        sprite->Render(rc);
        sprite->GetSize(size);
        x += (int)size.x;
    }
}
