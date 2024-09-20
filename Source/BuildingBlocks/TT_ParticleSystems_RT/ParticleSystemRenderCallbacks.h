#ifndef PARTICLESYSTEMRENDERCALLBACK_H
#define PARTICLESYSTEMRENDERCALLBACK_H

// New Render Callback

int RenderParticles_P(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_L(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_S(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_O(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_OS(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_CS(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_RS(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int RenderParticles_FS(CKRenderContext *dev, CKRenderObject *mov, void *arg);

#endif