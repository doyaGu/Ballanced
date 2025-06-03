/*************************************************************************/
/*	File : CKFontManager.h												 */
/*																		 */
/*	Author :  Aymeric Bard												 */
/*	Last Modification : 18/08/98										 */
/*																		 */
/*	Virtools Nemo SDK 													 */
/*	Copyright (c) 1998, All Rights Reserved.							 */
/*************************************************************************/
#ifndef CKFontManager_H
#define CKFontManager_H "$Id:$"

#include "CKDefines.h"
#include "CKBaseManager.h"
#include "CKTextureFont.h"
#include "CompiledData.h"

#include "DrawingPrimitives.h"

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

// Struct containing line information for a multi line text
struct LineData
{
    char *string;
    int len;
    int nbspace;
    float stringwidth;
};

#if defined(WIN32) && !defined(FONTMANAGER_NOSYSFONT)
typedef HFONT FONTHANDLE;
typedef TEXTMETRIC FONT_METRIC;
typedef OUTLINETEXTMETRIC FONT_OUTLINEMETRIC;
typedef ABC FONT_ABC;
#else
struct FONT_ABC
{
    int abcA;     // Left Side Bearing
    CKDWORD abcB; // Glyph Width
    int abcC;     // Advance Width
};

struct FONT_OUTLINEMETRIC
{
};

struct FONT_METRIC
{
    long tmHeight;
    long tmAscent;
    long tmDescent;
    long tmWeight;
    long tmAveCharWidth;
    long tmMaxCharWidth;
};

typedef void *FONTHANDLE;

#endif

typedef XHashTable<FONTHANDLE, XString> FontsTable;
typedef XHashTable<FONTHANDLE, XString>::Iterator FontIterator;

#define RECTANGLE_INTERIOR 1
#define RECTANGLE_BORDER 2

struct RectangleData
{
    CKDWORD m_InColor;
    CKMaterial *m_Material;
    CKDWORD m_BorderColor;
    float m_BorderSize;
    VxRect m_Screen;
    VxRect m_InUVs;

    RectangleData() : m_InColor(0), m_Material(NULL), m_BorderColor(0), m_BorderSize(0), m_Screen(), m_InUVs() {}
    RectangleData(CKBOOL in, const CKDWORD col, CKMaterial *mat, const VxRect &s, const VxRect &u, CKBOOL border, const CKDWORD borderColor, float bsize)
            : m_InColor(col), m_Material(mat), m_BorderColor(borderColor), m_Screen(s), m_InUVs(u) { m_BorderSize = border ? (in ? bsize : -bsize) : 0.0f; }
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
    VxRect m_TextZone;
    CKMaterial *m_Mat;
    int m_TextFlags;

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
                 m_TextFlags(0),
                 m_CaretSize(0.0f),
                 m_CaretMaterial(NULL) {}

    TextData(CK_ID entity, int fontIndex, CKSTRING string, Vx2DVector scale, CKDWORD startColor, CKDWORD endColor, int align, VxRect textZone, CKMaterial *mat, int textFlags)
        : m_Entity(entity), m_FontIndex(fontIndex), m_Scale(scale), m_StartColor(startColor), m_EndColor(endColor), m_Align(align), m_TextZone(textZone), m_Mat(mat), m_TextFlags(textFlags), m_CaretSize(0.0f), m_CaretMaterial(NULL)
    {
        m_String = CKStrdup(string);
    }

    ~TextData()
    {
        CKDeletePointer(m_String);
    }

    TextData(const TextData &td)
    {
        m_Entity = td.m_Entity;
        m_FontIndex = td.m_FontIndex;
        m_String = CKStrdup(td.m_String);
        m_Scale = td.m_Scale;
        m_EndColor = td.m_EndColor;
        m_StartColor = td.m_StartColor;
        m_Align = td.m_Align;
        m_Mat = td.m_Mat;
        m_TextFlags = td.m_TextFlags;
        m_CaretSize = td.m_CaretSize;
        m_CaretMaterial = td.m_CaretMaterial;
    }

    TextData &operator=(const TextData &td)
    {
        if (this != &td)
        {
            CKDeletePointer(m_String);
            m_Entity = td.m_Entity;
            m_FontIndex = td.m_FontIndex;
            m_String = CKStrdup(td.m_String);
            m_Scale = td.m_Scale;
            m_EndColor = td.m_EndColor;
            m_StartColor = td.m_StartColor;
            m_Align = td.m_Align;
            m_Mat = td.m_Mat;
            m_TextFlags = td.m_TextFlags;
            m_CaretSize = td.m_CaretSize;
            m_CaretMaterial = td.m_CaretMaterial;
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
    ////////////////////////////////////////////////////////
    //                Public Part                       ////
    ////////////////////////////////////////////////////////
public:
    ///
    // Virtual Override

    // Initialization
    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKEnd();

    virtual CKERROR SequenceToBeDeleted(CK_ID *objIds, int count);

    // Pause Resume
    virtual CKERROR OnCKReset();

    virtual CKERROR PostClearAll();
    virtual CKERROR PreProcess();
    virtual CKERROR PostLoad();

    virtual CKERROR LoadData(CKStateChunk *chunk, CKFile *LoadedFile);
    virtual CKStateChunk *SaveData(CKFile *SavedFile);

    virtual CKERROR OnPostRender(CKRenderContext *dev);
    virtual CKERROR OnPostSpriteRender(CKRenderContext *dev);

    virtual CKDWORD GetValidFunctionsMask() { return CKMANAGER_FUNC_OnSequenceToBeDeleted |
                                                     CKMANAGER_FUNC_PostClearAll |
                                                     CKMANAGER_FUNC_OnCKInit |
                                                     CKMANAGER_FUNC_PostLoad |
                                                     CKMANAGER_FUNC_OnCKEnd |
                                                     CKMANAGER_FUNC_OnCKReset |
                                                     CKMANAGER_FUNC_PreProcess |
                                                     CKMANAGER_FUNC_OnPostRender |
                                                     CKMANAGER_FUNC_OnPostSpriteRender; }

    // Virtual methods

    // Create a font from a texture
    virtual int CreateTextureFont(CKSTRING fontName, CKTexture *fontTexture, VxRect &textZone, Vx2DVector &charNumber, CKBOOL fixed = TRUE, int firstCharacter = 0, float iSpaceSize = 0.3f);

    // Get a font index
    virtual int GetFontIndex(CKSTRING name);

    // Get a font
    virtual CKTextureFont *GetFont(unsigned int fontIndex);

    // Create a logical font from a system font name
    virtual CKBOOL CreateFont(CKSTRING fontName, int systemFontIndex, int weight, CKBOOL italic, CKBOOL underline, int resolution, int iForcedSize);

    // Create a texture from a logical font
    virtual CKTexture *CreateTextureFromFont(int systemFontIndex, int resolution, CKBOOL extended, CKBOOL bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iFontSize = 0);

    // Draw a text
    virtual void DrawText(CKRenderContext *iRC, int iFontIndex, const char *iText, const Vx2DVector &iPosition, const Vx2DVector &iScale, CKDWORD iStartColor, CKDWORD iEndColor);

    // font deletion
    virtual void DeleteFont(int iFontIndex);

    // Methods

    // entity under mouse
    CK2dEntity *GetEntityUnderMouse() { return m_EntityUnderMouse; }

    // Text Lines
    void ClearLines();
    void AddLine(LineData &data);
    int GetLineCount();
    LineData *GetLine(int i);

    // Font Creation
    int RegisterFont(CKTextureFont *font);
    void RebuildFontEnumeration();

    // Compiled Text Data Access
    CompiledTextData *AddCompiledText(CK_ID id);
    void RemoveCompiledText(CK_ID id);
    CompiledTextData *GetCompiledText(CK_ID id);

    // Rectangles Drawing
    void DrawRectangle(const RectangleData &rd);
    void DrawText(CK_ID entity, int fontIndex, CKSTRING string, Vx2DVector scale, CKDWORD startColor, CKDWORD endColor, int align, VxRect textZone, CKMaterial *mat, int textFlags);

    // Generate font enumeration
    CKBOOL RegenerateFontEnumeration();
    // Delete a	 logical font
    void DeleteFontHandle(CKSTRING fontName);
    // Select a logical font by name
    CKBOOL SelectFont(CKSTRING fontName);
    // test if current font is a true type font
    CKBOOL IsTrueTypeFont();
    // Get a logical font metric
    CKBOOL GetFontMetrics(FONT_METRIC &metric);
    CKBOOL GetOutlineFontMetric(FONT_OUTLINEMETRIC &metric);
    // Get a font character widths
    CKBOOL GetCharABCWidths(CKBYTE startChar, CKBYTE endChar, FONT_ABC *ABCWidths);
    CKBOOL GetCharWidths(CKBYTE startChar, CKBYTE endChar, int *widths);

    int GetFontCount() { return m_FontArray.Size(); }

    ////////////////////////////////////////////////////////
    ////               Private Part                     ////
    ////////////////////////////////////////////////////////
    //{secret}
    CKFontManager(CKContext *ctx);
    //{secret}
    ~CKFontManager();

    static const char *Name;
    CKBOOL m_VersionUpdate;

private:
    void ClearFontData();
    void ClearTextData();
    void OnTextureToBeDeleted();
    void RegisterAttributes();
    // Post render Callback
    void DrawRectanglesCallback(CKRenderContext *dev);
    static void DrawTextCallback2D(CKRenderContext *dev, void *arg);
    static void DrawTextCallback3D(CKRenderContext *dev, void *arg);

    //////////////////////////////
    // Members
    /////////////////////////////

    ///
    // CONTROLS DATA

    // the 2D Entity currently under the mouse cursor
    CK2dEntity *m_EntityUnderMouse;

    ///
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
#ifndef FONTMANAGER_NOSYSFONT
    // Memory device context for font rendering
    HDC m_DC;
    // Associative table 'font name' -> 'font handle'
    FontsTable m_Fonts;
#endif
};

#endif
