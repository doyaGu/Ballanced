#ifndef CK_TEXTUREFONT_H
#define CK_TEXTUREFONT_H

#include "CKAll.h"

#define VALIGN_TOP		4
#define VALIGN_BOTTOM	8

#define HALIGN_LEFT		1
#define HALIGN_RIGHT	2

#define FONT_GRADIENT			1
#define FONT_SHADOW				2
#define FONT_LIGHTING			4
#define FONT_DISABLEFILTER		8

#define TEXT_SCREEN				1
#define TEXT_BACKGROUND			2
#define TEXT_CLIP				4
#define TEXT_RESIZE_VERT		8
#define TEXT_RESIZE_HORI		16
#define TEXT_WORDWRAP			32
#define TEXT_JUSTIFIED			64
#define TEXT_COMPILED			128
#define TEXT_MULTIPLE			256
#define TEXT_SHOWCARET			512
#define TEXT_3D					1024
#define TEXT_SCREENCLIP			2048

#define CKPGUID_FONTPROPERTIES CKGUID(0x63223dd5, 0x6b5f68fc)
#define CKPGUID_TEXTPROPERTIES CKGUID(0x4157001d, 0x4cc82922)

struct CharacterTextureCoordinates
{
    float ustart;
    float vstart;
    float uwidth;
    float uprewidth;
    float upostwidth;
    float vwidth;
};

class CKFontManager;
class CompiledTextData;

class CKTextureFont
{
public:
    enum SpacingProperties
    {
        FIXED = 1,
        SPACINGTOBESAVED = 2,
        TOBESAVED = 3,
        CREATED = 4
    };

    CKTextureFont(CKFontManager *fm, CKContext *ctx, char *name);
    ~CKTextureFont();

    // Font Access
    char *GetFontName();
    CKTexture *GetFontTexture() { return (CKTexture *)m_Context->GetObject(m_FontTexture); }

    // String Calculation
    float GetStringWidth(CKSTRING string);
    int GetTextExtents(float &width, float &height);

    // Fixed Font Creation
    void CreateFromTexture();
    void CreateCKFont(CKTexture *fontTexture, VxRect &textZone, Vx2DVector &charNumber, CKBOOL fixed = TRUE, int firstCharacter = 0, float iSpaceSize = 0.3f);
    CKBOOL IsFontSimilar(CKTexture *fontTexture, Vx2DVector &charNumber, CKBOOL fixed = TRUE);

    // Display functions
    void SetRenderStates(CKRenderContext *dev, int options);
    virtual void DrawCKText(CKRenderContext *dev, CKBeObject *obj, CKSTRING string, int align, VxRect &textZone, CKMaterial *mat, int options, CKBOOL reallyDraw);
    void DrawStringEx(CKRenderContext *iRC, const char *iString, const VxRect &iTextZone, int iOptions);
    void DrawString(CKRenderContext *dev, CKSTRING string, int len, VxVector position, VxRect &textZone, CKDWORD options, CompiledTextData *ctdata = NULL);
    void DrawStringShadowed(CKRenderContext *dev, CKSTRING string, int len, VxVector position, VxRect &textZone, CKDWORD options, CompiledTextData *ctdata = NULL);
    void DrawCaret(CKRenderContext *context, float posx, float posy, float dimx, float dimy, CKDWORD flags);

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