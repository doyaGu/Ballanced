#include "CKAll.h"

#include "ParticleEmitter.h"
#include "ParticleSystemRenderCallbacks.h"

int (*renderParticles)(CKRenderContext *dev, CKRenderObject *obj, void *arg);

// ACC - July 10,2002
// Need to add code into each RenderCB to test if shutdown
// Then we need to wait on PE to be finished
//
extern VxMutex logguard;
extern FILE *ACCLOG;

#ifdef USE_THR
void ThreadWaitForCompletion(ParticleEmitter *em, const char *RenderingMethod)
{
#ifdef MT_VERB
    // ACC, July 10, 2002
    {
        VxMutexLock lock(logguard);
        fprintf(ACCLOG, "About to render a %s\n", RenderingMethod);
        fflush(ACCLOG);
    }
#endif

    // Wait
    if (!em->hasBeenRendered && em->hasBeenEnqueued)
    {
        WaitForSingleObject(em->hasBeenComputedEvent, INFINITE);
        em->hasBeenRendered = TRUE;
    }
}
#endif

int RenderParticles_P(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    ParticleEmitter *em = (ParticleEmitter *)arg;

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Point");
#endif

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

    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    while (p)
    {
        *positions = p->pos;
        *colors = RGBAFTOCOLOR(&(p->m_Color));

        // next point
        p = p->next;

        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Line");
#endif

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

    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    while (p)
    {
        // Colors
        oldColor.r = p->m_Color.r - p->deltaColor.r * p->m_DeltaTime;
        oldColor.g = p->m_Color.g - p->deltaColor.g * p->m_DeltaTime;
        oldColor.b = p->m_Color.b - p->deltaColor.b * p->m_DeltaTime;
        oldColor.a = p->m_Color.a - p->deltaColor.a * p->m_DeltaTime;

        // Start of line
        *positions = p->pos - p->dir * p->m_DeltaTime;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

        *colors = RGBAFTOCOLOR(&oldColor);
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);

        // End of line
        *positions = p->pos + p->dir * p->m_Angle;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

        *colors = RGBAFTOCOLOR(&(p->m_Color));
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);

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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Sprite");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    const int VBUFFERSIZE = 4000;

    int pc = em->particleCount;
    if (!pc)
        return 0;
#ifndef PSX2
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT | CKRST_DP_VBUFFER), (4 * pc > VBUFFERSIZE) ? VBUFFERSIZE : 4 * pc);
#else
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT), (4 * pc > VBUFFERSIZE) ? VBUFFERSIZE : 4 * pc);
#endif

    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

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

    BOOL changeuv = FALSE;
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
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos + cr + cu;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos + cr - cu;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos - cr - cu;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

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
#ifndef PSX2
                data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT | CKRST_DP_VBUFFER), (remaining > VBUFFERSIZE) ? VBUFFERSIZE : remaining);
#else
                data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT), (remaining > VBUFFERSIZE) ? VBUFFERSIZE : remaining);
#endif
                uvs = (VxUV *)data->TexCoordPtr;
                positions = (VxVector *)data->PositionPtr;
                colors = (CKDWORD *)data->ColorPtr;
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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "FastSprite");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 3 * pc);

    XPtrStrided<VxUV> uvs(data->TexCoordPtr, data->TexCoordStride);
    XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
    XPtrStrided<CKDWORD> colors(data->ColorPtr, data->ColorStride);

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

    BOOL changeuv = FALSE;
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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "OrientableSprite");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;
#ifndef PSX2
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT /*|CKRST_DP_VBUFFER*/), 4 * pc);
#else
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT), 4 * pc);
#endif

    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

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

    BOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)((float)(sqrtf((float)(em->m_TextureFrameCount - 1))));
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
        // we exagerate the current position according to the latenty
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
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos + vv * p->m_Size - ww * p->m_Size;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = old - vv * oldsize - ww * oldsize;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = oldcol;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = old - vv * oldsize + ww * oldsize;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = oldcol;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Comet");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * pc);
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

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

    BOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)((float)(sqrtf((float)(em->m_TextureFrameCount - 1))));
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
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = center - ww;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = old;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = oldcol;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

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
    // ACC, July 10, 2002

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Radial");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int pc = em->particleCount;
    if (!pc)
        return 0;
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * pc);
    CKWORD *indices = dev->GetDrawPrimitiveIndices(pc * 6);

    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    // Render States
    em->SetState(dev);

    CK3dEntity *cam = dev->GetViewpoint();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    dev->SetWorldTransformationMatrix(cam->GetWorldMatrix());

    BOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)(sqrtf((float)(em->m_TextureFrameCount - 1)));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    VxVector pos, old(0, 0, 0), vv(0, 0, 0), ww(0, 0, 0);

    float u = 0.0f;
    float v = 0.0f;

    CK3dEntity *ent = (CK3dEntity *)dev->GetCKContext()->GetObject(em->m_Entity);
    cam->InverseTransform(&old, &vv, ent);
    const VxMatrix &worldCam = cam->GetWorldMatrix();

    old += Normalize(old) * 40.0f;

    CK_ID oldtex = -1;

    const VxMatrix &invCam = cam->GetInverseWorldMatrix();

    VxMatrix finalMat = /*ent->GetWorldMatrix()*/ invCam;

    int fi = 0;
    int ni = 0;
    while (p)
    {
        Vx3DMultiplyMatrixVector(&pos, finalMat, &p->pos);

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
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = pos + vv * p->m_Size - ww * p->m_Size;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = old - vv * p->m_Size - ww * p->m_Size;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u + step;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

        *positions = old - vv * p->m_Size + ww * p->m_Size;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *colors = col;
        colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
        uvs->u = u;
        uvs->v = v + step;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

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

#ifdef USE_THR
    ThreadWaitForCompletion(em, "Object");
#endif

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

int RenderParticles_PS(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
	ParticleEmitter *em = (ParticleEmitter*)arg;

#ifdef USE_THR
	ThreadWaitForCompletion(em,"PointSprite");
#endif

    if (em->m_IsTimePointEmitter)
        em->AddParticles2();

    int count = em->particleCount;
    if (count == 0)
        return 0;

    CK3dEntity *cam = dev->GetViewpoint();
    const VxMatrix &mat = cam->GetWorldMatrix();
    VxMatrix oldmatrix = dev->GetWorldTransformationMatrix();
    VxMatrix realmatrix = oldmatrix * mov->GetInverseWorldMatrix();
    dev->SetWorldTransformationMatrix(realmatrix);

    int pc = em->particleCount;
    if (!pc)
        return 0;
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_VCT, 4 * pc);
    CKWORD *indices = dev->GetDrawPrimitiveIndices(6 * pc);

    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector *positions = (VxVector *)data->PositionPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;

    // Render States
    em->SetState(dev);

    BOOL changeuv = FALSE;
    float step = 1.0f;
    int framehc = 0;
    if (em->m_TextureFrameCount > 1)
    {
        changeuv = TRUE;
        framehc = 1 + (int)(sqrtf((float)(em->m_TextureFrameCount - 1)));
        step = 1.0f / framehc;
    }

    Particle *p = em->GetParticles();

    if (p)
    {
        int ni = 0;
        for (CKWORD *i = indices; p != NULL; i += 6)
        {
            i[0] = 4 * ni;
            i[1] = 4 * ni + 1;
            i[2] = 4 * ni + 2;
            i[3] = 4 * ni;
            i[4] = 4 * ni + 2;
            i[5] = 4 * ni + 3;

            float x = p->pos.x;
            float y = p->pos.y;
            float z = p->pos.z;

            if (p->m_Angle == 0.0f)
            {
                positions->x = x - p->m_Size;
                positions->y = y;
                positions->z = z + p->m_Size;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x + p->m_Size;
                positions->y = y;
                positions->z = z + p->m_Size;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x + p->m_Size;
                positions->y = y;
                positions->z = z - p->m_Size;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x - p->m_Size;
                positions->y = y;
                positions->z = z - p->m_Size;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
            }
            else
            {
                float hs = p->m_Size * 0.5;
                float ca = cos(p->m_Angle);
                float sa = sin(p->m_Angle);

                positions->x = x - hs * ca + hs * sa;
                positions->y = y;
                positions->z = z + hs * sa + hs * ca;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x + hs * ca + hs * sa;
                positions->y = y;
                positions->z = z - hs * ca + hs * ca;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x + hs * ca - hs * sa;
                positions->y = y;
                positions->z = z - hs * ca - hs * ca;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

                positions->x = x - hs * ca - hs * sa;
                positions->y = y;
                positions->z = z + hs * sa;
                positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
            }

            // the colors
            CKDWORD col = RGBAFTOCOLOR(&(p->m_Color));
            *colors = col;
            colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
            *colors = col;
            colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
            *colors = col;
            colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);
            *colors = col;
            colors = (CKDWORD *)((CKBYTE *)colors + data->ColorStride);

            // The texture coordinates
            if (changeuv)
            {
                float v = 0.0f;
                int c = p->m_CurrentFrame;

                while (c >= framehc)
                {
                    v += step;
                    c -= framehc;
                }

                uvs->u = c * step;
                uvs->v = v;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = c * step + step;
                uvs->v = v;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = c * step + step;
                uvs->v = v + step;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = c * step;
                uvs->v = v + step;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
            }
            else
            {
                uvs->u = 0.0f;
                uvs->v = 0.0f;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = 1.0f;
                uvs->v = 0.0f;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = 1.0f;
                uvs->v = 1.0f;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);

                uvs->u = 0.0f;
                uvs->v = 1.0f;
                uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
            }

            p = p->next;
            ++ni;
        }
    }

    dev->DrawPrimitive(VX_TRIANGLELIST, indices, pc * 6, data);

    // we let write to the ZBuffer
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);

    dev->SetWorldTransformationMatrix(oldmatrix);
    return 1;
}
