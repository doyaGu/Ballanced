#ifndef BUILDINGBLOCKS_SPHERICALEMITTER_H
#define BUILDINGBLOCKS_SPHERICALEMITTER_H

#include "PointEmitter.h"

class SphericalEmitter : public PointEmitter
{
public:
    SphericalEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name){};

protected:
    // add a particle
    void InitiateParticle(Particle *);
    void InitiateDirection(Particle *);
};

#endif BUILDINGBLOCKS_SPHERICALEMITTER_H