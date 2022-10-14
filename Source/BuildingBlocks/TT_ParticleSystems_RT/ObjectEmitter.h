#ifndef OBJECTEMITTER_H
#define OBJECTEMITTER_H

#include "PointEmitter.h"

class ObjectEmitter : public PointEmitter
{
public:
    ObjectEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name), m_Shape(NULL) {}

    CKMesh *m_Shape;

protected:
    // add a particle
    void InitiateParticle(Particle *);
    void InitiateDirection(Particle *);
};

#endif