#include "CKAll.h"

#include "ParticleManager.h"
#include "ParticleEmitter.h"

Particle *KillParticle(Particle *iParticle, ParticleEmitter *ioEmitter)
{
    Particle *tmp = iParticle->next;
    if (iParticle->prev)
        iParticle->prev->next = iParticle->next;
    else
        ioEmitter->particles = iParticle->next;

    if (iParticle->next)
        iParticle->next->prev = iParticle->prev;

    iParticle->next = ioEmitter->m_Pool;
    ioEmitter->m_Pool = iParticle;
    ioEmitter->particleCount--; // ADD ONE TO POOL
    return tmp;
}

void ParticleManager::ManageInfinitePlaneDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response;
    float friction;
    float density;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DInfinitePlaneAttribute);

    VxVector prevpos, olddir, intpoint, o_intnormal, intnormal, d, O;

    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DInfinitePlaneAttribute);
        if (!pout)
            continue;
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        pout->GetValue(&density);

        float invDeltaTime = 1.0f / deltat;

        o_intnormal.x = 0.0f;
        o_intnormal.y = 1.0f;
        o_intnormal.z = 0.0f;
        ent->GetPosition(&O);
        ent->TransformVector(&intnormal, &o_intnormal, NULL);
        intnormal.Normalize();

        ParticleImpact pi;
        Particle *particle = em->particles;
        while (particle)
        {
            olddir = particle->dir * particle->m_DeltaTime;
            prevpos = particle->pos - olddir;

            if (rand() * INV_RAND < density)
            {
                if (RayPlaneIntersection(intnormal, O, prevpos, particle->pos, intpoint))
                {
                    float ps = DotProduct(olddir, intnormal);
                    d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);
                    particle->pos = intpoint + d;
                    particle->dir = d * invDeltaTime;

                    if (em->m_DeflectorsFlags & PD_IMPACTS)
                    {
                        pi.m_Position = intpoint;
                        pi.m_Direction = intnormal;
                        pi.m_Object = ent;
                        em->m_Impacts.PushBack(pi);
                    }

                    if (!messageSend)
                    {
                        CKMessageManager *mm = m_Context->GetMessageManager();
                        mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                        messageSend = TRUE;
                    }

                    if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                    {
                        particle = KillParticle(particle, em);
                        continue;
                    }
                }
            }

            particle = particle->next;
        }
    }
}

void ParticleManager::ManagePlaneDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response = 1.0f;
    float friction = 1.0f;
    float density = 1.0f;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DPlaneAttribute);

    VxVector olddir, o_intpoint, intpoint, o_intnormal, intnormal, d, O;

    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // World Boxes fast rejection
        VxBbox worldbox = ent->GetBoundingBox();
        worldbox.Max.y += 0.1f;
        worldbox.Min.y -= 0.1f;
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(worldbox, emibox))
            continue;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DPlaneAttribute);
        if (!pout)
            continue;
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        if (pout)
            pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        if (pout)
            pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        if (pout)
            pout->GetValue(&density);

        float invDeltaTime = 1.0f / deltat;

        Particle *particle = em->particles;
        o_intnormal.x = 0.0f;
        o_intnormal.y = 1.0f;
        o_intnormal.z = 0.0f;
        ent->GetPosition(&O);
        ent->TransformVector(&intnormal, &o_intnormal, NULL);
        intnormal.Normalize();

        ParticleImpact pi;
        while (particle)
        {
            if (particle->m_Density < density)
            {
                if (RayPlaneIntersection(intnormal, O, particle->prevpos, particle->pos, intpoint))
                {
                    ent->InverseTransform(&o_intpoint, &intpoint);
                    if (((o_intpoint.x > -1) && (o_intpoint.x < 1)) && ((o_intpoint.z > -1) && (o_intpoint.z < 1)))
                    {
                        olddir = particle->pos - particle->prevpos;

                        float ps = DotProduct(olddir, intnormal);
                        d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);
                        particle->pos = intpoint + d;
                        particle->dir = d * invDeltaTime;

                        if (em->m_DeflectorsFlags & PD_IMPACTS)
                        {
                            pi.m_Position = intpoint;
                            pi.m_Direction = intnormal;
                            pi.m_Object = ent;
                            pi.m_UVs.Set(0.5f * (o_intpoint.x + 1.0f), 0.5f * (-o_intpoint.z + 1.0f));
                            em->m_Impacts.PushBack(pi);
                        }

                        if (!messageSend)
                        {
                            CKMessageManager *mm = m_Context->GetMessageManager();
                            mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                            messageSend = TRUE;
                        }

                        if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                        {
                            particle = KillParticle(particle, em);
                            continue;
                        }
                    }
                }
            }

            particle = particle->next;
        }
    }
}

void ParticleManager::ManageCylinderDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response;
    float friction;
    float density;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DCylinderAttribute);

    VxVector prevpos, olddir, o_prevpos, o_ppos, o_intpoint, intpoint, o_intnormal, intnormal, d;

    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // World Boxes fast rejection
        const VxBbox &worldbox = ent->GetBoundingBox();
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(worldbox, emibox))
            continue;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DCylinderAttribute);
        if (!pout)
            continue;
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        pout->GetValue(&density);

        float invDeltaTime = 1.0f / deltat;

        ParticleImpact pi;
        Particle *particle = em->particles;
        while (particle)
        {
            olddir = particle->dir * particle->m_DeltaTime;
            prevpos = particle->pos - olddir;

            if (rand() * INV_RAND < density)
            {
                ent->InverseTransform(&o_prevpos, &prevpos);
                ent->InverseTransform(&o_ppos, &(particle->pos));

                if (RayTubeIntersection(o_prevpos, o_ppos, o_intpoint, o_intnormal))
                {

                    ent->TransformVector(&intnormal, &o_intnormal);
                    intnormal.Normalize();
                    ent->Transform(&intpoint, &o_intpoint);
                    float ps = DotProduct(olddir, intnormal);
                    d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);
                    particle->pos = intpoint + d;
                    particle->dir = d * invDeltaTime;

                    if (em->m_DeflectorsFlags & PD_IMPACTS)
                    {
                        pi.m_Position = intpoint;
                        pi.m_Direction = intnormal;
                        pi.m_Object = ent;
                        em->m_Impacts.PushBack(pi);
                    }

                    if (!messageSend)
                    {
                        CKMessageManager *mm = m_Context->GetMessageManager();
                        mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                        messageSend = TRUE;
                    }

                    if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                    {
                        particle = KillParticle(particle, em);
                        continue;
                    }
                }
            }
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageSphereDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response;
    float friction;
    float density;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DSphereAttribute);

    VxVector prevpos, olddir, o_prevpos, o_ppos, o_intpoint, intpoint, o_intnormal, intnormal, d;

    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // World Boxes fast rejection
        const VxBbox &worldbox = ent->GetBoundingBox();
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(worldbox, emibox))
            continue;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DSphereAttribute);
        if (!pout)
            continue;
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        pout->GetValue(&density);

        float invDeltaTime = 1.0f / deltat;

        float radius = ent->GetRadius();
        VxVector O;
        ent->GetPosition(&O);

        ParticleImpact pi;
        Particle *particle = em->particles;
        while (particle)
        {
            olddir = particle->dir * particle->m_DeltaTime;
            prevpos = particle->pos - olddir;

            if (rand() * INV_RAND < density)
            {
                ent->InverseTransform(&o_prevpos, &prevpos);
                ent->InverseTransform(&o_ppos, &(particle->pos));

                if (RaySphereIntersection(o_prevpos, o_ppos, o_intpoint, o_intnormal))
                {

                    ent->Transform(&intpoint, &o_intpoint);
                    ent->TransformVector(&intnormal, &o_intnormal);
                    intnormal.Normalize();
                    float ps = DotProduct(olddir, intnormal);
                    d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);
                    particle->pos = intpoint + d * invDeltaTime;
                    particle->dir = d * invDeltaTime;

                    if (em->m_DeflectorsFlags & PD_IMPACTS)
                    {
                        pi.m_Position = intpoint;
                        pi.m_Direction = intnormal;
                        pi.m_Object = ent;
                        em->m_Impacts.PushBack(pi);
                    }

                    if (!messageSend)
                    {
                        CKMessageManager *mm = m_Context->GetMessageManager();
                        mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                        messageSend = TRUE;
                    }

                    if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                    {
                        particle = KillParticle(particle, em);
                        continue;
                    }
                }
            }
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageBoxDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response;
    float friction;
    float density;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DBoxAttribute);

    VxVector prevpos, olddir, o_prevpos, o_ppos, o_intpoint, intpoint, o_intnormal, intnormal, d;
    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // World Boxes fast rejection
        const VxBbox &worldbox = ent->GetBoundingBox();
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(worldbox, emibox))
            continue;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DBoxAttribute);
        if (!pout)
            continue;
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        pout->GetValue(&density);

        const VxBbox &box = ent->GetBoundingBox(TRUE);

        float invDeltaTime = 1.0f / deltat;

        ParticleImpact pi;
        Particle *particle = em->particles;
        int res;
        while (particle)
        {
            olddir = particle->dir * particle->m_DeltaTime;
            prevpos = particle->pos - olddir;

            if (rand() * INV_RAND < density)
            {
                ent->InverseTransform(&o_prevpos, &prevpos);
                ent->InverseTransform(&o_ppos, &(particle->pos));

                // TODO : use collision manager functions
                res = RayBoxIntersection(box, o_prevpos, o_ppos, o_intpoint, o_intnormal);
                if (res == -1)
                    res = RayBoxIntersection(box, o_ppos, o_prevpos, o_intpoint, o_intnormal);
                if (res > 0)
                {

                    ent->TransformVector(&intnormal, &o_intnormal);
                    intnormal.Normalize();
                    ent->Transform(&intpoint, &o_intpoint);
                    float ps = DotProduct(olddir, intnormal);
                    d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);

                    /* Old version
                    d = -Normalize(Reflect(olddir,intnormal));
                    d *= (particle->m_Bounce * m_Speed * response);
                    slide = olddir - intnormal*DotProduct(olddir,intnormal);
                    d+= (slide*friction);
                    */
                    // particle->pos = intpoint+d;
                    particle->pos = prevpos;
                    particle->dir = d * invDeltaTime;

                    if (em->m_DeflectorsFlags & PD_IMPACTS)
                    {
                        pi.m_Position = intpoint;
                        pi.m_Direction = intnormal;
                        pi.m_Object = ent;
                        em->m_Impacts.PushBack(pi);
                    }

                    if (!messageSend)
                    {
                        CKMessageManager *mm = m_Context->GetMessageManager();
                        mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                        messageSend = TRUE;
                    }

                    if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                    {
                        particle = KillParticle(particle, em);
                        continue;
                    }
                }
            }
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageObjectDeflectors(ParticleEmitter *em, float deltat)
{
    // Deflectors Message Management
    CKBOOL noNeedToSend = (em->m_MessageType == -1);

    float response;
    float friction;
    float density;
    CKBOOL smooth;
    CKParameterOut *pout;

    CKAttributeManager *attman = m_Context->GetAttributeManager();
    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DObjectAttribute);

    VxVector prevpos, olddir, o_prevpos, intpoint, o_intnormal, intnormal, d;

    CK3dEntity *emitter = (CK3dEntity *)m_Context->GetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get the box
        CK3dEntity *ent = (CK3dEntity *)*it;
        CKBOOL messageSend = noNeedToSend;

        // World Boxes fast rejection
        const VxBbox &worldbox = ent->GetBoundingBox();
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(worldbox, emibox))
            continue;

        // we get the attribute value
        pout = ent->GetAttributeParameter(m_DObjectAttribute);
        if (!pout)
            continue;

        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        pout = (CKParameterOut *)m_Context->GetObject(paramids[0]);
        pout->GetValue(&response);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[1]);
        pout->GetValue(&friction);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[2]);
        pout->GetValue(&density);
        pout = (CKParameterOut *)m_Context->GetObject(paramids[3]);
        pout->GetValue(&smooth);

        float invDeltaTime = 1.0f / deltat;

        ParticleImpact pi;
        Particle *particle = em->particles;
        while (particle)
        {
            olddir = particle->dir * particle->m_DeltaTime;
            prevpos = particle->pos - olddir;

            if (rand() * INV_RAND < density)
            {
                // we test first if the bounding box is intersected
                if (RayInteriorBoxIntersection(worldbox, prevpos, (particle->pos), o_prevpos, o_intnormal))
                {
                    VxIntersectionDesc inter;
                    if (ent->RayIntersection(&prevpos, &(particle->pos), &inter, NULL, CKRAYINTERSECTION_SEGMENT))
                    {
                        // TODO prendre la normale de la face
                        if (smooth)
                        {
                            ent->TransformVector(&intnormal, &inter.IntersectionNormal);
                        }
                        else
                        {
                            CKMesh *mesh = ent->GetCurrentMesh();
                            VxVector n = mesh->GetFaceNormal(inter.FaceIndex);
                            ent->TransformVector(&intnormal, &n);
                        }
                        // Maybe superflu ??
                        intnormal.Normalize();
                        ent->Transform(&intpoint, &inter.IntersectionPoint);
                        float ps = DotProduct(olddir, intnormal);
                        d = (olddir - (intnormal * ps)) * friction - intnormal * (ps * response * particle->m_Bounce);
                        particle->pos = intpoint + d;
                        particle->dir = d * invDeltaTime;

                        if (em->m_DeflectorsFlags & PD_IMPACTS)
                        {
                            pi.m_Position = intpoint;
                            pi.m_Direction = intnormal;
                            pi.m_Object = ent;
                            pi.m_UVs.Set(inter.TexU, inter.TexV);
                            em->m_Impacts.PushBack(pi);
                        }

                        if (!messageSend)
                        {
                            CKMessageManager *mm = m_Context->GetMessageManager();
                            mm->SendMessageSingle(em->m_MessageType, ent, emitter);
                            messageSend = TRUE;
                        }

                        if (em->m_DeflectorsFlags & PD_DIEONIMPACT)
                        {
                            particle = KillParticle(particle, em);
                            continue;
                        }
                    }
                }
            }
            particle = particle->next;
        }
    }
}
