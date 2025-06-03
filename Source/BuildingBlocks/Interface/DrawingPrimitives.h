#ifndef CK_DRAWINGPRIMITIVES_H
#define CK_DRAWINGPRIMITIVES_H

#define TRANSFORM 1
#define LIGHTING  2

// Rectangle Drawing
void DrawFillRectangle(CKRenderContext *dev, CKDWORD col, CKMaterial *mat, const VxRect &rect, const VxRect &uvrect, int mode = 0);
void DrawBorderRectangle(CKRenderContext *dev, CKDWORD col, CKMaterial *mat, const VxRect &rect, const float bsize);

#endif