#include "CKAll.h"
#include "CKBitmapFont.h"

CKBitmapFont::CKBitmapFont() : m_FontBitmap(0), m_Height(0)
{
    for (int i = 0; i < 256; i++)
    {
        m_CorrespondenceTable[i].pos = -1;
        m_CorrespondenceTable[i].width = 0;
    }
}

void VerticalLine(CKSprite *sprite, int x, int height, CKDWORD col)
{
    for (int i = 0; i < height; i++)
    {
        sprite->SetPixel(x, i, col);
    }
}

int CKBitmapFont::CreateFont(CK_ID sprite, CKContext *ctx)
{
    m_FontBitmap = sprite;
    CKSprite *fontBitmap = (CKSprite *)ctx->GetObject(sprite);
    if (!fontBitmap)
        return 0;

    fontBitmap->GetCKContext()->OutputToConsole("Creating Font");

    // We get the height
    m_Height = fontBitmap->GetHeight();

    // We get the width
    int width = fontBitmap->GetWidth();

    int x = 0;

    // First we must find the separating color
    CKDWORD col;
    m_SeparationColor = fontBitmap->GetPixel(x++, 0);

    // it should be the space character there, so we take this color as color key...
    m_FillColor = fontBitmap->GetPixel(x, 0);

    // If the 2 colors are the same, the font is invalid
    if (m_SeparationColor == m_FillColor)
    {
        m_FontBitmap = 0;
        ctx->OutputToConsole("Wrong Bitmap Font");
        return 0;
    }

    // read all letters
    int pos = 1;
    int cw;
    for (int i = 32; i < '~' + 1; i++)
    {
        // we advance till we found the other separator
        cw = -1;
        do
        {
            col = fontBitmap->GetPixel(x++, 0);
            cw++;
        } while (col != m_SeparationColor);
        m_CorrespondenceTable[i].width = cw;
        m_CorrespondenceTable[i].pos = pos;
        pos += (cw + 1);
        if (pos >= width)
            break;
    }

    return 1;
}

CKBitmapFont::~CKBitmapFont() {}

void CKBitmapFont::MaskSeparationLines(CKBOOL mask, CKContext *ctx)
{
    // We test if there is a font attached
    CKSprite *fontBitmap = (CKSprite *)ctx->GetObject(m_FontBitmap);
    if (!fontBitmap)
        return;

    CKDWORD col = (mask) ? m_FillColor : m_SeparationColor;

    for (int i = 32; i < '~' + 1; i++)
    {
        VerticalLine(fontBitmap, m_CorrespondenceTable[i].pos - 1, m_Height, col);
    }
}

int CKBitmapFont::DrawCharacter(CKRenderContext *context, int i, int x, int y, Vx2DVector &scale)
{
    if (m_CorrespondenceTable[i].pos != -1)
    {
        CKSprite *fontBitmap = (CKSprite *)context->GetCKContext()->GetObject(m_FontBitmap);
        if (!fontBitmap)
            return 0;
        VxRect srcrect;
        srcrect.left = (float)m_CorrespondenceTable[i].pos;
        srcrect.top = 0;
        srcrect.right = srcrect.left + m_CorrespondenceTable[i].width;
        srcrect.bottom = (float)m_Height;
        VxRect rect;
        rect.left = (float)x;
        rect.top = (float)y;
        rect.right = (float)(x + (int)(m_CorrespondenceTable[i].width * scale.x));
        rect.bottom = (float)(y + (int)((m_Height)*scale.y));
        fontBitmap->SetSourceRect(srcrect);
        fontBitmap->SetRect(rect);
        fontBitmap->Render(context);
        return m_CorrespondenceTable[i].width;
    }
    return 0;
}

void CKBitmapFont::DrawString(CKRenderContext *context, char *str, int x, int y, int s, Vx2DVector &scale)
{
    CKSprite *fontBitmap = (CKSprite *)context->GetCKContext()->GetObject(m_FontBitmap);
    if (!fontBitmap)
        return;

    fontBitmap->UseSourceRect(TRUE);
    fontBitmap->Show(CKSHOW);

    while (*str != '\0')
    {
        x += (int)(scale.x * DrawCharacter(context, *str, x, y, scale));
        x += (int)(scale.x * s);
        ++str;
    }

    fontBitmap->UseSourceRect(FALSE);
    fontBitmap->Show(CKHIDE);
    VxRect rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = (float)fontBitmap->GetWidth();
    rect.bottom = (float)fontBitmap->GetHeight();
    fontBitmap->SetRect(rect);
}

int CKBitmapFont::StringWidth(char *str, int s, Vx2DVector &scale)
{
    int x = 0;
    while (*str != '\0')
    {
        x += (int)(scale.x * m_CorrespondenceTable[*str].width);
        x += (int)(scale.x * s);
        str++;
    }

    return x;
}
