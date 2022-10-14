#include "CKAll.h"

#include "ObjectEmitter.h"

void ObjectEmitter::InitiateParticle(Particle *p)
{
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);

    // we get a random face
    int nbface = m_Shape->GetFaceCount();
    int face = rand() % nbface;

    VxVector av, bv, cv, fn;
    fn = m_Shape->GetFaceNormal(face);
    av = m_Shape->GetFaceVertex(face, 0);
    bv = m_Shape->GetFaceVertex(face, 1);
    cv = m_Shape->GetFaceVertex(face, 2);
    float af, bf, cf;
    af = (float)fabs(RANDNUM);
    bf = (float)fabs(RANDNUM);
    cf = (float)fabs(RANDNUM);
    av *= af;
    bv *= bf;
    cv *= cf;

    VxVector entpos = av + bv + cv;
    entpos /= (af + bf + cf);
    entity->Transform(&(p->pos), &entpos);

    // CALCULATE THE STARTING DIRECTION VECTOR
    VxVector endPoint = entpos + fn;
    VxVector newStart, newEnd, newDir;
    entity->Transform(&newStart, &entpos);
    entity->Transform(&newEnd, &endPoint);
    newDir = newEnd - newStart;
    VxMatrix mat;
    Vx3DMatrixFromEulerAngles(mat, m_YawVariation * RANDNUM, m_PitchVariation * RANDNUM, 0.0);
    Vx3DRotateVector(&(p->dir), mat, &newDir);
}

void ObjectEmitter::InitiateDirection(Particle *p)
{
    // Do nothing because already done in the InitiateParticle
    // TODO : may be change this
    // with TransformVector....
}
