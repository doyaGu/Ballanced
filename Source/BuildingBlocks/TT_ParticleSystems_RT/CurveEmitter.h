#ifndef BUILDINGBLOCKS_CURVEEMITTER_H
#define BUILDINGBLOCKS_CURVEEMITTER_H

#include "PointEmitter.h"

class CurveEmitter : public PointEmitter
{
public:
    CurveEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name) { m_Curve = (CKCurve *)m_Context->GetObject(ent); }

    CKCurve *m_Curve;

protected:
    // add a particle
    void InitiateParticle(Particle *);
    void InitiateDirection(Particle *);
};

#endif // BUILDINGBLOCKS_CURVEEMITTER_H