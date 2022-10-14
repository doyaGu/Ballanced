#include "CKAll.h"

#include "CurveEmitter.h"

void CurveEmitter::InitiateParticle(Particle *p)
{
    m_Curve->GetPos(rand() * INV_RAND, &(p->pos));
}

void CurveEmitter::InitiateDirection(Particle *p)
{
    VxVector newDir;
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->GetOrientation(NULL, &newDir);
    VxMatrix mat;
    Vx3DMatrixFromEulerAngles(mat, m_YawVariation * RANDNUM, 0.0, m_PitchVariation * RANDNUM);
    Vx3DRotateVector(&(p->dir), mat, &newDir);
}
