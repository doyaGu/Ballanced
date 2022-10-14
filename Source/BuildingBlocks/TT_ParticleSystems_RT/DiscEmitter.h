#ifndef DISCEMITTER_H
#define DISCEMITTER_H

#include "PointEmitter.h"

class DiscEmitter : public PointEmitter
{
public:
    DiscEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name) {}

protected:
    // add a particle
    void InitiateParticle(Particle *);
};

#endif