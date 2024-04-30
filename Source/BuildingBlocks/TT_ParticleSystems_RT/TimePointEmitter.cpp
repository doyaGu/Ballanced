#include "CKAll.h"

#include "TimePointEmitter.h"

void TimePointEmitter::InitiateParticle(Particle *p) {
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->GetPosition(&(p->pos));
}