#ifndef CK_TEXTUREFONT_H
#define CK_TEXTUREFONT_H

#include "CKFontManager.h"

#define VALIGN_TOP       4
#define VALIGN_BOTTOM    8

#define HALIGN_LEFT      1
#define HALIGN_RIGHT     2

#define FONT_GRADIENT            1
#define FONT_SHADOW              2
#define FONT_LIGHTING            4
#define FONT_DISABLEFILTER       8

#define TEXT_SCREEN              1
#define TEXT_BACKGROUND          2
#define TEXT_CLIP                4
#define TEXT_RESIZE_VERT         8
#define TEXT_RESIZE_HORI         16
#define TEXT_WORDWRAP            32
#define TEXT_JUSTIFIED           64
#define TEXT_COMPILED            128
#define TEXT_MULTIPLE            256
#define TEXT_SHOWCARET           512
#define TEXT_3D                  1024
#define TEXT_SCREENCLIP          2048

#define CKPGUID_FONTPROPERTIES CKGUID(0x63223dd5, 0x6b5f68fc)
#define CKPGUID_TEXTPROPERTIES CKGUID(0x4157001d, 0x4cc82922)

struct CharacterTextureCoordinates {
    float ustart;
    float vstart;
    float uwidth;
    float uprewidth;
    float upostwidth;
    float vwidth;
};

class CKFontManager;

class CKTextureFont {
public:
    enum SpacingProperties {
        FIXED = 1,
        SPACINGTOBESAVED = 2,
        TOBESAVED = 3,
        CREATED = 4
    };

    CKTextureFont(CKFontManager *fm, CKContext *ctx, char *name) {
        // Font Visual Properties
        m_Leading.Set(0.0f, 0.0f);
        m_Scale.Set(1.0f, 1.0f);
        m_ShadowOffset.Set(4.0f, 4.0f);
        m_ShadowScale.Set(1.0f, 1.0f);
        m_ItalicOffset = 0.0f;
        m_StartColor = RGBAITOCOLOR(255, 255, 255, 255);
        m_EndColor = RGBAITOCOLOR(0, 0, 0, 255);
        m_ShadowColor = RGBAITOCOLOR(0, 0, 0, 128);
        m_Material = 0;
        m_Properties = 0;
        m_FontTexture = 0;
        m_FirstCharacter = 0;
        m_ParagraphIndentation = Vx2DVector(0.0f);
        m_SpacingProperties = 0;
        m_CaretMaterial = NULL;
        m_CaretSize = 0.0f;
        m_SpacePercentage = 0.3f;

        m_FontName = CKStrdup(name);
        m_Context = ctx;
        m_FontManager = fm;
    }

    ~CKTextureFont() { CKDeletePointer(m_FontName); }

    virtual void DrawCKText(CKRenderContext *dev, CKBeObject *obj, CKSTRING string, int align, VxRect &textzone, CKMaterial *mat, int options, CKBOOL reallyDraw);

#if CKVERSION != 0x13022002
    virtual void DrawStringEx(CKRenderContext* iRC, const char* iString, const VxRect& iTextZone, int iOptions);
#endif

public:
    // The Font Name
    char *m_FontName;
    char *m_SystemFontName;

    /////////////////////////
    // Font Visual Properties

    // First character (code ASCII) in the texture
    int m_FirstCharacter;
    // Space between character (horizontal and vertical)
    Vx2DVector m_Leading;
    // Scale of the font
    Vx2DVector m_Scale;
    // Italic offset
    float m_ItalicOffset;
    // Paragraph Indentation
    Vx2DVector m_ParagraphIndentation;
    // Offset value
    Vx2DVector m_Offset;
    // Start Color of the font
    CKDWORD m_StartColor;
    // End color of the font
    CKDWORD m_EndColor;
    // Shadow color
    CKDWORD m_ShadowColor;
    // Shadow offset
    Vx2DVector m_ShadowOffset;
    // Shadow size
    Vx2DVector m_ShadowScale;
    // Screen Extents
    Vx2DVector m_ScreenExtents;
    // Material (when lighted)
    CK_ID m_Material;
    // Text Properties
    CKDWORD m_Properties;
    // Text Margins
    VxRect m_Margins;
    // Text Extents
    VxRect m_TextExtents;
    // Clipping Rect
    VxRect m_ClippingRect;
    // Percentage of a ' ' character
    float m_SpacePercentage;

    int m_LineCount;

    CKMaterial *m_CaretMaterial;
    float m_CaretSize;

    // Special Drawing Variables : temporary variables
    float m_SpaceSize;
    float m_HLeading;
    float m_LineWidth;

    ////////////////////////
    // Static Font Members

    // Number of chars, horizontally and vertically
    Vx2DVector m_CharNumber;
    // Zone of the fonts in the texture
    VxRect m_FontZone;
    // Spacing Properties
    CKDWORD m_SpacingProperties;
    // the font texture
    CK_ID m_FontTexture;

    // The Font Manager
    CKFontManager *m_FontManager;

    // Font coordinates
    CharacterTextureCoordinates m_FontCoordinates[256];

private:
    CKContext *m_Context;
};

#endif