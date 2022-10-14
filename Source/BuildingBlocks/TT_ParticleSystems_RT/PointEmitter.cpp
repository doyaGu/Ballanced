#include "CKAll.h"

#include "PointEmitter.h"

void PointEmitter::InitiateParticle(Particle *p)
{
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->GetPosition(&(p->pos));
}
