#include "LineEmitter.h"

#include "TT_ParticleSystems_RT.h"

void LineEmitter::InitiateParticle(Particle *p)
{
    VxVector randpos(RANDNUM, 0.0, 0.0);
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->Transform(&p->pos, &randpos);
}
