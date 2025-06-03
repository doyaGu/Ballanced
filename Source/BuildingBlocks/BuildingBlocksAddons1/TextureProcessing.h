#ifndef TEXTUREPROCESSING_H
#define TEXTUREPROCESSING_H

//////////////////////////////////////
// Texture Processing functions

// Convolutions Matrix
// Gaussian Blur Matrix
extern int CMAntialias[3][3];
// Gaussian Blur Matrix
extern int CMGaussianBlur[3][3];
// Gaussian Blur Matrix
extern int CMUniformBlur[3][3];
// Gaussian Blur Matrix
extern int CMDirectionnal[3][3];
// Emboss Matrix
extern int CMEmboss[3][3];
// Passe haut Matrix
extern int CMHighPass[3][3];
// Passe bas Matrix
extern int CMLowPass[3][3];

// Decal  Matrices
extern int CMDecalLeft[3][3];
extern int CMDecalRight[3][3];
extern int CMDecalTop[3][3];
extern int CMDecalBottom[3][3];
extern int CMDecalNone[3][3];
extern int CMDecalBR[3][3];
extern int CMDecalBL[3][3];
extern int CMDecalTL[3][3];
extern int CMDecalTR[3][3];

void ApplyMatrixToTexture(CKTexture *tex, int matrix[3][3], int sum);

void ProcessPixelsTemporalMMX(void *SrcData, void *DstData, int width, int height, void *MatrixData, int Damping);
void ExchangeMemoryMMX(void *Data1, void *Data2, CKDWORD sizet);
void ProcessPixelsMMX(void *SrcData, void *DstData, int width, int height, void *MatrixData);
void WaterEffectMMX(void *CurrentData, void *PreviousData, int width, int height, CKDWORD BorderColor, int Damping);
void WaterEffectWillamette(void *CurrentData, void *PreviousData, int width, int height, CKDWORD BorderColor, int Damping);
void BlendDataMMX(void *ResData, void *Data1, void *Data2, int NbDword, float Factor);

#if defined(macintosh)
#if G4
void BlendDataAltivec(unsigned long *ResData, unsigned long *Data1, unsigned long *Data2, int NbDword, float Factor);
#else
void BlendDataC(unsigned long *ResData, unsigned long *Data1, unsigned long *Data2, int NbDword, float Factor);
#endif
#endif
#endif
