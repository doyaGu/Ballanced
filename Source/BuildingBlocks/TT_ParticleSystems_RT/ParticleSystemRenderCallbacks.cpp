#include "CKAll.h"

#include "ParticleEmitter.h"
#include "ParticleSystemRenderCallbacks.h"

int (*renderParticles)(CKRenderContext *dev, CKRenderObject *obj, void *arg);

int RenderParticles_P(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    if (!em->particleCount)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, em->particleCount);

    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    dev->SetWorldTransformationMatrix(oldmatrix * mov->GetInverseWorldMatrix());

    // we don't let write to the ZBuffer
    dev->SetTexture(NULL);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, em->m_SrcBlend);
    dev->SetState(VXRENDERSTATE_DESTBLEND, em->m_DestBlend);
    if (em->m_DestBlend != VXBLEND_ZERO)
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    else
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
    dev->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);

    Particle *p = em->GetParticles();

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    while (p)
    {
        *positions = p->pos;
        *colors = RGBAFTOCOLOR(&(p->m_Color));

        // next point
        p = p->next;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
    }

    // The drawing
    dev->DrawPrimitive(VX_POINTLIST, NULL, em->particleCount, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 0;
}

int RenderParticles_L(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    if (!em->particleCount)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, 2 * em->particleCount);

    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    dev->SetWorldTransformationMatrix(oldmatrix * mov->GetInverseWorldMatrix());

    // we don't let write to the ZBuffer
    dev->SetTexture(NULL);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, em->m_SrcBlend);
    dev->SetState(VXRENDERSTATE_DESTBLEND, em->m_DestBlend);
    if (em->m_DestBlend != VXBLEND_ZERO)
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    else
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
    dev->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);

    Particle *p = em->GetParticles();
    VxColor oldColor;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    while (p)
    {
        // Colors
        oldColor.r = p->m_Color.r - p->deltaColor.r * p->m_DeltaTime;
        oldColor.g = p->m_Color.g - p->deltaColor.g * p->m_DeltaTime;
        oldColor.b = p->m_Color.b - p->deltaColor.b * p->m_DeltaTime;
        oldColor.a = p->m_Color.a - p->deltaColor.a * p->m_DeltaTime;

        // Start of line
        *positions = p->pos - p->dir * p->m_DeltaTime;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif

        *colors = RGBAFTOCOLOR(&oldColor);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif

        // End of line
        *positions = p->pos + p->dir * p->m_Angle;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif

        *colors = RGBAFTOCOLOR(&(p->m_Color));
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif

        // Next particle
        p = p->next;
    }

    // the Drawing itself
    dev->DrawPrimitive(VX_LINELIST, NULL, em->particleCount, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 0;
}

int RenderParticles_S(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    const int VBUFFERSIZE = 4000;

    int pc = em->particleCount;
    if (!pc)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT | CKRST_DP_VBUFFER), (4 * pc > VBUFFERSIZE) ? VBUFFERSIZE : 4 * pc);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    ///
    // Indices
    if (pc * 6 > ParticleEmitter::m_GlobalIndicesCount)
    {
        delete[] ParticleEmitter::m_GlobalIndices;

        ParticleEmitter::m_GlobalIndicesCount = pc * 6;
        ParticleEmitter::m_GlobalIndices = new CKWORD[ParticleEmitter::m_GlobalIndicesCount];

        int ni = 0;
        int fi = 0;
        for (int i = 0; i < pc; ++i)
        {
            ParticleEmitter::m_GlobalIndices[fi] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 1] = ni + 1;
            ParticleEmitter::m_GlobalIndices[fi + 2] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 3] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 4] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 5] = ni + 3;
            fi += 6;
            ni += 4;
        }
    }

    // Render States
    em->SetState(dev);

    CK3dEntity *cam = dev->GetViewpoint();
    const VxMatrix &mat = cam->GetWorldMatrix();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    VxMatrix realmatrix = oldmatrix * mov->GetInverseWorldMatrix();
    dev->SetWorldTransformationMatrix(realmatrix);
    VxVector camright;
    Vx3DRotateVector(&camright, realmatrix, &mat[0]);
    VxVector camup;
    Vx3DRotateVector(&camup, realmatrix, &mat[1]);

    CKBOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)(sqrtf((float)(em->m_TextureFrameCount - 1)));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector cr, cu;
    VxVector rcr, rcu;
    VxVector pos;

    float u = 0.0f;
    float v = 0.0f;

    int np = 0;
    int remaining = 4 * pc;

    while (p)
    {
        pos = p->pos;

        // Precalcul for positions
        float halfsize = p->m_Size * 0.5f;
        cr = camright * halfsize;
        cu = camup * halfsize;

        // Angle rotation, if necessary
        if (p->m_Angle)
        {
            float cosa = cosf(p->m_Angle);
            float sina = sinf(p->m_Angle);
            rcr = cr * cosa - cu * sina;
            rcu = cr * sina + cu * cosa;
            cr = rcr;
            cu = rcu;
        }

        // Precalcul for color
        CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));

        // Precalcul for uvs
        if (changeuv)
        {
            v = 0.0f;
            int c = p->m_CurrentFrame;

            while (c >= framehc)
            {
                v += step;
                c -= framehc;
            }
            u = c * step;
        }

        // Filling vertices
        *positions = pos - cr + cu;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif
        *positions = pos + cr + cu;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = pos + cr - cu;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = pos - cr - cu;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        ++np;
        p = p->next;

        if (np * 4 == VBUFFERSIZE)
        { // need to flush the vertex buffer
            // The Primitive Drawing
            dev->DrawPrimitive(VX_TRIANGLELIST, ParticleEmitter::m_GlobalIndices, 6 * np, data);

            remaining -= VBUFFERSIZE;
            np = 0; // restart

            if (remaining > 0)
            {
                data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT | CKRST_DP_VBUFFER), (remaining > VBUFFERSIZE) ? VBUFFERSIZE : remaining);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                uvs = (VxUV *)data->TexCoordPtr;
                positions = (VxVector *)data->PositionPtr;
                colors = (CKDWORD *)data->ColorPtr;
#else
                uvs = (VxUV *)data->TexCoord.Ptr;
                positions = (VxVector *)data->Positions.Ptr;
                colors = (CKDWORD *)data->Colors.Ptr;
#endif
            }
        }
    }

    // The Primitive Drawing
    if (remaining)
        dev->DrawPrimitive(VX_TRIANGLELIST, ParticleEmitter::m_GlobalIndices, 6 * np, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}

int RenderParticles_FS(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 3 * pc);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    XPtrStrided<VxUV> uvs(data->TexCoordPtr, data->TexCoordStride);
    XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
    XPtrStrided<CKDWORD> colors(data->ColorPtr, data->ColorStride);
#else
    XPtrStrided<VxUV> uvs(data->TexCoord.Ptr, data->TexCoord.Stride);
    XPtrStrided<VxVector> positions(data->Positions.Ptr, data->Positions.Stride);
    XPtrStrided<CKDWORD> colors(data->Colors.Ptr, data->Colors.Stride);
#endif

    // Render States
    em->SetState(dev);

    CK3dEntity *cam = dev->GetViewpoint();
    const VxMatrix &mat = cam->GetWorldMatrix();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    VxMatrix realmatrix = oldmatrix * mov->GetInverseWorldMatrix();
    dev->SetWorldTransformationMatrix(realmatrix);
    VxVector camright;
    Vx3DRotateVector(&camright, realmatrix, &mat[0]);
    VxVector camup;
    Vx3DRotateVector(&camup, realmatrix, &mat[1]);

    CKBOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)(sqrtf((float)(em->m_TextureFrameCount - 1)));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector cr, cu;
    VxVector rcr, rcu;
    VxVector pos;

    float u = 0.0f;
    float v = 0.0f;

    int count = 0;
    while (p)
    {
        cr = camright * p->m_Size * 0.5f;
        cu = camup * p->m_Size * 0.5f;

        // Positions
        if (p->m_Angle)
        {
            float cosa = cosf(p->m_Angle);
            float sina = sinf(p->m_Angle);
            rcr = cr * cosa - cu * sina;
            rcu = cr * sina + cu * cosa;
            cr = rcr;
            cu = rcu;
        }
        pos = p->pos;

        // Colors
        CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));

        // The texture coordinates
        if (changeuv)
        {
            v = 0.0f;
            int c = p->m_CurrentFrame;

            while (c >= framehc)
            {
                v += step;
                c -= framehc;
            }
            u = c * step;
        }

        *positions = pos + cu;
        ++positions;
        *colors = col;
        ++colors;
        uvs->u = u + step * 0.5f;
        uvs->v = v;
        ++uvs;

        *positions = pos + cr - cu;
        ++positions;
        *colors = col;
        ++colors;
        uvs->u = u + step;
        uvs->v = v + step;
        ++uvs;

        *positions = pos - cr - cu;
        ++positions;
        *colors = col;
        ++colors;
        uvs->u = u;
        uvs->v = v + step;
        ++uvs;

        p = p->next;
        ++count;
    }

    // The Primitive Drawing
    dev->DrawPrimitive(VX_TRIANGLELIST, NULL, 3 * pc, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}

// Oriented Sprite
int RenderParticles_OS(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * pc);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    ///
    // Indices
    if (pc * 6 > ParticleEmitter::m_GlobalIndicesCount)
    {
        delete[] ParticleEmitter::m_GlobalIndices;

        ParticleEmitter::m_GlobalIndicesCount = pc * 6;
        ParticleEmitter::m_GlobalIndices = new CKWORD[ParticleEmitter::m_GlobalIndicesCount];

        int ni = 0;
        int fi = 0;
        for (int i = 0; i < pc; ++i)
        {
            ParticleEmitter::m_GlobalIndices[fi] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 1] = ni + 1;
            ParticleEmitter::m_GlobalIndices[fi + 2] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 3] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 4] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 5] = ni + 3;
            fi += 6;
            ni += 4;
        }
    }

    // Set States
    em->SetState(dev, TRUE);

    CK3dEntity *cam = dev->GetViewpoint();
    const VxMatrix &mat = cam->GetWorldMatrix();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    VxMatrix realmatrix = oldmatrix * mov->GetInverseWorldMatrix();
    dev->SetWorldTransformationMatrix(realmatrix);
    VxVector camright;
    Vx3DRotateVector(&camright, realmatrix, &mat[0]);
    VxVector camup;
    Vx3DRotateVector(&camup, realmatrix, &mat[1]);
    VxVector camdir;
    Vx3DRotateVector(&camdir, realmatrix, &mat[2]);

    CKBOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)sqrtf((float)(em->m_TextureFrameCount - 1));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector pos, dir, old, vv(0, 0, 0), ww(0, 0, 0);
    float oldsize;
    VxColor oldColor;

    float u = 0.0f;
    float v = 0.0f;

    int ni = 0;
    int fi = 0;
    while (p)
    {
        pos = p->pos;
        dir = p->dir;

        // the old pos
        old = pos - dir * p->m_DeltaTime;
        // the old size
        oldsize = p->m_Size - p->m_DeltaSize * p->m_DeltaTime;
        // we exaggerate the current position according to the latency
        pos += dir * p->m_Angle;

        vv = pos - old;
        vv.Normalize();
        ww = Normalize(CrossProduct(camdir, vv));

        // the colors
        oldColor.r = p->m_Color.r - p->deltaColor.r * p->m_DeltaTime;
        oldColor.g = p->m_Color.g - p->deltaColor.g * p->m_DeltaTime;
        oldColor.b = p->m_Color.b - p->deltaColor.b * p->m_DeltaTime;
        oldColor.a = p->m_Color.a - p->deltaColor.a * p->m_DeltaTime;

        CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));
        CKDWORD oldcol = RGBAFTOCOLOR(&oldColor);

        // The texture coordinates
        if (changeuv)
        {
            v = 0.0f;
            int c = p->m_CurrentFrame;

            while (c >= framehc)
            {
                v += step;
                c -= framehc;
            }
            u = c * step;
        }

        // Filling vertices
        *positions = pos + vv * p->m_Size + ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = pos + vv * p->m_Size - ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = old - vv * oldsize - ww * oldsize;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = oldcol;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = old - vv * oldsize + ww * oldsize;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = oldcol;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        p = p->next;
        ni += 4;
    }

    // The Primitive Drawing
    dev->DrawPrimitive(VX_TRIANGLELIST, ParticleEmitter::m_GlobalIndices, 6 * pc, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}

// Comet Sprite
int RenderParticles_CS(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * pc);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    // Indices
    if (pc * 6 > ParticleEmitter::m_GlobalIndicesCount)
    {
        delete[] ParticleEmitter::m_GlobalIndices;

        ParticleEmitter::m_GlobalIndicesCount = pc * 6;
        ParticleEmitter::m_GlobalIndices = new CKWORD[ParticleEmitter::m_GlobalIndicesCount];

        int ni = 0;
        int fi = 0;
        for (int i = 0; i < pc; ++i)
        {
            ParticleEmitter::m_GlobalIndices[fi] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 1] = ni + 1;
            ParticleEmitter::m_GlobalIndices[fi + 2] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 3] = ni;
            ParticleEmitter::m_GlobalIndices[fi + 4] = ni + 2;
            ParticleEmitter::m_GlobalIndices[fi + 5] = ni + 3;
            fi += 6;
            ni += 4;
        }
    }

    // Set States
    em->SetState(dev, TRUE);

    CK3dEntity *cam = dev->GetViewpoint();
    const VxMatrix &mat = cam->GetWorldMatrix();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    VxMatrix realmatrix = oldmatrix * mov->GetInverseWorldMatrix();
    dev->SetWorldTransformationMatrix(realmatrix);
    VxVector camright;
    Vx3DRotateVector(&camright, realmatrix, &mat[0]);
    VxVector camup;
    Vx3DRotateVector(&camup, realmatrix, &mat[1]);
    VxVector camdir;
    Vx3DRotateVector(&camdir, realmatrix, &mat[2]);

    CKBOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)sqrtf((float)(em->m_TextureFrameCount - 1));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector pos, dir, center, old, vv(0, 0, 0), ww(0, 0, 0);
    VxColor oldColor;

    float u = 0.0f;
    float v = 0.0f;

    int ni = 0;
    int fi = 0;
    while (p)
    {
        pos = p->pos;
        dir = p->dir;
        float halfsize = p->m_Size;
        float speed = Magnitude(dir);
        dir /= speed;

        // the old pos
        old = pos - dir * (2.0f * halfsize + speed * p->m_Angle * p->m_DeltaTime);

        vv = pos - old;

        float fu = -halfsize * DotProduct(dir, camright);
        float fv = -halfsize * DotProduct(dir, camup);
        ww = -fv * camright + fu * camup;
        center = pos - halfsize * dir;

        // w = Normalize(CrossProduct(camdir,v));

        // Colors
        oldColor.r = p->m_Color.r - p->deltaColor.r * p->m_DeltaTime;
        oldColor.g = p->m_Color.g - p->deltaColor.g * p->m_DeltaTime;
        oldColor.b = p->m_Color.b - p->deltaColor.b * p->m_DeltaTime;
        oldColor.a = p->m_Color.a - p->deltaColor.a * p->m_DeltaTime;

        CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));
        CKDWORD oldcol = RGBAFTOCOLOR(&oldColor);

        // The texture coordinates
        if (changeuv)
        {
            v = 0.0f;
            int c = p->m_CurrentFrame;

            while (c >= framehc)
            {
                v += step;
                c -= framehc;
            }
            u = c * step;
        }

        // Filling vertices
        *positions = center + ww;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = pos;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = center - ww;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = old;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = oldcol;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        p = p->next;
        ni += 4;
    }

    // The Primitive Drawing
    dev->DrawPrimitive(VX_TRIANGLELIST, ParticleEmitter::m_GlobalIndices, 6 * pc, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}

int RenderParticles_RS(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * pc);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    // Render States
    em->SetState(dev);

    CK3dEntity *cam = dev->GetViewpoint();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    dev->SetWorldTransformationMatrix(cam->GetWorldMatrix());

    CKBOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)sqrtf((float)(em->m_TextureFrameCount - 1));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector pos, old(0, 0, 0), vv(0, 0, 0), ww(0, 0, 0);

    float u = 0.0f;
    float v = 0.0f;

    CK3dEntity *ent = (CK3dEntity *)dev->GetCKContext()->GetObject(em->m_Entity);
    cam->InverseTransform(&old, &vv, ent);
    old += Normalize(old) * 40.0f;

    const VxMatrix &invCam = cam->GetInverseWorldMatrix();

    int fi = 0;
    int ni = 0;
    while (p)
    {
        Vx3DMultiplyMatrixVector(&pos, invCam, &p->pos);

        vv.x = pos.x - old.x;
        vv.y = pos.y - old.y;
        vv.Normalize();
        ww.x = -vv.y;
        ww.y = vv.x;

        // the colors
        CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));

        // The texture coordinates
        if (changeuv)
        {
            v = 0.0f;
            int c = p->m_CurrentFrame;

            while (c >= framehc)
            {
                v += step;
                c -= framehc;
            }
            u = c * step;
        }

        // Filling vertices
        *positions = pos + vv * p->m_Size + ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = pos + vv * p->m_Size - ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = old - vv * p->m_Size - ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u + step;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        *positions = old - vv * p->m_Size + ww * p->m_Size;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif
        *colors = col;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
#else
        colors = (CKDWORD *)((CKBYTE *)colors + data->Colors.Stride);
#endif
        uvs->u = u;
        uvs->v = v + step;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        p = p->next;
        ni += 4;
    }

    dev->DrawPrimitive(VX_TRIANGLELIST, ParticleEmitter::m_GlobalIndices, pc * 6, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}

// TODO : change this callback : not very nice...
int RenderParticles_O(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    //	int todo_manage_reflection_in_objects;
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    CKGroup *group = (CKGroup *)dev->GetCKContext()->GetObject(em->m_Group);
    if (!group)
        return 1;
    CK3dEntity *ent;
    CKMaterial *mat;
    CKMesh *mesh;
    Particle *p = em->GetParticles();
    VxVector dir, up, right;
    int oc = group->GetObjectCount();
    while (p)
    {
        if (p->m_GroupIndex < oc)
            ent = (CK3dEntity *)group->GetObject(p->m_GroupIndex);
        else
            ent = (CK3dEntity *)group->GetObject(0);
        if (CKIsChildClassOf(ent, CKCID_3DENTITY))
        {
            mesh = ent->GetCurrentMesh();
            if (mesh)
            {
                mat = mesh->GetFaceMaterial(0);
                if (mat)
                    mat->SetDiffuse((p->m_Color));

                ent->SetPosition(&(p->pos), NULL);
                dir = Normalize(p->dir);
                up.Set(0, 1, 0);
                right = CrossProduct(up, dir);
                up = CrossProduct(dir, right);
                ent->SetOrientation(&dir, &up, &right);
                if (p->m_Angle)
                {
                    ent->Rotate(&dir, p->m_Angle);
                }
                {
                    VxVector vec(p->m_Size, p->m_Size, p->m_Size);
                    ent->SetScale(&vec);
                }
                ent->Render(dev, FALSE);
            }
        }
        p = p->next;
    }

    return 1;
}
