#include "CKAll.h"

#include "DiscEmitter.h"

void DiscEmitter::InitiateParticle(Particle *p)
{
    VxVector randpos(RANDNUM, RANDNUM, 0.0);
    if (SquareMagnitude(randpos) > 1.0) randpos *= 0.5f;
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->Transform(&p->pos, &randpos);
}
