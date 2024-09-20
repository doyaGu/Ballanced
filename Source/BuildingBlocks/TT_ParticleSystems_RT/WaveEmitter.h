#ifndef WAVEEMITTER_H
#define WAVEEMITTER_H

#include "ParticleEmitter.h"

class WaveEmitter : public ParticleEmitter
{
public:
    WaveEmitter(CKContext *ctx, CK_ID ent, char *name) : ParticleEmitter(ctx, ent, name)
    {
        CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
        entity->GetPosition(&m_Position);
        m_Angle = 0;
        m_IsWaveEmitter = TRUE;
    }

    VxVector m_Position;
    float m_Angle;

protected:
    // add a particle
    void InitiateParticle(Particle *);
    void InitiateDirection(Particle *p);
};

#endif