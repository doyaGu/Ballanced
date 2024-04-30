#ifndef TIMEPOINTEMITTER_H
#define TIMEPOINTEMITTER_H

#include "PointEmitter.h"

class TimePointEmitter : public PointEmitter
{
public:
    TimePointEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name), m_Vector(VxVector::axis0()), m_EmissionDelay(0)
    {
        hasBeenRendered = TRUE;
        hasBeenEnqueud = TRUE;
    }

    VxVector m_Vector;
    float m_EmissionDelay;
    CKDWORD m_Unknown;

protected:
    // add a particle
    void InitiateParticle(Particle *p);
};

#endif