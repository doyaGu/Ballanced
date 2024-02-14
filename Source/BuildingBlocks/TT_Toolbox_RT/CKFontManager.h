#ifndef CK_FONTMANAGER_H
#define CK_FONTMANAGER_H

#include "CKAll.h"

#define FONT_MANAGER_GUID CKGUID(0x64fb5810, 0x73262d3b)
#define CKPGUID_FONT CKGUID(0x64fb5811, 0x33862d3b)

class CKTextureFont;

class CKFontManager : public CKBaseManager
{
public:
#if CKVERSION == 0x13022002
    // Create a font from a texture
    virtual int CreateTextureFont(CKSTRING FontName, CKTexture *fonttexture, VxRect &tzone, Vx2DVector &charnumber, CKBOOL fixed = TRUE, int firstcharacter = 0, float iSpaceSize = 0.3f) = 0;

    // Get a font index
    virtual int GetFontIndex(CKSTRING name) = 0;

    // Get a font
    virtual CKTextureFont *GetFont(unsigned int fontindex) = 0;

    // Create a logical font from a system font name
    virtual CKBOOL CreateFont(CKSTRING fontName, int systemFontIndex, int weigth, CKBOOL italic, CKBOOL underline, int resolution, int iForcedSize) = 0;

    // Create a texture from a logical font
    virtual CKTexture *CreateTextureFromFont(int systemFontIndex, int resolution, CKBOOL extended, CKBOOL bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iFontSize = 0) = 0;
#else
    // Get a font index
    virtual int GetFontIndex(CKSTRING name) = 0;

    // Get a font
    virtual CKTextureFont *GetFont(unsigned int fontindex) = 0;

    // Create a font from a texture
    virtual int CreateTextureFont(CKSTRING FontName,CKTexture* fonttexture,VxRect& tzone,Vx2DVector& charnumber,CKBOOL fixed=TRUE,int firstcharacter = 0,float iSpaceSize = 0.3f) = 0;

    // Create a logical font from a system font name
    virtual CKBOOL CreateFont(CKSTRING fontName, int systemFontIndex, int weigth, CKBOOL italic, CKBOOL underline, int resolution,int iForcedSize) = 0;

    // Create a texture from a logical font
    virtual CKTexture *CreateTextureFromFont(int systemFontIndex , int resolution, CKBOOL extended, CKBOOL bold, CKBOOL italic, CKBOOL underline, CKBOOL renderControls, CKBOOL dynamic, int iFontSize = 0) = 0;

    // Draw a text
    virtual void DrawText(CKRenderContext* iRC, int iFontIndex, const char* iText, const Vx2DVector& iPosition, const Vx2DVector& iScale, CKDWORD iStartColor, CKDWORD iEndColor) = 0;
#endif
};

#endif // CK_FONTMANAGER_H
