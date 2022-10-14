#include "CKAll.h"

#include <time.h>

#include "SphericalEmitter.h"

void SphericalEmitter::InitiateParticle(Particle *p)
{
    VxVector randpos(RANDNUM, RANDNUM, RANDNUM);
    randpos.Normalize();

    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->Transform(&p->pos, &randpos);
    VxVector newdir;
    entity->TransformVector(&newdir, &randpos);
    p->dir = Normalize(newdir);
}

void SphericalEmitter::InitiateDirection(Particle *p)
{
}
