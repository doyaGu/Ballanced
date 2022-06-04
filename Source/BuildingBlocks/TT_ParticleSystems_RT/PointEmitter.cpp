#include "PointEmitter.h"

#include "TT_ParticleSystems_RT.h"

void PointEmitter::InitiateParticle(Particle *p)
{
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->GetPosition(&(p->pos));
}
