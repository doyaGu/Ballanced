#ifndef CUBICEMITTER_H
#define CUBICEMITTER_H

#include "PointEmitter.h"

class CubicEmitter : public PointEmitter
{
public:
    CubicEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name) {}

protected:
    // add a particle
    void InitiateParticle(Particle *p);
};

#endif