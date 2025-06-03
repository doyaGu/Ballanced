#include "CKAll.h"

#include "DrawingPrimitives.h"

void DrawFillRectangle(CKRenderContext *dev, CKDWORD col, CKMaterial *mat, const VxRect &rect, const VxRect &uvrect, int mode)
{
    if (mat)
    {
        mat->SetAsCurrent(dev);
    }
    else
    {
        dev->SetTexture(NULL);
        dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
        if (ColorGetAlpha(col) == 255)
        {
            dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
            dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
            dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ZERO);
        }
        else
        {
            dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
            dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
            dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
        }
    }

    VxDrawPrimitiveData *data;
    if (mode & (TRANSFORM | LIGHTING))
        data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_VNT, 4);
    else if (mode & TRANSFORM)
        data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_VCT, 4);
    else if (mode & LIGHTING)
        data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_VNT, 4);
    else
        data = dev->GetDrawPrimitiveStructure(CKRST_DP_CL_VCT, 4);

    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
    VxVector *normals = (VxVector *)data->NormalPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    if (mode & LIGHTING)
    {
        /////////////////
        // Normals

        // Normal 0
        normals->x = 0;
        normals->y = 0;
        normals->z = 1.0f;
        normals = (VxVector *)((CKBYTE *)normals + data->NormalStride);
        // Normal 1
        normals->x = 0;
        normals->y = 0;
        normals->z = 1.0f;
        normals = (VxVector *)((CKBYTE *)normals + data->NormalStride);
        // Normal 2
        normals->x = 0;
        normals->y = 0;
        normals->z = 1.0f;
        normals = (VxVector *)((CKBYTE *)normals + data->NormalStride);
        // Normal 3
        normals->x = 0;
        normals->y = 0;
        normals->z = 1.0f;
        normals = (VxVector *)((CKBYTE *)normals + data->NormalStride);
    }
    else
    {
        /////////////////
        // Colors

        VxFillStructure(4, colors, data->ColorStride, 4, &col);
    }

    /////////////////
    // UVs

    // Uvs
    uvs[0].u = uvrect.left;
    uvs[0].v = uvrect.top;
    uvs[1].u = uvrect.right;
    uvs[1].v = uvrect.top;
    uvs[2].u = uvrect.right;
    uvs[2].v = uvrect.bottom;
    uvs[3].u = uvrect.left;
    uvs[3].v = uvrect.bottom;

    /////////////////
    // Positions

    // Vertex 0
    positions->x = rect.left;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 1
    positions->x = rect.right;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 2
    positions->x = rect.right;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 3
    positions->x = rect.left;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);

    // the drawing itself
    dev->DrawPrimitive(VX_TRIANGLEFAN, NULL, 4, data);
}

void DrawBorderRectangle(CKRenderContext *dev, CKDWORD col, CKMaterial *mat, const VxRect &screen, const float bsize)
{
    VxRect rect = screen;

    if (mat)
        mat->SetAsCurrent(dev);
    else
    {
        dev->SetTexture(NULL);
        dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
        if (ColorGetAlpha(col) == 255)
        {
            dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
            dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
            dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ZERO);
        }
        else
        {
            dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
            dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
            dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
        }
    }

    VxDrawPrimitiveData *data;
    data = dev->GetDrawPrimitiveStructure(CKRST_DP_CL_VC, 8);

    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    /////////////////
    // Colors

    VxFillStructure(8, colors, data->ColorStride, 4, &col);

    /////////////////
    // Positions

    // Vertex 0
    positions->x = rect.left;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 1
    positions->x = rect.right;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 2
    positions->x = rect.right;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 3
    positions->x = rect.left;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);

    rect.Inflate(Vx2DVector(-bsize, -bsize));

    // Vertex 4
    positions->x = rect.left;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 5
    positions->x = rect.right;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 6
    positions->x = rect.right;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
    // Vertex 7
    positions->x = rect.left;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);

    // Indices
    CKWORD *indices = dev->GetDrawPrimitiveIndices(24);
    indices[0] = 0;
    indices[1] = 5;
    indices[2] = 4;
    indices[3] = 5;
    indices[4] = 0;
    indices[5] = 1;
    indices[6] = 5;
    indices[7] = 1;
    indices[8] = 2;
    indices[9] = 5;
    indices[10] = 2;
    indices[11] = 6;
    indices[12] = 6;
    indices[13] = 2;
    indices[14] = 3;
    indices[15] = 6;
    indices[16] = 3;
    indices[17] = 7;
    indices[18] = 7;
    indices[19] = 3;
    indices[20] = 4;
    indices[21] = 4;
    indices[22] = 3;
    indices[23] = 0;

    // the drawing itself
    dev->DrawPrimitive(VX_TRIANGLELIST, indices, 24, data);
}
