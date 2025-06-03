/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              TextureProcessing
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "TextureProcessing.h"

// Convolutions Matrix
// Gaussian Blur Matrix
int CMAntialias[3][3] = {1, 2, 1, 2, 8, 2, 1, 2, 1};
// Gaussian Blur Matrix
int CMGaussianBlur[3][3] = {2, 4, 2, 4, 0, 4, 2, 4, 2};
// Gaussian Blur Matrix
int CMUniformBlur[3][3] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
// Gaussian Blur Matrix
int CMDirectionnal[3][3] = {4, 3, 2, 3, 2, 2, 2, 1, 0};
// Emboss Matrix
int CMEmboss[3][3] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
// Passe haut Matrix
int CMHighPass[3][3] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
// Passe bas Matrix
int CMLowPass[3][3] = {0, 1, 0, 1, 3, 1, 0, 1, 0};

// Decal Matrices
int CMDecalLeft[3][3] = {0, 0, 0, 1, 0, 0, 0, 0, 0};
int CMDecalRight[3][3] = {0, 0, 0, 0, 0, 1, 0, 0, 0};
int CMDecalTop[3][3] = {0, 0, 0, 0, 0, 0, 0, 1, 0};
int CMDecalBottom[3][3] = {0, 1, 0, 0, 0, 0, 0, 0, 0};
int CMDecalNone[3][3] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
int CMDecalBR[3][3] = {1, 0, 0, 0, 0, 0, 0, 0, 0};
int CMDecalBL[3][3] = {0, 0, 1, 0, 0, 0, 0, 0, 0};
int CMDecalTL[3][3] = {0, 0, 0, 0, 0, 0, 0, 0, 1};
int CMDecalTR[3][3] = {0, 0, 0, 0, 0, 0, 1, 0, 0};

CKDWORD *g_Buffer = NULL;
int g_Size = 0;

CKDWORD ColorMask[2] = {0x00FF00FF, 0x00FF00FF};

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

//*************************************************************************
//  ApplyMatrixToTexture
//  C++ Implementation
//*************************************************************************
void ApplyMatrixToTexture(CKTexture *tex, int matrix[3][3], int sum)
{
    if (!tex)
        return;

    if (!sum)
    { // The user wants us to calculate the sum
        for (int a = 0; a < 3; ++a)
            for (int b = 0; b < 3; ++b)
                sum += matrix[a][b];
    }
    if (!sum)
        sum = 1;

    int width = tex->GetWidth();
    int height = tex->GetHeight();
    CKDWORD *image = (CKDWORD *)tex->LockSurfacePtr();

#if !defined(macintosh) // || defined (__i386__)

    if (GetProcessorFeatures() & PROC_MMX)
    {
        // Prepare Matrix valable uniquement pour sum!=0
        // Scale la matrice de facon a ce que sum=256
        short int SMat[3][3];
        for (int a = 0; a < 3; ++a)
            for (int b = 0; b < 3; ++b)
            {
                short int val = (int)(matrix[a][b] * 256) / (int)sum;
                SMat[a][b] = val;
            }
        // Tableau de short int prepar�s dans l'ordre utilis� par la routine MMX
        short int MatrixVals[24] = {
            SMat[0][1], SMat[0][0], SMat[0][1], SMat[0][0], SMat[0][2], SMat[0][2], SMat[0][2], SMat[0][2],
            SMat[1][1], SMat[1][0], SMat[1][1], SMat[1][0], SMat[1][2], SMat[1][2], SMat[1][2], SMat[1][2],
            SMat[2][1], SMat[2][0], SMat[2][1], SMat[2][0], SMat[2][2], SMat[2][2], SMat[2][2], SMat[2][2]};

        // recopie les couleurs precedentes dans un buffer un poil plus large
        int ssize = (width + 2) * (height + 2);
        int dsize = (width) * (height);

        if (!g_Buffer || ssize > g_Size)
        {
            if (g_Buffer)
                delete[] g_Buffer;
            g_Buffer = new CKDWORD[ssize];
            g_Size = ssize;
        }
        CKDWORD *oldcolors = g_Buffer;
        int dindex = width + 3, sindex = 0, pitch = width * sizeof(CKDWORD);
        for (int i = 0; i < height; ++i, dindex += width + 2, sindex += width)
        {
            oldcolors[dindex - 1] = image[sindex];				   // copy left
            oldcolors[dindex + width] = image[sindex + width - 1]; //	copy right
            memcpy(&oldcolors[dindex], &image[sindex], pitch);
        }
        memcpy(&oldcolors[1], image, pitch);													 // copy top
        memcpy(&oldcolors[(height + 1) * (width + 2) + 1], &image[(height - 1) * width], pitch); // copy bottom
                                                                                                 // corners
        oldcolors[0] = image[0];
        oldcolors[width + 1] = image[width - 1];
        oldcolors[ssize - 1] = image[dsize - 1];
        oldcolors[ssize - (width + 2)] = image[dsize - width];

        ProcessPixelsMMX(oldcolors, image, width, height, MatrixVals);

        // TODO : callback pour deleter le global
        // delete [] oldcolors;
    }
    else
#endif // #if !defined(macintosh)  || defined (__i386__)

    {
        CKDWORD *newcolors = new CKDWORD[width * height];
        CKDWORD *colptr = newcolors;
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                int im1 = i - 1;
                if (i == 0)
                    im1 = 0;

                int ip1 = i + 1;
                if (i == height - 1)
                    ip1 = height - 1;

                int jm1 = j - 1;
                if (i == 0)
                    im1 = 0;

                int jp1 = j + 1;
                if (i == width - 1)
                    jp1 = width - 1;

                intcolor sumcol(0);
                intcolor oldcol;

                oldcol = intcolor(image[jm1 + (im1)*width]);
                oldcol *= matrix[0][0];
                sumcol += oldcol;

                oldcol = intcolor(image[jm1 + (i)*width]);
                oldcol *= matrix[0][1];
                sumcol += oldcol;

                oldcol = intcolor(image[jm1 + (ip1)*width]);
                oldcol *= matrix[0][2];
                sumcol += oldcol;

                oldcol = intcolor(image[j + (im1)*width]);
                oldcol *= matrix[1][0];
                sumcol += oldcol;

                oldcol = intcolor(image[j + (i)*width]);
                oldcol *= matrix[1][1];
                sumcol += oldcol;

                oldcol = intcolor(image[j + (ip1)*width]);
                oldcol *= matrix[1][2];
                sumcol += oldcol;

                oldcol = intcolor(image[jp1 + (im1)*width]);
                oldcol *= matrix[2][0];
                sumcol += oldcol;

                oldcol = intcolor(image[jp1 + (i)*width]);
                oldcol *= matrix[2][1];
                sumcol += oldcol;

                oldcol = intcolor(image[jp1 + (ip1)*width]);
                oldcol *= matrix[2][2];
                sumcol += oldcol;

                sumcol /= sum;

                *colptr = RGBAITOCOLOR(sumcol.r, sumcol.g, sumcol.b, sumcol.a);
                colptr++;
            }
        }

        memcpy(image, newcolors, width * height * 4);

        delete[] newcolors;
    }

    tex->ReleaseSurfacePtr();
}

#if defined(WIN32) //  || (defined(macintosh) && defined(__i386__))

//*************************************************************************
//  SrcData doit etre de taille (Width+2)*(Height+2)
//	DstData doit etre de taille (Width*Height)
//
//
//*************************************************************************
void ProcessPixelsMMX(void *SrcData, void *DstData, int width, int height, void *MatrixData)
{
    __asm {
        // Matrix Data: Array de 6 x 64 bits ou 24 short :
        // ex : elements 0 et 1 : MM4 = (00M1|00M0|00M1|00M0)
        //		elements 2		  MM4 = (0000|00M2|0000|00M2)
        // etc.

        // MM6 et MM7 => accumulateurs : => MM6 = R(32)|G(32) et MM7 = B(32)|A(32)
        // Pixels : 0  1  2
        //			3  4  5
        //			6  7  8
        pushad
        mov esi,SrcData
        mov edi,DstData
        mov ebx,MatrixData
        mov edx,height
        mov ecx,width
        add ecx,2
        shl ecx,2
        add esi,ecx // Place Esi to source data[1][1] equivalent a (0,0)	

        pxor MM2,MM2

BoucleH:
        mov eax,width
        add esi,4
BoucleW:
        push esi
        sub esi,ecx // previous line 

        movq MM6,MM2
        movq MM7,MM2
            // Load matrix data for pixels (0,1)
        movq		MM4,QWORD PTR [ebx]
        // Process first two pixels (0,1) :
        movq		MM0,QWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
            // Load matrix data for pixel (2)
        movq		MM4,QWORD PTR [ebx+8]
        // Process next pixel (2) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
            // Next Line 
        add			esi,ecx
            // Load matrix data for pixels (3,4)
        movq		MM4,QWORD PTR [ebx+16]
        // Process first two pixels (3,4):
        movq		MM0,QWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
            // Load matrix data for pixel (5)
        movq		MM4,QWORD PTR [ebx+24]
        // Process next pixel (5) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
            // Last Line 
        add			esi,ecx
            // Load matrix data for pixels (6,7)
        movq		MM4,QWORD PTR [ebx+32]
        // Process first two pixels (6,7) :
        movq		MM0,DWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //

            // Load matrix data for pixel (8)
        movq		MM4,QWORD PTR [ebx+40]
        // Process next pixel (8) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //  

        pop esi

                // Conversion to 32 bits
        psrad		MM7,8 // Divise par 256 ( decalage )
        psrad		MM6,8 // Divise par 256 ( decalage )
        packssdw	MM7,MM6 // MM7 & MM6 (Dword => Words) dans MM7
            // MM7=00|RR|00|GG|00|BB|00|AA 	

        packuswb	MM7,MM7 // Convertit les word en bytes avec clamping (0..255)
            // MM7=RR|GG|BB|AA|RR|GG|BB|AA 	
        
        movd		DWORD PTR [edi],MM7 // Store in destination
        add edi,4
        add esi,4

        dec eax
        jz SuiteW
        jmp BoucleW
SuiteW:
        add esi,4
        dec edx
        jz finTot
        jmp BoucleH
finTot:
        EMMS
        popad
    }
}

void ExchangeMemoryMMX(void *Data1, void *Data2, CKDWORD sizet)
{
    __asm {
        mov		esi,Data1
        mov		edi,Data2
        mov		ecx,sizet

        shr		ecx,4 // 4 pixel at a time
LoopExchange:
        movq		MM0,QWORD PTR [esi]
        movq		MM1,QWORD PTR [esi+8]
        movq		MM2,QWORD PTR [edi]
        movq		MM3,QWORD PTR [edi+8]
        
        movq		QWORD PTR [edi],MM0
        movq		QWORD PTR [edi+8],MM1

        movq		QWORD PTR [esi],MM2
        movq		QWORD PTR [esi+8],MM3
        add		esi,16
        add		edi,16
        dec		ecx
        jnz LoopExchange
        EMMS
    }
}

//*************************************************************************
//  SrcData doit etre de taille (Width)*(Height)
//	DstData doit etre de taille (Width*Height)
//
//	SrcData represente les donn�es de la frame pr�c�dente
//*************************************************************************

void ProcessPixelsTemporalMMX(void *CurrentData, void *PreviousData, int width, int height, void *MatrixData, int Damping)
{
    __asm {
        // Matrix Data: Array de 6 x 64 bits ou 24 short :
        // ex : elements 0 et 1 : MM4 = (00M1|00M0|00M1|00M0)
        //		elements 2		  MM4 = (0000|00M2|0000|00M2)
        // etc.

        // MM6 et MM7 => accumulateurs : => MM6 = R(32)|G(32) et MM7 = B(32)|A(32)
        // Pixels : 0  1  2
        //			3  4  5
        //			6  7  8
        pushad
        mov esi,CurrentData
        mov edi,PreviousData
        mov ebx,MatrixData
        mov edx,height
        sub edx,2
        mov ecx,width
        shl ecx,2

        pxor MM2,MM2

BoucleH:
        mov eax,width
        sub eax,2
BoucleW:
        push esi
        sub esi,ecx // previous line 

        movq MM6,MM2
        movq MM7,MM2
        // Load matrix data for pixels (0,1)
        movq		MM4,QWORD PTR [ebx]
        // Process first two pixels (0,1) :
        movq		MM0,QWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
        // Load matrix data for pixel (2)
        movq		MM4,QWORD PTR [ebx+8]
        // Process next pixel (2) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
        // Next Line
        add			esi,ecx
        // Load matrix data for pixels (3,4)
        movq		MM4,QWORD PTR [ebx+16]
        // Process first two pixels (3,4):
        movq		MM0,QWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
        // Load matrix data for pixel (5)
        movq		MM4,QWORD PTR [ebx+24]
        // Process next pixel (5) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //
        // Last Line
        add			esi,ecx
        // Load matrix data for pixels (6,7)
        movq		MM4,QWORD PTR [ebx+32]
        // Process first two pixels (6,7) :
        movq		MM0,DWORD PTR [esi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        movq		MM1,MM0	
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        punpcklbw	MM1,MM2 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movq		MM3,MM0					
        punpckhwd	MM3,MM1 // MM3=00|R1|00|R0|00|G1|00|G0	Mixe Pixels avant mul (H)
        punpcklwd	MM0,MM1 // MM0=00|B1|00|B0|00|A1|00|A0	Mixe Pixels avant mul (L)
        pmaddwd		MM3,MM4 // MM3=R1*M1+R0*M0|G1*M1+G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B1*M1+B0*M0|A1*M1+A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //

        // Load matrix data for pixel (8)
        movq		MM4,QWORD PTR [ebx+40]
        // Process next pixel (8) :
        movd		MM0,DWORD PTR [esi+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        punpcklbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        movq		MM3,MM0					
        punpckhwd	MM3,MM2 // MM3=00|00|00|R0|00|00|00|G0	Mixe Pixel 0 et 1 
        punpcklwd	MM0,MM2 // MM0=00|00|00|B0|00|00|00|A0	Mixe Pixel 0 et 1 	
        pmaddwd		MM3,MM4 // MM3=R0*M0	  |		 G0*M0  Multiplication et addition
        pmaddwd		MM0,MM4 // MM0=B0*M0	  |		 A0*M0	Multiplication et addition
        paddd		MM6,MM3 // Ajout aux accumulateurs
        paddd		MM7,MM0 //  

        pop esi

        // Conversion to 32 bits
        psrad		MM7,8 // Divise par 256 ( decalage )
        psrad		MM6,8 // Divise par 256 ( decalage )
        packssdw	MM7,MM6 // MM7 & MM6 (Dword => Words) dans MM7
        // MM7=00|RR|00|GG|00|BB|00|AA

        //--- Load Destination color
        movq		MM0,QWORD PTR [edi-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load destination pixels
        punpckhbw	MM0,MM2 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)

        //-----
        paddusw		MM7,MM7 // MM7=2*MM7	
        psubusw		MM7,MM0

        //-----------------------------------------------------------
        movq		MM6,MM7			
        movd		MM0,DWORD ptr Damping
        psrlw		MM6,MM0 // Divide Res by damping
        psubusw		MM7,MM6 // Res= Res - res*Damping

        packuswb	MM7,MM7 // Convertit les word en bytes avec clamping (0..255)
        // MM7=RR|GG|BB|AA|RR|GG|BB|AA

        movd		DWORD PTR [edi],MM7 // Store in destination
        add edi,4
        add esi,4

        dec eax
        jz SuiteW
        jmp BoucleW
SuiteW:
        add edi,8
        add esi,8

        dec edx
        jz finTot
        jmp BoucleH
finTot:
        EMMS
        popad
    }
}

//*************************************************************************
//  SrcData doit etre de taille (Width)*(Height)
//	DstData doit etre de taille (Width*Height)
//
//  Applique un filtre sur CurrentData  NewP(x,y)= 2*Voisinage(CurrentP(x,y)) -  PreviousP(x,y);
//  Le resultat est �crit dans Previous Data
//*************************************************************************
void WaterEffectMMX(void *CurrentData, void *PreviousData, int width, int height, CKDWORD BorderColor, int Damping)
{
    __asm {
        pushad
        mov esi,CurrentData
        mov edi,PreviousData
        mov ecx,width
        shl ecx,2

        mov edx,height
        sub edx,2

        pxor MM7,MM7
        movd MM6,DWORD ptr Damping
        movd MM5,DWORD ptr BorderColor
        punpcklbw	MM5,MM7						
        movq		MM4,MM5
        paddusw     MM4,MM5

        //--------- First line pixels don't have a top neighbour pixel
        // Top Left Pixel
        movd		MM1,DWORD PTR [esi+4]	
        movd		MM3,DWORD PTR [esi+ecx]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        mov eax,width
        sub eax,2

        // Top Pixels
FirstLine:
        movd		MM1,DWORD PTR [esi-4]			
        movd		MM2,DWORD PTR [esi+4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi+ecx]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        dec eax
        jnz FirstLine

        // Top Right Pixel
        movd		MM1,DWORD PTR [esi-4]	
        movd		MM3,DWORD PTR [esi+ecx]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        sub esi,ecx
BoucleH:
        mov eax,width
        sub eax,2

        // First Pixel of the raw => No left neightbourg
        movd		MM1,DWORD PTR [esi]			
        movd		MM2,DWORD PTR [esi+ecx+4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi+ecx*2]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

BoucleW:

        // Process upper pixel (0,-1) :
        movd		MM0,DWORD PTR [esi] // MM0=00|00|00|00|R0|G0|B0|A0	Load pixels
        movd		MM1,DWORD PTR [esi+ecx-4] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        movd		MM2,DWORD PTR [esi+ecx+4] // MM0=R0|G0|B0|A0|00|00|00|00	Load pixels
        
        punpcklbw	MM1,MM7 // MM1=00|R1|00|G1|00|B1|00|A1	Transfo Byte->Word (L)	 	
        movd		MM3,DWORD PTR [esi+ecx*2] // MM0=R0|G0|B0|A0|R1|G1|B1|A1	Load pixels
        
        punpcklbw	MM2,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (L)	
        
        paddusw       MM1,MM0
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	

        paddusw       MM3,MM2
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels

        paddusw       MM1,MM3
        psrlw		MM1,2 // Divise par 4 ( moyenne )

        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)

        //-----
        paddusw		MM1,MM1
        psubusw		MM1,MM0

        //-----------------------------------------------------------
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping

        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)

        add esi,4
        movd		DWORD PTR [edi],MM1 // Store in destination
        add edi,4

        dec eax
        jz SuiteW
        jmp BoucleW
SuiteW:
        // Last Pixel of the raw => No right neightbourg
        movd		MM1,DWORD PTR [esi]			
        movd		MM2,DWORD PTR [esi+ecx-4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi+ecx*2]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        dec edx
        jz finTot
        jmp BoucleH
finTot:
        //		sub esi,ecx
        //--------- Last line pixels don't have a bottom neighbour pixel
        // Bottom Left Pixel
        movd		MM1,DWORD PTR [esi]	
        movd		MM3,DWORD PTR [esi+ecx+4]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        mov eax,width
        sub eax,2

        // Top Pixels
LastLine:
        movd		MM1,DWORD PTR [esi+ecx-4]			
        movd		MM2,DWORD PTR [esi+ecx+4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        dec eax
        jnz LastLine

        // Top Right Pixel
        movd		MM1,DWORD PTR [esi+ecx-4]	
        movd		MM3,DWORD PTR [esi]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        EMMS
        popad
    }
}

//*************************************************************************
//  SrcData doit etre de taille (Width)*(Height)
//	DstData doit etre de taille (Width*Height)
//  Both must be aligned on 16 bytes booundary
//
//  Applique un filtre sur CurrentData  NewP(x,y)= 2*Voisinage(CurrentP(x,y)) - PreviousP(x,y);
//  Le resultat est �crit dans Previous Data
//*************************************************************************
void WaterEffectWillamette(void *CurrentData, void *PreviousData, int width, int height, CKDWORD BorderColor, int Damping)
{
    __asm {
        pushad
        mov esi,CurrentData
        mov edi,PreviousData
        mov ecx,width
        shl ecx,2

        mov edx,height
        sub edx,2

        pxor MM7,MM7
        movd MM6,DWORD ptr Damping
        movd MM5,DWORD ptr BorderColor
        punpcklbw	MM5,MM7						
        movq		MM4,MM5
        paddusw     MM4,MM5
        xorps		xmm7,xmm7
        movq2dq     xmm6,mm6

        //--------- First line pixels don't have a top neighbour pixel
        // Top Left Pixel
        movd		MM1,DWORD PTR [esi+4]	
        movd		MM3,DWORD PTR [esi+ecx]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        mov eax,width
        sub eax,2

        // Top Pixels
FirstLine:
        movd		MM1,DWORD PTR [esi-4]			
        movd		MM2,DWORD PTR [esi+4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi+ecx]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        dec eax
        jnz FirstLine

        // Top Right Pixel
        movd		MM1,DWORD PTR [esi-4]	
        movd		MM3,DWORD PTR [esi+ecx]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        sub esi,ecx
BoucleH:
        mov eax,width
        sub eax,8
        movaps		xmm0,[esi] // Load upper line  ( T0|T1|T2|T3 )
        movaps		xmm2,[esi+ecx*2] // Load Lower line  ( B0|B1|B2|B3 )

        movaps		xmm1,[esi+ecx]		
        pavgb       xmm0,xmm2

        movaps		xmm3,xmm1 // backup same line 
        movaps		xmm4,xmm0

        pslldq		xmm1,4 // xmm1 =  (  ??|N0|N1|N2 )
        movss		xmm5,DWORD ptr BorderColor // xmm5 =  (  N-1|0|0|0 )
        
        punpcklbw	xmm0,xmm7					
        movss		xmm2,[esi+ecx+16]			

        punpckhbw	xmm4,xmm7					
        movss		xmm1,xmm5

        movss		xmm3,xmm2
        pshufd		xmm3,xmm3,0x39
        
        movaps		xmm5,[edi] // Load destination ( D0|D1|D2|D3 )
        pavgb		xmm1,xmm3
        
        movaps		xmm3,xmm5 // Store dest
        movaps		xmm2,xmm1

        add esi,16

        punpcklbw	xmm1,xmm7					

        prefetcht1	[esi+16]	
        prefetcht1	[esi+ecx+16]
        prefetcht1	[esi+ecx*2+16]

        punpckhbw	xmm2,xmm7					
        paddusw		xmm0,xmm1		

        punpcklbw	xmm3,xmm7					
        paddusw		xmm2,xmm4		
        
        psrlw		xmm0,1
        punpckhbw	xmm5,xmm7					
        
        paddusw		xmm0,xmm0		
        psrlw		xmm2,1
        
        psubusw		xmm0,xmm3
        paddusw		xmm2,xmm2

        movaps		xmm3,xmm0			
        psubusw		xmm2,xmm5

        //------------------- Damping ----------------------------------------
        movaps		xmm4,xmm2
        
        psrlw		xmm3,xmm6 // Divide Res by damping
        psrlw		xmm4,xmm6 // Divide Res by damping

        psubusw		xmm0,xmm3 // Res= Res - res*Damping
        psubusw		xmm2,xmm4 // Res= Res - res*Damping

        packuswb	xmm0,xmm2 // Convertit les word en bytes avec clamping (0..255)
        movaps		[edi],xmm0 // Store in destination

        add edi,16

BoucleW:
        movaps		xmm0,[esi] // Load upper line  ( T0|T1|T2|T3 )
        movaps		xmm2,[esi+ecx*2] // Load Lower line  ( B0|B1|B2|B3 )

        movaps		xmm1,[esi+ecx]		
        pavgb       xmm0,xmm2

        movaps		xmm3,xmm1 // backup same line 
        movaps		xmm4,xmm0

        pslldq		xmm1,4 // xmm1 =  (  ??|N0|N1|N2 )
        movss		xmm5,[esi+ecx-4] // xmm5 =  (  N-1|0|0|0 )
        
        punpcklbw	xmm0,xmm7					
        movss		xmm2,[esi+ecx+16]			

        punpckhbw	xmm4,xmm7					
        movss		xmm1,xmm5

        movss		xmm3,xmm2
        pshufd		xmm3,xmm3,0x39
        
        movaps		xmm5,[edi] // Load destination ( D0|D1|D2|D3 )
        pavgb		xmm1,xmm3
        
        movaps		xmm3,xmm5 // Store dest
        movaps		xmm2,xmm1

        add esi,16

        punpcklbw	xmm1,xmm7					

        prefetcht1	[esi+16]	
        prefetcht1	[esi+ecx+16]
        prefetcht1	[esi+ecx*2+16]

        punpckhbw	xmm2,xmm7					
        paddusw		xmm0,xmm1		

        punpcklbw	xmm3,xmm7					
        paddusw		xmm2,xmm4		
        
        psrlw		xmm0,1
        punpckhbw	xmm5,xmm7					
        
        paddusw		xmm0,xmm0		
        psrlw		xmm2,1
        
        psubusw		xmm0,xmm3
        paddusw		xmm2,xmm2

        movaps		xmm3,xmm0			
        psubusw		xmm2,xmm5

        //------------------- Damping ----------------------------------------
        movaps		xmm4,xmm2			
        
        psrlw		xmm3,xmm6 // Divide Res by damping
        psrlw		xmm4,xmm6 // Divide Res by damping

        psubusw		xmm0,xmm3 // Res= Res - res*Damping
        psubusw		xmm2,xmm4 // Res= Res - res*Damping

        packuswb	xmm0,xmm2 // Convertit les word en bytes avec clamping (0..255)
        movaps		[edi],xmm0 // Store in destination

        add edi,16

        sub eax,4
        jz SuiteW
        jmp BoucleW

SuiteW:
        movaps		xmm0,[esi] // Load upper line  ( T0|T1|T2|T3 )
        movaps		xmm2,[esi+ecx*2] // Load Lower line  ( B0|B1|B2|B3 )
        movaps		xmm1,[esi+ecx]		
        pavgb       xmm0,xmm2
        movaps		xmm3,xmm1 // backup same line 
        movaps		xmm4,xmm0
        pslldq		xmm1,4 // xmm1 =  (  ??|N0|N1|N2 )
        movss		xmm5,[esi+ecx-4] // xmm5 =  (  N-1|0|0|0 )
        punpcklbw	xmm0,xmm7					
        movss		xmm2,DWORD ptr BorderColor			
        punpckhbw	xmm4,xmm7					
        movss		xmm1,xmm5
        movss		xmm3,xmm2
        pshufd		xmm3,xmm3,0x39
        movaps		xmm5,[edi] // Load destination ( D0|D1|D2|D3 )
        pavgb		xmm1,xmm3
        movaps		xmm3,xmm5 // Store dest
        movaps		xmm2,xmm1
        add esi,16
        punpcklbw	xmm1,xmm7					
        prefetcht1	[esi+16]	
        prefetcht1	[esi+ecx+16]
        prefetcht1	[esi+ecx*2+16]
        punpckhbw	xmm2,xmm7					
        paddusw		xmm0,xmm1		
        punpcklbw	xmm3,xmm7					
        paddusw		xmm2,xmm4		
        psrlw		xmm0,1
        punpckhbw	xmm5,xmm7					
        paddusw		xmm0,xmm0		
        psrlw		xmm2,1
        psubusw		xmm0,xmm3
        paddusw		xmm2,xmm2
        movaps		xmm3,xmm0			
        psubusw		xmm2,xmm5
        //------------------- Damping ----------------------------------------
        movaps		xmm4,xmm2			
        psrlw		xmm3,xmm6 // Divide Res by damping
        psrlw		xmm4,xmm6 // Divide Res by damping
        psubusw		xmm0,xmm3 // Res= Res - res*Damping
        psubusw		xmm2,xmm4 // Res= Res - res*Damping
        packuswb	xmm0,xmm2 // Convertit les word en bytes avec clamping (0..255)
        movaps		[edi],xmm0 // Store in destination

        add edi,16

        dec edx
        jz finTot
        jmp BoucleH
finTot:
        //		sub esi,ecx
        //--------- Last line pixels don't have a bottom neighbour pixel
        // Bottom Left Pixel
        movd		MM1,DWORD PTR [esi]	
        movd		MM3,DWORD PTR [esi+ecx+4]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1	 // Store in destination
        add esi,4
        add edi,4

        mov eax,width
        sub eax,2

        // Top Pixels
LastLine:
        movd		MM1,DWORD PTR [esi+ecx-4]			
        movd		MM2,DWORD PTR [esi+ecx+4]	
        punpcklbw	MM1,MM7						
        movd		MM3,DWORD PTR [esi]	

        punpcklbw	MM2,MM7			
        punpcklbw	MM3,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw     MM1,MM2
        paddusw     MM1,MM3
        paddusw     MM1,MM5
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        dec eax
        jnz LastLine

        // Top Right Pixel
        movd		MM1,DWORD PTR [esi+ecx-4]	
        movd		MM3,DWORD PTR [esi]	
        punpcklbw	MM1,MM7						
        punpcklbw	MM3,MM7			
        paddusw     MM1,MM3
        paddusw     MM1,MM4
        movd		MM0,DWORD PTR [edi] // MM0 = Load destination pixels
        psrlw		MM1,2 // Divise par 4 ( moyenne )
        punpcklbw	MM0,MM7 // MM0=00|R0|00|G0|00|B0|00|A0	Transfo Byte->Word (H)	
        paddusw		MM1,MM1
        psubusw		MM1,MM0
        movq		MM2,MM1			
        psrlw		MM2,MM6 // Divide Res by damping
        psubusw		MM1,MM2 // Res= Res - res*Damping
        packuswb	MM1,MM1 // Convertit les word en bytes avec clamping (0..255)
        movd		DWORD PTR [edi],MM1 // Store in destination
        add esi,4
        add edi,4

        EMMS
        popad
    }
}
#endif

#if defined(WIN32) || (defined(macintosh) && defined(__i386__))

/*****************************************************************************************************************
/*																												 */
/*  																												 */
/*																												 */
/*****************************************************************************************************************/
void BlendDataMMX(void *ResData, void *Data1, void *Data2, int NbDword, float Factor)
{
    CKDWORD BFactor2 = (CKDWORD)(Factor * 256.0f);
    CKDWORD BFactor1 = (CKDWORD)((1.0f - Factor) * 256.0f);

    BFactor2 = BFactor2 | (BFactor2 << 16);
    BFactor1 = BFactor1 | (BFactor1 << 16);

    __asm {
        mov esi,Data1
        mov ebx,Data2
        mov edi,ResData
        mov ecx,NbDword

        movd MM7,BFactor2
        movd MM6,BFactor1
        
        movq MM5,MM7
        movq MM4,MM6

        psllq MM5,32
        psllq MM4,32

        por   MM7,MM5
        por   MM6,MM4

        pxor  MM5,MM5

BlendLoop:
        movd MM0,[esi] // MM0=00|00|00|00|AA|RR|GG|BB (tex 1)
        movd MM1,[ebx] // MM0=00|00|00|00|AA|RR|GG|BB (tex 2)			

        punpcklbw	MM0,MM5 // MM0=00|A1|00|R1|00|G1|00|B1		
        punpcklbw	MM1,MM5 // MM1=00|A2|00|R2|00|G2|00|B2			
    
        pmullw MM0,MM6 // MM0=A1*F1|R1*F1|G1*F1|B1*F1		
        pmullw MM1,MM7 // MM1=A2*F2|R2*F2|G2*F2|B2*F2

        paddusw MM0,MM1 // MM0=00|A1|00|R1|00|G1|00|B1		
        psrlw	MM0,8

        packuswb MM0,MM0 // WORD -> Bytes
        movd [edi],MM0

        add esi,4
        add edi,4
        add ebx,4

        dec ecx
        jnz BlendLoop

        EMMS
    }
}

/*		movaps	xmm0,[esi]						// Load upper line  ( T0|T1|T2|T3 )
        movaps	xmm2,[esi+ecx*2]				// Load Lower line  ( B0|B1|B2|B3 )
        movaps	xmm1,[esi+ecx]					// Load Same line   ( N0|N1|N2|N3 )
        pavgb	xmm0,xmm2						// Average : xmm0 = (T0+B0)/2|(T1+B1)/2|(T2+B2)/2|(T3+B3)/2

        movaps  xmm3,xmm1						// backup same line
        pslldq  xmm1,4							// xmm1 =  (  ??|N0|N1|N2 )

        movss   xmm2,[esi+ecx+16]				// xmm3 =  (  N4|N1|N2|N3 )
        movss   xmm5,[esi+ecx-4]				// xmm1 =  (  N-1|N0|N1|N2 )
        movss   xmm3,xmm2
        movss   xmm1,xmm5
        pshufd  xmm3,xmm3,0x39					// rotate xmm3 =  (  N1|N2|N3|N4 )

        pavgb	xmm0,xmm1						// Average

        movaps  xmm4,[edi]						// Load destination ( D0|D1|D2|D3 )
        pavgb	xmm0,xmm3						// Average

        movaps		xmm5,xmm4					// Store dest
        movaps		xmm2,xmm0					// Store vois

        punpcklbw	xmm0,xmm7					// xmm0=00|B0|00|G0|00|R0|00|A0|00|B1|00|G1|00|R1|00|A1	Transfo Byte->Word (H)
        punpckhbw	xmm2,xmm7					// xmm0=00|B2|00|G2|00|R2|00|A2|00|B3|00|G3|00|R3|00|A3	Transfo Byte->Word (H)

        punpcklbw	xmm4,xmm7					// xmm4=00|B0|00|G0|00|R0|00|A0|00|B1|00|G1|00|R1|00|A1	Transfo Byte->Word (H)
        punpckhbw	xmm5,xmm7					// xmm5=00|B2|00|G2|00|R2|00|A2|00|B3|00|G3|00|R3|00|A3	Transfo Byte->Word (H)
*/

/*
        movaps		xmm4,xmm0
        punpcklbw	xmm0,xmm7

        movaps		xmm5,xmm2
        punpcklbw	xmm2,xmm7

        movaps		xmm1,[esi+ecx]
        punpckhbw	xmm4,xmm7

        paddusw		xmm0,xmm2					// Average Top-Bottom 1


        punpckhbw	xmm5,xmm7

        movaps		xmm3,xmm1					// backup same line
        paddusw		xmm4,xmm5					// Average Top-Bottom 2		xmm0 & xmm4 = average T&B

        pslldq		xmm1,4						// xmm1 =  (  ??|N0|N1|N2 )
        movss		xmm5,[esi+ecx-4]			// xmm5 =  (  N-1|0|0|0 )

        movss		xmm2,[esi+ecx+16]
        movss		xmm1,xmm5

        movss		xmm3,xmm2
        movaps		xmm2,xmm1

        pshufd		xmm3,xmm3,0x39
        punpcklbw	xmm1,xmm7

        movaps		xmm5,xmm3
        punpckhbw	xmm2,xmm7
        prefetcht1	[esi+16]

        punpcklbw	xmm3,xmm7
        paddusw		xmm2,xmm4
        prefetcht1	[esi+ecx+16]

        punpckhbw	xmm5,xmm7
        paddusw		xmm1,xmm3
        prefetcht1	[esi+2*ecx+16]

        paddusw		xmm2,xmm5
        movaps		xmm3,[edi]					// Load destination ( D0|D1|D2|D3 )

        paddusw		xmm0,xmm1
        movaps		xmm5,xmm3					// Store dest

        psrlw		xmm0,2
        punpcklbw	xmm3,xmm7					// xmm4=00|B0|00|G0|00|R0|00|A0|00|B1|00|G1|00|R1|00|A1	Transfo Byte->Word (H)

        paddusw		xmm0,xmm0
        psrlw		xmm2,2

        punpckhbw	xmm5,xmm7					// xmm5=00|B2|00|G2|00|R2|00|A2|00|B3|00|G3|00|R3|00|A3	Transfo Byte->Word (H)
        paddusw		xmm2,xmm2

        psubusw		xmm0,xmm3
        psubusw		xmm2,xmm5
//------------------- Damping ----------------------------------------
        movaps		xmm3,xmm0

        movaps		xmm4,xmm2
        psrlw		xmm3,xmm6					// Divide Res by damping

        psrlw		xmm4,xmm6					// Divide Res by damping
        psubusw		xmm0,xmm3				 	// Res= Res - res*Damping
        psubusw		xmm2,xmm4				 	// Res= Res - res*Damping

        add esi,16
        packuswb	xmm0,xmm2					// Convertit les word en bytes avec clamping (0..255)
        movaps		[edi],xmm0					// Store in destination

        add edi,16

        sub eax,4
        jz SuiteW
        jmp BoucleW

*/
#endif

#if (defined(macintosh) && defined(__ppc__))

#if G4
#include <altivec.h>
void BlendDataAltivecAligned(unsigned long *ResData, unsigned long *Data0, unsigned long *Data1, int NbDword, float Factor);
void BlendDataAltivecUnAligned(unsigned long *ResData, unsigned long *Data0, unsigned long *Data1, int NbDword, float Factor);

void BlendDataAltivec(unsigned long *ResData, unsigned long *Data0, unsigned long *Data1, int NbDword, float Factor)
{
    if ((((long)ResData) & 0x0f != 0) ||
        (((long)ResData) & 0x0f != 0) ||
        (((long)ResData) & 0x0f != 0) ||
        NbDword % 4)
        BlendDataAltivecUnAligned(ResData, Data0, Data1, NbDword, Factor);

    // Aligned Data
    BlendDataAltivecAligned(ResData, Data0, Data1, NbDword, Factor);
}

void BlendDataAltivecAligned(unsigned long *ResData, unsigned long *Data0, unsigned long *Data1, int NbDword, float Factor)
{
#ifdef __GNUC__
#undef long
#define long int
#endif
    unsigned long mul0 = (unsigned long)(Factor * 256.0f);
    unsigned long mul1 = (unsigned long)((1.0f - Factor) * 256.0f);

    vector unsigned long c0;
    vector unsigned long c1;

    vector unsigned long m0, m1;
    vector unsigned long R;
    vector unsigned char mask, zero;

    vector unsigned short c00, c01, c02, c03;
    vector unsigned short c10, c11, c12, c13;

    zero = (vector unsigned char)vec_splat_u32(0);

    vector unsigned char unpack0 = (vector unsigned char)(0, 0, 0, 0x10, 0, 0, 0, 0x11, 0, 0, 0, 0x12, 0, 0, 0, 0x13);
    vector unsigned char unpack1 = (vector unsigned char)(0, 0, 0, 0x14, 0, 0, 0, 0x15, 0, 0, 0, 0x16, 0, 0, 0, 0x17);
    vector unsigned char unpack2 = (vector unsigned char)(0, 0, 0, 0x18, 0, 0, 0, 0x19, 0, 0, 0, 0x1A, 0, 0, 0, 0x1B);
    vector unsigned char unpack3 = (vector unsigned char)(0, 0, 0, 0x1C, 0, 0, 0, 0x1D, 0, 0, 0, 0x1E, 0, 0, 0, 0x1F);

    vector unsigned long roll = (vector unsigned long)(0x08, 0x08, 0x08, 0x08);

    vector unsigned char repack0 = (vector unsigned char)(0x03, 0x07, 0x0b, 0x0f, 0x13, 0x17, 0x1b, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    vector unsigned char repack1 = (vector unsigned char)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x13, 0x17, 0x1b, 0x1f, 0x00, 0x00, 0x00, 0x00);
    vector unsigned char repack2 = (vector unsigned char)(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x13, 0x17, 0x1b, 0x1f);

    // Load the Multiplicators

    m0 = vec_lde(0, &mul0);		 // m0 : mul0
    mask = vec_lvsl(0, &mul0);	 // create the permute mask
    m0 = vec_perm(m0, m0, mask); // align
    m0 = vec_splat(m0, 0);		 // m0 : mul0|mul0|mul0|mul0

    m1 = vec_lde(0, &mul1);		 // m1 : mul1
    mask = vec_lvsl(0, &mul1);	 // create the permute mask
    m1 = vec_perm(m1, m1, mask); // align
    m1 = vec_splat(m1, 0);		 // m1 : mul1|mul1|mul1|mul1

    int count = NbDword / 4;
    while (count)
    {

        c0 = vec_ld(0, Data0); // c0 : ARGB|ARGB|ARGB|ARGB
        c1 = vec_ld(0, Data1); // c1 : ARGB|ARGB|ARGB|ARGB

        c00 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c0, unpack0);				  // c00 : 000A|000R|000G|000B perm
        c00 = (vector unsigned short)vec_mulo((vector unsigned short)c00, (vector unsigned short)m0); // c00 : 00AA|00RR|00GG|00BB mulitplied by factor

        c10 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c1, unpack0);				  // c01 : 000A|000R|000G|000B perm
        c10 = (vector unsigned short)vec_mulo((vector unsigned short)c10, (vector unsigned short)m1); // c01 : 00AA|00RR|00GG|00BB mulitplied by factor

        c00 = (vector unsigned short)vec_add((vector unsigned long)c00, (vector unsigned long)c10); // c00 : 0AAA|0RRR|0GGG|0BBB add
        c00 = (vector unsigned short)vec_sr((vector unsigned long)c00, roll);						// c00 : 000A|000R|000G|000B back to 0 - 255

        c01 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c0, unpack1);				  // c10 : 000A|000R|000G|000B perm
        c01 = (vector unsigned short)vec_mulo((vector unsigned short)c01, (vector unsigned short)m0); // c10 : 00AA|00RR|00GG|00BB mulitplied by factor

        c11 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c1, unpack1);				  // c11 : 000A|000R|000G|000B perm
        c11 = (vector unsigned short)vec_mulo((vector unsigned short)c11, (vector unsigned short)m1); // c11 : 00AA|00RR|00GG|00BB mulitplied by factor

        c01 = (vector unsigned short)vec_add((vector unsigned short)c01, (vector unsigned short)c11); // c10 : 0AAA|0RRR|0GGG|0BBB add
        c01 = (vector unsigned short)vec_sr((vector unsigned long)c01, roll);						  // c10 : 000A|000R|000G|000B back to 0 - 255

        c02 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c0, unpack2);				  // c20 : 000A|000R|000G|000B perm
        c02 = (vector unsigned short)vec_mulo((vector unsigned short)c02, (vector unsigned short)m0); // c20 : 00AA|00RR|00GG|00BB mulitplied by factor

        c12 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c1, unpack2);				  // c21 : 000A|000R|000G|000B perm
        c12 = (vector unsigned short)vec_mulo((vector unsigned short)c12, (vector unsigned short)m1); // c21 : 00AA|00RR|00GG|00BB mulitplied by factor

        c02 = (vector unsigned short)vec_add((vector unsigned short)c02, (vector unsigned short)c12); // c20 : 0AAA|0RRR|0GGG|0BBB add
        c02 = (vector unsigned short)vec_sr((vector unsigned long)c02, roll);						  // c20 : 000A|000R|000G|000B back to 0 - 255

        c03 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c0, unpack3);				  // c30 : 000A|000R|000G|000B perm
        c03 = (vector unsigned short)vec_mulo((vector unsigned short)c03, (vector unsigned short)m0); // c30 : 00AA|00RR|00GG|00BB mulitplied by factor

        c13 = (vector unsigned short)vec_perm(zero, (vector unsigned char)c1, unpack3);				  // c31 : 000A|000R|000G|000B perm
        c13 = (vector unsigned short)vec_mulo((vector unsigned short)c13, (vector unsigned short)m1); // c31 : 00AA|00RR|00GG|00BB mulitplied by factor

        c03 = (vector unsigned short)vec_add((vector unsigned short)c03, (vector unsigned short)c13); // c30 : 0AAA|0RRR|0GGG|0BBB add
        c03 = (vector unsigned short)vec_sr((vector unsigned long)c03, roll);						  // c30 : 000A|000R|000G|000B back to 0 - 255

        // Distribute
        R = (vector unsigned long)vec_perm((vector unsigned char)c00, (vector unsigned char)c01, repack0); // R : ARGB|ARGB|0000|0000
        R = (vector unsigned long)vec_perm((vector unsigned char)R, (vector unsigned char)c02, repack1);   // R : ARGB|ARGB|ARGB|0000
        R = (vector unsigned long)vec_perm((vector unsigned char)R, (vector unsigned char)c03, repack2);   // R : ARGB|ARGB|ARGB|ARGB

        vec_st(R, 0, (unsigned int *)ResData); // Store it to destination
        count--;
        Data0 += 4;
        Data1 += 4;
        ResData += 4;
    }
#ifdef __GNUC__
#undef long
#define long long
#endif
}

void BlendDataAltivecUnAligned(unsigned long *ResData, unsigned long *Data0, unsigned long *Data1, int NbDword, float Factor)
{
#ifdef __GNUC__
#undef long
#define long int
#endif
    unsigned long mul0 = (unsigned long)(Factor * 256.0f);
    unsigned long mul1 = (unsigned long)((1.0f - Factor) * 256.0f);

    vector unsigned long c0;
    vector unsigned long c1;

    vector unsigned long m0, m1;
    vector unsigned long R, t0, t1;
    vector unsigned char mask, zero;

    zero = (vector unsigned char)vec_splat_u32(0);

    vector unsigned char unpack = (vector unsigned char)(0, 0, 0, 0x10, 0, 0, 0, 0x11, 0, 0, 0, 0x12, 0, 0, 0, 0x13);
    vector unsigned long roll = (vector unsigned long)(0x08, 0x08, 0x08, 0x08);
    vector unsigned char repack = (vector unsigned char)(0x13, 0x17, 0x1b, 0x1f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Load the Multiplicators

    m0 = vec_lde(0, &mul0);		 // m0 : mul0
    mask = vec_lvsl(0, &mul0);	 // create the permute mask
    m0 = vec_perm(m0, m0, mask); // align
    m0 = vec_splat(m0, 0);		 // m0 : mul0|mul0|mul0|mul0

    m1 = vec_lde(0, &mul1);		 // m1 : mul1
    mask = vec_lvsl(0, &mul1);	 // create the permute mask
    m1 = vec_perm(m1, m1, mask); // align
    m1 = vec_splat(m1, 0);		 // m1 : mul1|mul1|mul1|mul1

    int count = NbDword;

    while (count)
    {
        t0 = vec_lde(0, Data0);
        mask = vec_lvsl(0, Data0);
        c0 = vec_perm(t0, t0, mask);												 // 1 colors in c0 ARGB|0000|0000|00000
        c0 = (vector unsigned long)vec_perm(zero, (vector unsigned char)c0, unpack); // c0 : 000A|000R|000G|000B

        t0 = vec_mulo((vector unsigned short)c0, (vector unsigned short)m0);

        t1 = vec_lde(0, Data1);
        mask = vec_lvsl(0, Data1);
        c1 = vec_perm(t1, t1, mask);												 // 1 colors in c1 ARGB|0000|0000|00000
        c1 = (vector unsigned long)vec_perm(zero, (vector unsigned char)c1, unpack); // c1 : 000A|000R|000G|000B

        t1 = vec_mulo((vector unsigned short)c1, (vector unsigned short)m1);

        R = vec_add(t0, t1); //
        R = vec_sr(R, roll); // back 0-255

        R = (vector unsigned long)vec_perm(zero, (vector unsigned char)R, repack); // R : ARGB|0000|0000|0000

        R = vec_splat(R, 0);
        vec_ste(R, 0, (unsigned int *)ResData);

        count--;
        Data0++;
        Data1++;
        ResData++;
    }
#ifdef __GNUC__
#undef long
#define long long
#endif
}
#else

void BlendDataC(DWORD *ResData, DWORD *Data1, DWORD *Data2, int NbDword, float Factor)
{
    float BFactor1 = (Factor);			//*256.0f);
    float BFactor2 = ((1.0f - Factor)); //*256.0f);
    while (NbDword-- > 0)
    {
        DWORD f1 = *Data1++;
        DWORD f2 = *Data2++;
        DWORD ColAG = ((f1 & 0xFF00FF00) >> 2) * BFactor2 + ((f2 & 0xFF00FF00) >> 2) * BFactor1;
        DWORD ColRB = ((f1 & 0x00FF00FF)) * BFactor2 + ((f2 & 0x00FF00FF)) * BFactor1;
        ColAG = (ColAG << 2) & 0xFF00FF00;
        ColRB = (ColRB)&0x00FF00FF;
        *ResData++ = ColAG | ColRB;
    }
}
#endif

#endif