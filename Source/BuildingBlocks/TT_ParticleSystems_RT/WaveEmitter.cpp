#include "CKAll.h"

#include "WaveEmitter.h"

void WaveEmitter::InitiateParticle(Particle *p)
{
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->GetPosition(&p->pos);
    m_Position = p->pos;
}

void WaveEmitter::InitiateDirection(Particle *p) {
    p->dir.Set(0.0f, 0.0f, 0.0f);
}
