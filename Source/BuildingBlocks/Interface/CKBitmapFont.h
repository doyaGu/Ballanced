#ifndef CKBITMAPFONT_H
#define CKBITMAPFONT_H

#include "CKAll.h"

class CKBitmapFont
{
public:
    CKBitmapFont();
    ~CKBitmapFont();

    // return 0 if the sprite was not a valid font
    int CreateFont(CK_ID sprite, CKContext *ctx);
    int StringWidth(char *str, int s, Vx2DVector &scale);
    int DrawCharacter(CKRenderContext *context, int i, int x, int y, Vx2DVector &scale);
    void DrawString(CKRenderContext *context, char *str, int x, int y, int s, Vx2DVector &scale);
    void MaskSeparationLines(CKBOOL mask, CKContext *ctx);

    int GetHeight() { return m_Height; }
    CKBOOL HasFont(CKContext *ctx) { return ctx->GetObject(m_FontBitmap) != NULL; }
    void SetFont(CK_ID id) { m_FontBitmap = id; }
    CKSprite *GetFont(CKContext *ctx) { return (CKSprite *)ctx->GetObject(m_FontBitmap); }

public:
    struct characterProperties
    {
        int pos;
        int width;
    };

    CK_ID m_FontBitmap;
    characterProperties m_CorrespondenceTable[256];
    int m_Height;
    // Colors key of the fonts
    CKDWORD m_SeparationColor;
    CKDWORD m_FillColor;
};

#endif