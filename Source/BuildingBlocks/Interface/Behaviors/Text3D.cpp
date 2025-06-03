/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Draw 3D String
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

#define CKPGUID_ALIGNMENT      CKGUID(0x2e1e2209, 0x47da44b5)
#define CKPGUID_TEXTPROPERTIES CKGUID(0x4157001d, 0x4cc82922)

CKObjectDeclaration *FillBehaviorText3DDecl();
CKERROR CreateText3DProto(CKBehaviorPrototype **);
int Text3D(const CKBehaviorContext &behcontext);
CKERROR TextCallback(const CKBehaviorContext &behcontext);
int Text3DRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *arg);

CKObjectDeclaration *FillBehaviorText3DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("3D Text");
    od->SetDescription("Display a text on the plane XY of a 3D Frame, using a texture font");
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
    od->SetGuid(CKGUID(0x1f5b2fe7, 0x6d355175));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateText3DProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Interface/Text");
    return od;
}

CKERROR CreateText3DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("3D Text");
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
    proto->SetFunction(Text3D);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS | CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TextCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD | CKCB_BEHAVIORCREATE);

    *pproto = proto;
    return CK_OK;
}

int Text3D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        int options = 0;
        beh->GetLocalParameterValue(0, &options);

        if (options & TEXT_MULTIPLE)
        {
            // Only when activated, we set the callback
            if (ent && ent->IsVisible() && !ent->IsHiddenByParent())
                Text3DRenderCallback(behcontext.CurrentRenderContext, ent, (void *)beh->GetID());
            return CKBR_OK;
        }
        else
        {
            if (ent)
                ent->SetRenderAsTransparent(TRUE);
        }
    }
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    // if the frame was completely outside last frame
    if (ent && (ent->GetClassID() == CKCID_3DENTITY) && ent->IsAllOutsideFrustrum())
    {
        VxBbox box;
        ent->SetBoundingBox(&box);
    }

    // we add the render callback (temporarily)
    if (ent)
        ent->AddPostRenderCallBack(Text3DRenderCallback, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR TextCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int options = 0;
    beh->GetLocalParameterValue(0, &options);

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORCREATE:
        beh->EnableInputParameter(6, options & TEXT_BACKGROUND);
        beh->EnableInputParameter(7, options & TEXT_SHOWCARET);
        beh->EnableInputParameter(8, options & TEXT_SHOWCARET);
        break;

    case CKM_BEHAVIORSETTINGSEDITED:
    {
        beh->EnableInputParameter(6, options & TEXT_BACKGROUND);
        beh->EnableInputParameter(7, options & TEXT_SHOWCARET);
        beh->EnableInputParameter(8, options & TEXT_SHOWCARET);

        int nbOut = beh->GetOutputCount();
        if (options & TEXT_MULTIPLE)
        {
            if (nbOut == 2)
            {
                beh->DeleteInput(1);
                beh->DeleteInput(0);
                beh->DeleteOutput(1);
                beh->DeleteOutput(0);
                CKDestroyObject(beh->RemoveOutputParameter(1));
                CKDestroyObject(beh->RemoveOutputParameter(0));

                beh->CreateInput("In");
                beh->CreateOutput("Out");
            }
        }
        else
        {
            if (nbOut == 1)
            {
                beh->DeleteInput(0);
                beh->DeleteOutput(0);

                beh->CreateInput("On");
                beh->CreateInput("Off");
                beh->CreateOutput("Exit On");
                beh->CreateOutput("Exit Off");
                beh->CreateOutputParameter("Text Extents", CKPGUID_RECT);
                beh->CreateOutputParameter("Line Count", CKPGUID_INT);
            }
        }
    }
    break;
    }
    return CKBR_OK;
}

int Text3DRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    static const float inv32f = 1.0f / 32.0f;
    static const float inv16f = 1.0f / 16.0f;

    CKBehavior *beh = (CKBehavior *)CKGetObject(dev->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return TRUE;

    CK3dEntity *ent = (CK3dEntity *)obj;

    CKFontManager *fm = (CKFontManager *)beh->GetCKContext()->GetManagerByGuid(FONT_MANAGER_GUID);
    if (!fm)
        return 0;

    // Old version care for space changing
    fm->m_VersionUpdate = (beh->GetVersion() >= 0x00020000);

    ///
    // Input Parameters

    // We get the texture font
    int fontIndex = 0;
    beh->GetInputParameterValue(0, &fontIndex);
    CKTextureFont *font = fm->GetFont(fontIndex);
    if (!font)
        return 0;

    // We get the string
    CKSTRING textToDisplay = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
    if (!textToDisplay)
        return 0;

    // We get the alignment
    int align = 0;
    beh->GetInputParameterValue(2, &align);

    // Margins
    font->m_Margins.SetCorners(2, 2, 2, 2);
    beh->GetInputParameterValue(3, &font->m_Margins);
    font->m_Margins *= Vx2DVector(inv16f);

    // Scroll Offset
    font->m_Offset.Set(0.0f, 0.0f);
    beh->GetInputParameterValue(4, &font->m_Offset);

    // Paragraph indentation
    font->m_ParagraphIndentation.Set(0.0f, 0.0f);
    beh->GetInputParameterValue(5, &font->m_ParagraphIndentation);

    // Background Material
    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(6);

    // Caret parameters
    beh->GetInputParameterValue(7, &font->m_CaretSize);
    font->m_CaretMaterial = (CKMaterial *)beh->GetInputParameterObject(8);

    ///
    // Settings Parameters

    // Text flags
    int textFlags = 0;
    beh->GetLocalParameterValue(0, &textFlags);
    textFlags |= TEXT_3D;
    if (textFlags & TEXT_JUSTIFIED)
    {
        align &= ~HALIGN_RIGHT;
        align |= HALIGN_LEFT;
    }

    // we calculate the scales of the frame
    VxVector entScale;
    ent->GetScale(&entScale, FALSE);
    VxRect textZone(-entScale.x, -entScale.y, entScale.x, entScale.y);

    if (CKIsChildClassOf(ent, CKCID_SPRITE3D)) // the size must be taken into account the bounding box of sprite3D
    {
        const VxBbox &bbox = ent->GetBoundingBox(TRUE);
        textZone.left = bbox.Min.x * entScale.x;
        textZone.right = bbox.Max.x * entScale.x;
        textZone.top = bbox.Min.y * entScale.y;
        textZone.bottom = bbox.Max.y * entScale.y;
    }

    // TODO : n'afficher que ce qui rentre dans le clipping actuel

    if (textFlags & TEXT_MULTIPLE) // Registering the text for future display
    {
        fm->DrawText(ent->GetID(), fontIndex, textToDisplay, font->m_Scale, font->m_StartColor, font->m_EndColor, align, textZone, material, textFlags);
    }
    else // Rendering Now
    {
        ///
        // We set the transformation to the 3D Entity (slightly transformed (Y negative))
        const VxMatrix &mat = ent->GetWorldMatrix();
        VxMatrix resMat = mat;
        resMat[0] = (mat[0] / entScale.x);
        resMat[1] = -(mat[1] / entScale.y);
        resMat[2] = -(mat[2] / entScale.z);
        dev->SetWorldTransformationMatrix(resMat);

        // draw the text
        font->DrawCKText(dev, ent, textToDisplay, align, textZone, material, textFlags, TRUE);

        // we restore the transform mat
        dev->SetWorldTransformationMatrix(mat);

        if (!CKIsChildClassOf(ent, CKCID_SPRITE3D)) // No modify for the bounding boxes of Sprite 3D
        {
            VxBbox bbox;
            bbox.Min.Set(font->m_TextExtents.left, -font->m_TextExtents.bottom, 0.0f);
            bbox.Max.Set(font->m_TextExtents.right, -font->m_TextExtents.top, 0.0f);
            bbox.Min /= entScale;
            bbox.Max /= entScale;
            if (material)
            {
                VxBbox unitBox(1.0f);
                bbox.Merge(unitBox);
            }
            ((CK3dEntity *)ent)->SetBoundingBox(&bbox, TRUE);
        }

        // We write the text extents
        beh->SetOutputParameterValue(0, &(font->m_TextExtents));
        beh->SetOutputParameterValue(1, &(font->m_LineCount));
    }
    return 1;
}