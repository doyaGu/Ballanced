#ifndef CK_FONTMANAGER_H
#define CK_FONTMANAGER_H

#include "CKAll.h"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"
#undef WIN32_LEAN_AND_MEAN
#endif

#define CKPGUID_FONTRESOLUTION CKGUID(0x7157091d, 0x4fc82932)
#define CKPGUID_FONTNAME CKGUID(0x7167091a, 0x7f482632)

#define CKPGUID_ALIGNMENT CKGUID(0x2e1e2209, 0x47da44b5)
#define CKPGUID_FONTPROPERTIES CKGUID(0x63223dd5, 0x6b5f68fc)
#define CKPGUID_TEXTPROPERTIES CKGUID(0x4157001d, 0x4cc82922)

#define FONT_MANAGER_GUID CKGUID(0x64fb5810, 0x73262d3b)
#define CKPGUID_FONT CKGUID(0x64fb5811, 0x33862d3b)
#define CKPGUID_FONTWEIGHT CKGUID(0x4376013f, 0xb3462c0)

class CKTextureFont;
class CompiledTextData;

// Struct containing line information for a multi line text
struct LineData
{
    char *string;
    int len;
    int nbspace;
    float stringwidth;
};

typedef HFONT FONTHANDLE;
typedef TEXTMETRIC FONT_METRIC;
typedef OUTLINETEXTMETRIC FONT_OUTLINEMETRIC;
typedef ABC FONT_ABC;

typedef XHashTable<FONTHANDLE, XString> FontsTable;
typedef XHashTable<FONTHANDLE, XString>::Iterator FontIterator;

#define RECTANGLE_INTERIOR 1
#define RECTANGLE_BORDER 2

struct RectangleData
{
    RectangleData() {}
    RectangleData(CKBOOL in, const CKDWORD col, CKMaterial *mat, const VxRect &s, const VxRect &u, CKBOOL border, const CKDWORD bcol, float bsize)
        : m_InColor(col), m_Material(mat), m_Screen(s), m_InUVs(u), m_BorderColor(bcol) { m_BorderSize = border ? (in ? bsize : -bsize) : 0.0f; }
    CKDWORD m_InColor;
    CKMaterial *m_Material;
    CKDWORD m_BorderColor;
    float m_BorderSize;

    VxRect m_Screen;

    VxRect m_InUVs;
};

#define WHITE (0x00FFFFFF)

struct TextData
{
    CK_ID m_Entity;
    int m_FontIndex;
    CKSTRING m_String;
    Vx2DVector m_Scale;
    CKDWORD m_StartColor;
    CKDWORD m_EndColor;
    int m_Align;
    VxRect m_Textzone;
    CKMaterial *m_Mat;
    int m_Textflags;

    VxRect m_Margins;
    Vx2DVector m_Offset;
    Vx2DVector m_ParagraphIndentation;
    float m_CaretSize;
    CKMaterial *m_CaretMaterial;
    VxRect m_ClippingRect;

    TextData() : m_Entity(0),
                 m_FontIndex(0),
                 m_String(NULL),
                 m_Scale(1.0f, 1.0f),
                 m_EndColor(WHITE),
                 m_StartColor(WHITE),
                 m_Align(0),
                 m_Mat(NULL),
                 m_Textflags(0),
                 m_CaretMaterial(NULL) {}

    TextData(CK_ID entity, int fontindex, CKSTRING string, Vx2DVector scale, CKDWORD startColor, CKDWORD endColor, int align, VxRect textzone, CKMaterial *mat, int textflags)
        : m_Entity(entity), m_FontIndex(fontindex), m_Scale(scale), m_StartColor(startColor), m_EndColor(endColor), m_Align(align), m_Textzone(textzone), m_Mat(mat), m_Textflags(textflags)
    {
        m_String = CKStrdup(string);
    }

    ~TextData()
    {
        CKDeletePointer(m_String);
    }

    TextData(const TextData &td)
    {
        memcpy(this, &td, sizeof(struct TextData));
        m_String = CKStrdup(td.m_String);
    }

    TextData &operator=(const TextData &td)
    {
        if (this != &td)
        {
            CKDeletePointer(m_String);
            memcpy(this, &td, sizeof(struct TextData));
            m_String = CKStrdup(td.m_String);
        }
        return *this;
    }
};

typedef XArray<CKTextureFont *> TextureFontArray;
typedef XArray<LineData> LineDataArray;
typedef XArray<RectangleData> RectangleDataArray;
typedef XArray<TextData *> TextDataArray;

typedef XSHashTable<TextData *, CK_ID> TextDataTable;
typedef TextDataTable::Iterator TextDataTableIterator;
typedef XSHashTable<CompiledTextData *, CK_ID> CompiledTextTable;
typedef CompiledTextTable::Iterator CompiledTextTableIterator;

class CKFontManager : public CKBaseManager
{
public:
    // Create a font from a texture
    virtual int CreateTextureFont(CKSTRING FontName, CKTexture *fonttexture, VxRect &tzone, Vx2DVector &charnumber, CKBOOL fixed = TRUE, int firstcharacter = 0, float iSpaceSize = 0.3f);

    // Get a font index
    virtual int GetFontIndex(CKSTRING name);

    // Get a font
    virtual CKTextureFont *GetFont(unsigned int fontindex);

    // Create a logical font from a system font name
    virtual CKBOOL CreateFont(CKSTRING fontName, int systemFontIndex, int weigth, CKBOOL italic, CKBOOL underline, int resolution, int iForcedSize);

    // Create a texture from a logical font
    virtual CKTexture *CreateTextureFromFont(int systemFontIndex, int resolution, CKBOOL extended, CKBOOL bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iFontSize = 0);

    // entity under mouse
    CK2dEntity *GetEntityUnderMouse() { return m_EntityUnderMouse; }

    // Text Lines
    int GetLineCount() { return m_Lines.Size(); }
    LineData *GetLine(int i) { return (i < m_Lines.Size()) ? &m_Lines[i] : NULL; }

    // Compiled Text Data Access
    CompiledTextData *GetCompiledText(CK_ID id) {
        CompiledTextData *ctdata = NULL;
        m_CompiledText.LookUp(id, ctdata);
        return ctdata;
    }

    // Select a logical font by name
    CKBOOL SelectFont(CKSTRING fontName) {
        // We try to localize the font
        FontIterator it = m_Fonts.Find(XString(fontName));
        if (it == m_Fonts.End())
            return FALSE;

        // If it is present, we select it in the DC and retrieves the information
        return (::SelectObject(m_DC, *it) != NULL);
    }

    // test if current font is a true type font
    CKBOOL IsTrueTypeFont() {
        TEXTMETRIC metric;
        if (::GetTextMetrics(m_DC, &metric))
            return (metric.tmPitchAndFamily & TMPF_TRUETYPE);
        return FALSE;
    }

    // Get a logical font metric
    CKBOOL GetFontMetrics(FONT_METRIC &metric) {
        return ::GetTextMetrics(m_DC, &metric);
    }

    CKBOOL GetOutlineFontMetric(FONT_OUTLINEMETRIC &metric) {
        return ::GetOutlineTextMetrics(m_DC, sizeof(FONT_OUTLINEMETRIC), &metric);
    }

    // Get a font character widths
    CKBOOL GetCharABCWidths(CKBYTE startChar, CKBYTE endChar, FONT_ABC *ABCwidths) {
        return ::GetCharABCWidths(m_DC, startChar, endChar, ABCwidths);
    }

    CKBOOL GetCharWidths(CKBYTE startChar, CKBYTE endChar, int *widths) {
        return ::GetCharWidth32(m_DC, startChar, endChar, widths);
    }

    int GetFontCount() { return m_FontArray.Size(); }

    CKBOOL m_VersionUpdate;

private:
    // CONTROLS DATA

    // the 2D Entity currently under the mouse cursor
    CK2dEntity *m_EntityUnderMouse;

    // FONT DATA

    // The Font Array
    TextureFontArray m_FontArray;

    // The LineData Array
    LineDataArray m_Lines;
    // Remapping Array
    XArray<int> m_Remap;

    // The Rectangle Data Array
    RectangleDataArray m_Rectangles;

    // Text data
    TextDataArray m_2DTexts;
    TextDataArray m_3DTexts;

    // Compiled Text (sorted by entity ID)
    CompiledTextTable m_CompiledText;
    TextDataTable m_Texts;

    // The texture attribute
    int m_FontAttribute;

    CKBOOL m_TextureLoaded;

    // Win32 Specific (System TT fonts information)

    // Memory device context for font rendering
    HDC m_DC;
    // Associative table 'font name' -> 'font handle'
    FontsTable m_Fonts;
};

#endif // CK_FONTMANAGER_H
