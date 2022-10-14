#ifndef POINTEMITTER_H
#define POINTEMITTER_H

#include "ParticleEmitter.h"

class PointEmitter : public ParticleEmitter
{
public:
    PointEmitter(CKContext *ctx, CK_ID ent, char *name) : ParticleEmitter(ctx, ent, name) {}

protected:
    // add a particle
    void InitiateParticle(Particle *p);
};

#endif