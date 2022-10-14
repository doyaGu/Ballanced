#ifndef CylindricalEMITTER_H
#define CylindricalEMITTER_H

#include "PointEmitter.h"

class CylindricalEmitter : public PointEmitter {
public:
    CylindricalEmitter(CKContext* ctx,CK_ID ent,char* name) : PointEmitter(ctx,ent,name) {}

protected:
    // add a particle
    void InitiateParticle(Particle*);
    void InitiateDirection(Particle*);
};

#endif