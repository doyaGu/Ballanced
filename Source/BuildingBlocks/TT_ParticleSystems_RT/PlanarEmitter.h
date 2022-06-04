#ifndef BUILDINGBLOCKS_PLANAREMITTER_H
#define BUILDINGBLOCKS_PLANAREMITTER_H

#include "PointEmitter.h"

class PlanarEmitter : public PointEmitter
{
public:
    PlanarEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name){};

protected:
    // add a particle
    void InitiateParticle(Particle *p);
};

#endif // BUILDINGBLOCKS_PLANAREMITTER_H