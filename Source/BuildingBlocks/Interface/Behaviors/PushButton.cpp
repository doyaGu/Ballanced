/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PushButton
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorPushButtonDecl();
CKERROR CreatePushButtonProto(CKBehaviorPrototype **);
int PushButton(const CKBehaviorContext &behcontext);
CKBOOL PushButtonRenderCallback(CKRenderContext *context, CKRenderObject *ent, void *Argument);
CKERROR PushButtonCallBack(const CKBehaviorContext &behcontext);

#define CKPGUID_PUSHBUTTONOPTION CKGUID(0x25064124, 0x1fbe6aae)

CKObjectDeclaration *FillBehaviorPushButtonDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("PushButton");
    od->SetDescription("Transforms a 2D Frame into a push button.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Released: </SPAN>is activated when button is released.<BR>
    <SPAN CLASS=out>Pressed: </SPAN>is activated when button is pressed.<BR>
    <SPAN CLASS=out>Roll Over: </SPAN>is activated when button is rolled over.<BR>
    <BR>
    <SPAN CLASS=pin>Released Material: </SPAN>Material to use as the button is released.<BR>
    <SPAN CLASS=pin>Pressed Material: </SPAN>Material to use as the button is pressed.<BR>
    <SPAN CLASS=pin>RollOver Material: </SPAN>Material to use when the mouss is on the button.<BR>
    <SPAN CLASS=pin>Font: </SPAN>font to use if the button must have a text on it.<BR>
    <SPAN CLASS=pin>Text: </SPAN>text to display on the button.<BR>
    <SPAN CLASS=pin>Pressed Offset: </SPAN>Offset to apply to the text when the button is pressed.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x761b7e64, 0x2b4c6911));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00030000);
    od->SetCreationFunction(CreatePushButtonProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Interface/Controls");
    return od;
}

CKERROR CreatePushButtonProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("PushButton");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Disable");

    proto->DeclareOutput("Pressed");
    proto->DeclareOutput("Released");
    proto->DeclareOutput("Active");
    proto->DeclareOutput("Enter Button");
    proto->DeclareOutput("Exit Button");
    proto->DeclareOutput("In Button");

    proto->DeclareInParameter("Released Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Pressed Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("RollOver Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Font", CKPGUID_FONT);
    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Pressed Offset", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Disabled Material", CKPGUID_MATERIAL);

    proto->DeclareLocalParameter(NULL, CKPGUID_INT, "0");                                                                    // State
    proto->DeclareSetting("Events", CKPGUID_PUSHBUTTONOPTION, "Pressed,Released,Active,Enter Button,Exit Button,In Button"); // State

    proto->DeclareOutParameter("Current Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PushButton);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(PushButtonCallBack);

    *pproto = proto;
    return CK_OK;
}

#define RELEASED 0
#define PRESSED 1
#define ROLLOVER 2
#define PUSHED 3
#define DISABLED 4

enum
{
    BOM_PRESSED = 1,
    BOM_RELEASED = 2,
    BOM_ACTIVE = 4,
    BOM_ENTERREGION = 8,
    BOM_EXITREGION = 16,
    BOM_INREGION = 32
};

enum
{
    BO_PRESSED,
    BO_RELEASED,
    BO_ACTIVE,
    BO_ENTERREGION,
    BO_EXITREGION,
    BO_INREGION
};

int PushButton(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK2dEntity *frame = (CK2dEntity *)beh->GetTarget();

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
    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);
        state = DISABLED;
        beh->SetLocalParameterValue(0, &state);
    }

    int oldstate = 0;
    beh->GetLocalParameterValue(0, &oldstate);

    if (!(oldstate & DISABLED)) // If button is disabled, nothing to test
    {
        int eventMask = 63;
        beh->GetLocalParameterValue(1, &eventMask);

        CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
        if (!im)
            return CKBR_ACTIVATENEXTFRAME;

        // TODO : button configurable
        int button = 0;

        CKFontManager *fm = (CKFontManager *)behcontext.Context->GetManagerByGuid(FONT_MANAGER_GUID);
        CKBOOL inside = FALSE;
        if (frame == fm->GetEntityUnderMouse())
            inside = TRUE;

        CKBOOL mousepressed = im->IsMouseButtonDown(CK_MOUSEBUTTON(button));

        if (inside)
            state |= ROLLOVER;
        if (mousepressed)
            state |= PRESSED;

        if (state != oldstate) // State change
        {
            // we store the new state
            beh->SetLocalParameterValue(0, &state);
        }

        if (beh->GetVersion() > 0x00020000) // new version
        {
            int pos = 0;

            // Button state
            if (eventMask & BOM_PRESSED)
            {
                if (!(oldstate & PRESSED) && (state & PRESSED) && (state & ROLLOVER))
                    beh->ActivateOutput(pos);
                ++pos;
            }
            if (eventMask & BOM_RELEASED)
            {
                if ((oldstate == PUSHED) && !(state & PRESSED) && (state & ROLLOVER))
                    beh->ActivateOutput(pos);
                ++pos;
            }
            if (eventMask & BOM_ACTIVE)
            {
                if ((state & PRESSED) && (state & ROLLOVER))
                    beh->ActivateOutput(pos);
                ++pos;
            }

            // region
            if (eventMask & BOM_ENTERREGION)
            {
                if (!(oldstate & ROLLOVER) && (state & ROLLOVER))
                    beh->ActivateOutput(pos);
                ++pos;
            }

            if (eventMask & BOM_EXITREGION)
            {
                if ((oldstate & ROLLOVER) && !(state & ROLLOVER))
                    beh->ActivateOutput(pos);
                ++pos;
            }

            if (eventMask & BOM_INREGION)
            {
                if (state & ROLLOVER)
                    beh->ActivateOutput(pos);
                ++pos;
            }
        }
        else // old version
        {
            if (state != oldstate) // State change
            {

                // We activate the roll over if we enter it
                if ((oldstate & ROLLOVER) && !(state & ROLLOVER))
                    beh->ActivateOutput(RELEASED);

                // We activate the roll over if we enter it
                if (!(oldstate & ROLLOVER) && (state & ROLLOVER))
                    beh->ActivateOutput(ROLLOVER);

                // We activate the pressed output when we release the button
                if ((oldstate == PUSHED) && (state == ROLLOVER))
                    beh->ActivateOutput(PRESSED);
            }
        }
    }

    // we add the render callback
    if (frame)
        frame->AddPreRenderCallBack(PushButtonRenderCallback, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

CKBOOL PushButtonRenderCallback(CKRenderContext *dev, CKRenderObject *ent, void *iArg)
{
    static VxRect fulluv(0, 0, 1.0f, 1.0f);

    CKContext *ctx = dev->GetCKContext();
    CKBehavior *beh = (CKBehavior *)ctx->GetObject((CK_ID)iArg);
    if (!beh)
        return TRUE;

    CKFontManager *fm = (CKFontManager *)ctx->GetManagerByGuid(FONT_MANAGER_GUID);

    // Old version care for space changing
    fm->m_VersionUpdate = (beh->GetVersion() >= 0x00020000);

    CK2dEntity *frame = (CK2dEntity *)ent;

    ///
    // Clipping : necessary because i m being clipped to the camera otherwise!!!
    VxRect oldClip;
    CKBOOL isNotClippedToCamera = !frame->IsClippedToCamera();
    if (isNotClippedToCamera)
    {
        dev->GetViewRect(oldClip);
        VxRect clip = oldClip;
        dev->GetWindowRect(clip);
        clip.Translate(-clip.GetTopLeft());
        dev->SetViewRect(clip);
    }

    // 2D Frame Zone
    VxRect rect;
    frame->GetExtents(fulluv, rect);

    int state = 0;
    beh->GetLocalParameterValue(0, &state);

    if (state == PRESSED)
        state = RELEASED; // Pressed outside the button
    if (state == PUSHED)
        state = PRESSED; // Pressed inside the button
    if (state == DISABLED)
        state = 6;

    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(state);
    VxColor col(1.0f, 1.0f, 1.0f, 1.0f);
    if (mat)
        col = mat->GetDiffuse();

    if (mat)
        beh->SetOutputParameterObject(0, mat);

    // We draw the button background
    DrawFillRectangle(dev, col.GetRGBA(), mat, rect, fulluv);

    // We get the texture font
    int fontIndex = 0;
    beh->GetInputParameterValue(3, &fontIndex);
    CKTextureFont *font = fm->GetFont(fontIndex);
    if (!font)
        return TRUE;

    // Init of the font : hopefully temporary
    VxRect margins(2, 2, 2, 2);
    font->m_Margins = margins;
    Vx2DVector off(0.0f, 0.0f);
    font->m_Offset = off;
    font->m_ParagraphIndentation = Vx2DVector(0.0f);

    CKTexture *fontTexture = font->GetFontTexture();
    // font->m_ScreenExtents.Set(fontTexture->GetWidth(),fontTexture->GetHeight());

    // We get the string
    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(4);
    if (!string)
        return TRUE;

    if (state == PRESSED)
    {
        Vx2DVector offset;
        beh->GetInputParameterValue(5, &offset);

        rect.Translate(offset);
    }

    // we draw the button text
    font->DrawCKText(dev, frame, string, 0, rect, NULL, 4, TRUE);

    // Clipping
    if (isNotClippedToCamera)
    {
        dev->SetViewRect(oldClip);
    }

    return TRUE;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR PushButtonCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (beh->GetVersion() < 0x00030000)
        return CK_OK;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        int eventMask = 63;
        beh->GetLocalParameterValue(1, &eventMask);

        int pos = 0;
        CKBehaviorIO *out = NULL;

        // Outputs Creation/Deletion
        if (eventMask & BOM_PRESSED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Pressed");
            }
            else
                beh->AddOutput("Pressed");
            ++pos;
        }
        if (eventMask & BOM_RELEASED)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Released");
            }
            else
                beh->AddOutput("Released");
            ++pos;
        }
        if (eventMask & BOM_ACTIVE)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Active");
            }
            else
                beh->AddOutput("Active");
            ++pos;
        }
        if (eventMask & BOM_ENTERREGION)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Enter Button");
            }
            else
                beh->AddOutput("Enter Button");
            ++pos;
        }
        if (eventMask & BOM_EXITREGION)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("Exit Button");
            }
            else
                beh->AddOutput("Exit Button");
            ++pos;
        }
        if (eventMask & BOM_INREGION)
        {
            if (out = beh->GetOutput(pos))
            {
                out->SetName("In Button");
            }
            else
                beh->AddOutput("In Button");
            ++pos;
        }

        // we delete the unuseful outputs
        while (pos < beh->GetOutputCount())
            beh->DeleteOutput(pos);
    }
    break;
    }
    return CKBR_OK;
}
