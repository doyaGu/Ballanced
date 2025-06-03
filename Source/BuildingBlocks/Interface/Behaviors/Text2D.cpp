/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Draw 2D String
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

#define CKPGUID_ALIGNMENT      CKGUID(0x2e1e2209, 0x47da44b5)
#define CKPGUID_TEXTPROPERTIES CKGUID(0x4157001d, 0x4cc82922)

CKObjectDeclaration *FillBehaviorText2DDecl();
CKERROR CreateText2DProto(CKBehaviorPrototype **);
int Text2D(const CKBehaviorContext &behcontext);
CKERROR TextCallback(const CKBehaviorContext &behcontext);
CKBOOL Text2DRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *Argument);

CKObjectDeclaration *FillBehaviorText2DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("2D Text");
    od->SetDescription("Display a text on a 2D Frame, using a texture font");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Font:</SPAN>font created with the building blocks "Create Font" or "Create System Font".<BR>
    <SPAN CLASS=pin>Text:</SPAN>text to be displayed. The text can be multiline.<BR>
    <SPAN CLASS=pin>Alignment:</SPAN>alignment of the text in the frame.<BR>
    <SPAN CLASS=pin>Margins:</SPAN>margins to leave empty in the border of the frame.<BR>
    <SPAN CLASS=pin>Offset:</SPAN>offset at which the text is displayed inside the frame. This offset can
    be used to make a scrolling inside the frame, thus taking advantage of the frame clipping.<BR>
    <SPAN CLASS=pin>Paragraph Indentation:</SPAN>space to leave horizontally and vertically at each
    paragraph start (a paragraph begins just after a carriage return).<BR>
    <SPAN CLASS=pin>Background Material:</SPAN>material to be used if a background must be drawn (option "Background" enabled).<BR>
    <SPAN CLASS=pin>Caret Size:</SPAN>size of the caret in percent. The caret is only displayed when using the
    building blocks "Input String" or "Set Caret Position" and with the option "Show Caret" enabled.<BR>
    <SPAN CLASS=pin>Caret Material:</SPAN>material to be used for the display of the caret.<BR>
    <BR>
    <SPAN CLASS=pout>Text Extents:</SPAN>real extents of the text drawn. Can be used to achieve a scrolling with
    the <SPAN CLASS=pin>Offset</SPAN> parameter.<BR>
    <SPAN CLASS=pout>Line Count:</SPAN>number of text lines displayed.<BR>
    <BR>
    <SPAN CLASS=setting>Text Properties:</SPAN>options of text displaying:<BR>
    <UL>
    <LI>Screen Proportional: Enable this option if the text must be proportional of the screen size. Else, the size is fixed.</LI>
    <LI>Background: Enable this to display a background material behind the text, using the extents of the text.</LI>
    <LI>Clip To Dimension: Enable this to clip the text to the frame extents.</LI>
    <LI>Resize Vertically: Enable this to resize the frame according to the text height.</LI>
    <LI>Resize Horizontally: Enable this to resize the frame according to the text width. Works only with no wrapping and no justification.</LI>
    <LI>WordWrap: Enables the word wrapping of the text (automatically put a word to the next line when it doesn't fit in the frame.)</LI>
    <LI>Justified: Extension of the wrapping : words not fitting are sent to the next line and the spaces of the line are stretched for the
    line to fit exactly the frame width. These two options are incompatible with the Resize Horizontally option.</LI>
    <LI>Compiled: Enable this to compile the graphic data of the text, for more efficiency, if you know the string won't change
    after the first display.</LI>
    <LI>Multiple: Transforms the behavior in an In-Out building block, allowing you to draw several text on several frames with a single
    building block. Typically, you can use this with an Array containing a text and a position, then with an Array Iterator, move the
    frame to the stored position then display the text, and so on for each line of the array, all of this in one behavioral frame.</LI>
    <LI>Show Caret: Enable this if you want the caret to be displayed, when there is one.</LI>
    </UL>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x55b29fe, 0x662d5ca0));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateText2DProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Interface/Text");
    return od;
}

CKERROR CreateText2DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("2D Text");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Font", CKPGUID_FONT);
    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Alignment", CKPGUID_ALIGNMENT, "Top-Left");
    proto->DeclareInParameter("Margins", CKPGUID_RECT, "(2,2),(2,2)");
    proto->DeclareInParameter("Offset", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Paragraph Indentation", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Background Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Caret Size", CKPGUID_PERCENTAGE, "10");
    proto->DeclareInParameter("Caret Material", CKPGUID_MATERIAL);

    proto->DeclareOutParameter("Text Extents", CKPGUID_RECT);
    proto->DeclareOutParameter("Line Count", CKPGUID_INT);

    proto->DeclareSetting("Text Properties", CKPGUID_TEXTPROPERTIES);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Text2D);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS | CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TextCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD | CKCB_BEHAVIORCREATE);

    *pproto = proto;
    return CK_OK;
}

int Text2D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();

    if (beh->IsInputActive(1)) // we enter by off
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        int options = 0;
        beh->GetLocalParameterValue(0, &options);

        if (options & TEXT_MULTIPLE)
        {
            // Only when activated, we set the callback
            if (ent)
                Text2DRenderCallback(behcontext.CurrentRenderContext, ent, (void *)beh->GetID());
            return CKBR_OK;
        }
    }

    // we add the render callback (temporarily)
    if (ent)
        ent->AddPostRenderCallBack(Text2DRenderCallback, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

CKBOOL Text2DRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *Argument)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(dev->GetCKContext(), (CK_ID)Argument);
    if (!beh)
        return TRUE;

    CK2dEntity *ent = (CK2dEntity *)obj;
    CKContext *ctx = beh->GetCKContext();

    CKFontManager *fm = (CKFontManager *)ctx->GetManagerByGuid(FONT_MANAGER_GUID);

    // Old version care for space changing
    fm->m_VersionUpdate = (beh->GetVersion() >= 0x00020000);

    ///
    // Input Parameters

    // We get the texture font
    int fontIndex = 0;
    beh->GetInputParameterValue(0, &fontIndex);
    CKTextureFont *font = fm->GetFont(fontIndex);
    if (!font)
        return TRUE;

    // We get the string
    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
    if (!string)
        return 0;

    // We get the alignment
    int align = 0;
    beh->GetInputParameterValue(2, &align);

    // Margins
    VxRect margins(2, 2, 2, 2);
    beh->GetInputParameterValue(3, &margins);
    font->m_Margins = margins;

    // Scroll Offset
    Vx2DVector offset(0.0f, 0.0f);
    beh->GetInputParameterValue(4, &offset);
    font->m_Offset = offset;

    // Paragraph indentation
    Vx2DVector pIndent;
    beh->GetInputParameterValue(5, &pIndent);
    font->m_ParagraphIndentation = pIndent;

    // Caret parameters
    beh->GetInputParameterValue(7, &font->m_CaretSize);
    font->m_CaretMaterial = (CKMaterial *)beh->GetInputParameterObject(8);

    int textFlags = 0;
    beh->GetLocalParameterValue(0, &textFlags);

    ///
    // 2D Entity extents
    VxRect destRect;
    ent->GetRect(destRect);
    if (ent->IsRatioOffset())
    {
        VxRect screen;
        dev->GetViewRect(screen);
        destRect.Translate(screen.GetTopLeft());
    }

    CKDWORD oldFog = dev->GetState(VXRENDERSTATE_FOGENABLE);
    dev->SetState(VXRENDERSTATE_FOGENABLE, FALSE);

    VxRect textZone = destRect;

    // Screen Clipping
    if (ent->IsClippedToCamera())
    {
        textFlags |= TEXT_SCREENCLIP;
        dev->GetViewRect(font->m_ClippingRect);
    }

    // Parent Clipping
    if (ent->GetFlags() & CK_2DENTITY_CLIPTOPARENT)
    {
        CK2dEntity *parent = NULL;
        parent = ent->GetParent();
        if (parent)
        {
            VxRect dummy;
            if (textFlags & TEXT_SCREENCLIP) // Already clipped by screen
            {
                VxRect rect;
                parent->GetExtents(dummy, rect);
                rect.Clip(font->m_ClippingRect);
                font->m_ClippingRect = rect;
            }
            else
            {
                textFlags |= TEXT_SCREENCLIP;
                parent->GetExtents(dummy, font->m_ClippingRect);
            }
        }
    }

    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(6);

    if (textFlags & TEXT_MULTIPLE)
    {
        if (ent->IsVisible())
            fm->DrawText(ent->GetID(), fontIndex, string, font->m_Scale, font->m_StartColor, font->m_EndColor, align, textZone, mat, textFlags);
    }
    else
    {
        ///
        // Clipping : necessary because i m being clipped to the camera otherwise!!!
        VxRect oldClip;
        CKBOOL isNotClippedToCamera = !ent->IsClippedToCamera();
        if (isNotClippedToCamera)
        {
            dev->GetViewRect(oldClip);
            VxRect clip = oldClip;
            dev->GetWindowRect(clip);
            clip.Translate(-clip.GetTopLeft());
            dev->SetViewRect(clip);
        }

        // TODO : Display only what fits in the current clipping
        font->DrawCKText(dev, ent, string, align, textZone, mat, textFlags, TRUE);

        // Clipping
        if (isNotClippedToCamera)
        {
            dev->SetViewRect(oldClip);
        }

        // We write the text extents
        beh->SetOutputParameterValue(0, &font->m_TextExtents);
        beh->SetOutputParameterValue(1, &font->m_LineCount);

        if (dev->GetFogMode() != VXFOG_NONE)
            dev->SetState(VXRENDERSTATE_FOGENABLE, TRUE);
    }

    // Reenable Fog (if necessary)
    dev->SetState(VXRENDERSTATE_FOGENABLE, oldFog);

    return TRUE;
}