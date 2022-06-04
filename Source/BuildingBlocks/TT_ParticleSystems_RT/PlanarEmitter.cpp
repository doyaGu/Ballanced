#include "PlanarEmitter.h"

#include "TT_ParticleSystems_RT.h"

void PlanarEmitter::InitiateParticle(Particle *p)
{
    VxVector randpos(RANDNUM, RANDNUM, 0.0);
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->Transform(&p->pos, &randpos);
}