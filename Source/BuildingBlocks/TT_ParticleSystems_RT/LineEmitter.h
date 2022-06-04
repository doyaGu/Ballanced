#ifndef BUILDINGBLOCKS_LINEEMITTER_H
#define BUILDINGBLOCKS_LINEEMITTER_H

#include "PointEmitter.h"

class LineEmitter : public PointEmitter
{
public:
    LineEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name){};

protected:
    // add a particle
    void InitiateParticle(Particle *);
};

#endif // BUILDINGBLOCKS_LINEEMITTER_H