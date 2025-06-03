/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              WriteInTexture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_WRITEMODE CKDEFINEGUID(0x675e3903, 0x7a06003c)

struct intcolor
{
    intcolor(){};
    intcolor(CKDWORD col)
    {
        r = ColorGetRed(col);
        g = ColorGetGreen(col);
        b = ColorGetBlue(col);
        a = ColorGetAlpha(col);
    }
    intcolor &operator+=(const intcolor &v)
    {
        r += v.r;
        g += v.g;
        b += v.b;
        a += v.a;
        return *this;
    }
    intcolor &operator*=(int v)
    {
        r *= v;
        g *= v;
        b *= v;
        a *= v;
        return *this;
    }
    intcolor &operator/=(int v)
    {
        r /= v;
        g /= v;
        b /= v;
        a /= v;
        return *this;
    }
    intcolor &operator>>=(int v)
    {
        r >>= v;
        g >>= v;
        b >>= v;
        a >>= v;
        return *this;
    }
    operator CKDWORD()
    {
        return RGBAITOCOLOR(r, g, b, a);
    }

    void Normalize()
    {
        if (r < 0)
            r = 0;
        else if (r > 255)
            r = 255;
        if (g < 0)
            g = 0;
        else if (g > 255)
            g = 255;
        if (b < 0)
            b = 0;
        else if (b > 255)
            b = 255;
        if (a < 0)
            a = 0;
        else if (a > 255)
            a = 255;
    }
    int r, g, b, a;
};

typedef void (*SetPixelFunction)(CKDWORD *DstPtr, CKDWORD Color);

void SetPixelReplace(CKDWORD *DstPtr, CKDWORD Color);
void SetPixelBlend(CKDWORD *DstPtr, CKDWORD Color);
void SetPixelAddSat(CKDWORD *DstPtr, CKDWORD Color);
void SetPixelAdd(CKDWORD *DstPtr, CKDWORD Color);

SetPixelFunction SetPixelFct[4] = {SetPixelReplace, SetPixelBlend, SetPixelAddSat, SetPixelAdd};

CKObjectDeclaration *FillBehaviorWriteInTextureDecl();
CKERROR CreateWriteInTextureProto(CKBehaviorPrototype **);
int WriteInTexture(const CKBehaviorContext &behcontext);
CKERROR WriteInTextureCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWriteInTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Write In Texture");
    od->SetDescription("Write a colored pixel in the texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>UV: </SPAN>the uv coordinates of the pixel to write.<BR>
    <SPAN CLASS=pin>Color: </SPAN>the color of the pixel to write.<BR>
    <SPAN CLASS=pin>Size: </SPAN>size of the point to draw.<BR>
    */
    /* warning:
    - You can get UV coordinates from the "2d Picking" building block located in the category "Interfaces".<BR>
    - This operation is destructive, you will lost your original texture unless you have set initial conditions on it.<BR>
    */
    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1d57024d, 0x7c064238));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWriteInTextureProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

void SetPixelReplace(CKDWORD *DstPtr, CKDWORD Color)
{
    *DstPtr = Color;
}

void SetPixelBlend(CKDWORD *DstPtr, CKDWORD Color)
{
    intcolor c1(*DstPtr);
    intcolor c2(Color);
    c1 += c2;
    c1 >>= 1;
    *DstPtr = (CKDWORD)c1;
}

void SetPixelAddSat(CKDWORD *DstPtr, CKDWORD Color)
{
    intcolor c1(*DstPtr);
    intcolor c2(Color);
    c1 += c2;
    c1.Normalize();
    *DstPtr = (CKDWORD)c1;
}

void SetPixelAdd(CKDWORD *DstPtr, CKDWORD Color)
{
    intcolor c1(*DstPtr);
    intcolor c2(Color);
    c1 += c2;
    *DstPtr = (CKDWORD)c1;
}

CKERROR CreateWriteInTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Write In Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("UV", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Color", CKPGUID_COLOR);
    proto->DeclareInParameter("Size", CKPGUID_INT, "1");
    proto->DeclareInParameter("Pen Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Write Mode", CKPGUID_WRITEMODE);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WriteInTexture);

    *pproto = proto;
    return CK_OK;
}

int WriteInTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;

    Vx2DVector uv;
    beh->GetInputParameterValue(0, &uv);
    VxColor col;
    beh->GetInputParameterValue(1, &col);
    int size = 1;
    beh->GetInputParameterValue(2, &size);
    CKTexture *pen_tex = (CKTexture *)beh->GetInputParameterObject(3);
    int WriteMode = 0;
    beh->GetInputParameterValue(4, &WriteMode);

    //----- Ensure Texture Coordinates are valid
    while (uv.x < 0.0f)
        uv.x += 1.0f;
    while (uv.x > 1.0f)
        uv.x -= 1.0f;
    while (uv.y < 0.0f)
        uv.y += 1.0f;
    while (uv.y > 1.0f)
        uv.y -= 1.0f;

#define SetPixel(x, y, scol) ptr[y * (TexDesc.BytesPerLine >> 2) + x] = scol

    VxImageDescEx TexDesc;
    tex->GetSystemTextureDesc(TexDesc);
    CKDWORD *ptr = (CKDWORD *)tex->LockSurfacePtr();
    CKDWORD dcol = RGBAFTOCOLOR(&col);
    if (ptr && (TexDesc.BitsPerPixel == 32))
    {
        int sx = (int)(uv.x * TexDesc.Width), sy = (int)(uv.y * TexDesc.Height);
        if (pen_tex)
        {
            VxImageDescEx PenTexDesc;
            CKDWORD *sptr = (CKDWORD *)pen_tex->LockSurfacePtr();
            pen_tex->GetSystemTextureDesc(PenTexDesc);
            if (sptr && PenTexDesc.BitsPerPixel == 32)
            {
                int WidthOn2 = PenTexDesc.Width / 2;
                int HeightOn2 = PenTexDesc.Height / 2;
                int x0 = sx - WidthOn2;
                int y0 = sy - HeightOn2;
                int x1 = sx + WidthOn2 - 1;
                int y1 = sy + HeightOn2 - 1;
                if (x0 < 0)
                {
                    sptr += (-x0);
                    x0 = 0;
                }
                if (y0 < 0)
                {
                    sptr += (-y0 * PenTexDesc.Width);
                    y0 = 0;
                }
                if (x1 >= TexDesc.Width)
                    x1 = TexDesc.Width - 1;
                if (y1 >= TexDesc.Height)
                    y1 = TexDesc.Height - 1;

                // Go to first pixel
                int cwidth = ((x1 - x0) + 1);
                ptr += y0 * TexDesc.Width;
                for (int i = y0; i <= y1; ++i, sptr += PenTexDesc.Width, ptr += TexDesc.Width)
                    for (int j = 0; j < cwidth; ++j)
                        SetPixelFct[WriteMode](ptr + j + x0, sptr[j]);
            }
        }
        else if (size == 1)
            SetPixelFct[WriteMode](&ptr[sy * TexDesc.Width + sx], dcol);
        else
        {
            int x0 = sx - size;
            int y0 = sy - size;
            int x1 = sx + size;
            int y1 = sy + size;
            if (x0 < 0)
                x0 = 0;
            if (y0 < 0)
                y0 = 0;
            if (x1 >= TexDesc.Width)
                x1 = TexDesc.Width - 1;
            if (y1 >= TexDesc.Height)
                y1 = TexDesc.Height - 1;

            ptr += TexDesc.Width * y0 + x0;
            for (int j = y0; j <= y1; ++j, ptr += TexDesc.Width)
            {
                CKDWORD *ptr2 = ptr;
                for (int i = x0; i <= x1; ++i, ptr2++)
                    SetPixelFct[WriteMode](ptr2, dcol);
            }
        }
    }
    tex->ReleaseSurfacePtr();

    return CKBR_OK;
}
