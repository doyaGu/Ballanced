#include "CKAll.h"

#include "ParticleManager.h"
#include "ParticleEmitter.h"

void ParticleManager::ManageGravity(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_GravityAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CKBeObject *ent = (CKBeObject *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_GravityAttribute);
        float gravityforce;
        pout->GetValue(&gravityforce);

        Particle *particle = em->particles;
        while (particle)
        {
            particle->dir.y += deltat * gravityforce * particle->m_Weight;
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageAtmosphere(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_AtmosphereAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CKBeObject *ent = (CKBeObject *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_AtmosphereAttribute);
        float atmoforce;
        pout->GetValue(&atmoforce);

        Particle *particle = em->particles;
        while (particle)
        {
            particle->dir -= (particle->dir * atmoforce * deltat * particle->m_Surface);
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageGlobalWind(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_GlobalWindAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_GlobalWindAttribute);
        float globalWindforce;
        pout->GetValue(&globalWindforce);

        VxVector wind;
        ent->GetOrientation(&wind, NULL, NULL);

        Particle *particle = em->particles;
        while (particle)
        {
            particle->dir += wind * (globalWindforce * deltat * particle->m_Surface);
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageLocalWind(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_LocalWindAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_LocalWindAttribute);
        Vx2DVector v2;
        if (pout)
            pout->GetValue(&v2);
        float localWindforce = v2.x;
        float decayforce = v2.y;

        VxVector wind;
        ent->GetOrientation(&wind, NULL, NULL);
        VxVector windori;
        ent->GetPosition(&windori);

        Particle *particle = em->particles;

        float radius = ent->GetRadius();
        VxVector ppos;
        while (particle)
        {
            ent->InverseTransform(&ppos, &particle->pos);

            if (ppos.z > 0)
            { // we check if we are in front of the wind emitter
                if ((ppos.x * ppos.x + ppos.y * ppos.y) < radius * radius)
                {
                    float att = 1.0f / (1.0f + ppos.z * decayforce);
                    particle->dir += wind * (att * localWindforce * deltat * particle->m_Surface);
                }
            }

            particle = particle->next;
        }
    }
}

void ParticleManager::ManageMagnet(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_MagnetAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        float radius = ent->GetRadius();

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_MagnetAttribute);
        float magnetforce;
        pout->GetValue(&magnetforce);

        VxVector magpos;
        ent->GetPosition(&magpos);

        Particle *particle = em->particles;

        VxVector dir;
        float t;
        while (particle)
        {
            dir = magpos - particle->pos + particle->dir * deltat;

            // we calculate the new particle direction
            // particle->pos += dir*(magnetforce/(1.0f+SquareMagnitude(dir)));
            t = Magnitude(dir);
            if (t > 0.0f)
            {
                if (t > radius)
                {
                    dir /= (t * t);
                    particle->dir += magnetforce * dir * deltat * 0.001f;
                }
                else
                {
                    particle->dir *= 1.0f / (1.0f + t);
                }
            }

            particle = particle->next;
        }
    }
}

void ParticleManager::ManageVortex(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_VortexAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_VortexAttribute);
        VxVector vortexforce;
        pout->GetValue(&vortexforce);

        VxVector ppos, newpos;

        Particle *particle = em->particles;

        while (particle)
        {
            // we calculate the new particle position
            ent->InverseTransform(&ppos, &particle->pos);

            float invmag = 1.0f / (1.0f + SquareMagnitude(ppos));

            VxMatrix mat;
            Vx3DMatrixFromEulerAngles(mat, vortexforce.x * invmag, vortexforce.y * invmag, vortexforce.z * invmag);
            Vx3DRotateVector(&newpos, mat, &ppos);
            ent->Transform(&particle->pos, &newpos);

            particle = particle->next;
        }
    }
}

void ParticleManager::ManageDisruptionBox(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_DisruptionBoxAttribute);

    CK3dEntity *emitter = (CK3dEntity *)CKGetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_DisruptionBoxAttribute);
        VxVector disruptionBoxforce;
        pout->GetValue(&disruptionBoxforce);

        const VxBbox &box = ent->GetBoundingBox();

        ////////////////////////////////////
        // optimisation : tester d'abord les  overlapping de bounding boxs
        // avant de tester chaque particuls
        ////////////////////////////////////
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(emibox, box))
            continue;

        Particle *particle = em->particles;
        while (particle)
        {
            // we calculate the new particle position
            if (box.VectorIn(particle->pos))
            {
                VxVector newpos;
                newpos.x = box.Min.x + (box.Max.x - box.Min.x) * (float)rand() / RAND_MAX;
                newpos.y = box.Min.y + (box.Max.y - box.Min.y) * (float)rand() / RAND_MAX;
                newpos.z = box.Min.z + (box.Max.z - box.Min.z) * (float)rand() / RAND_MAX;

                particle->dir += (newpos - particle->pos) * disruptionBoxforce * deltat;
            }
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageMutationBox(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_MutationBoxAttribute);

    CK3dEntity *emitter = (CK3dEntity *)CKGetObject(em->m_Entity);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        const VxBbox &box = ent->GetBoundingBox();

        ////////////////////////////////////
        // optimisation : tester d'abord les  overlapping de bounding boxs
        // avant de tester chaque particuls
        ////////////////////////////////////
        const VxBbox &emibox = emitter->GetBoundingBox();
        if (!VxIntersect::AABBAABB(emibox, box))
            continue;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_MutationBoxAttribute);
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        float size;
        pout = (CKParameterOut *)CKGetObject(paramids[0]);
        pout->GetValue(&size);
        float sizespeed;
        pout = (CKParameterOut *)CKGetObject(paramids[1]);
        pout->GetValue(&sizespeed);
        sizespeed *= deltat;
        VxColor color;
        pout = (CKParameterOut *)CKGetObject(paramids[2]);
        pout->GetValue(&color);
        float colorspeed;
        pout = (CKParameterOut *)CKGetObject(paramids[3]);
        pout->GetValue(&colorspeed);
        colorspeed *= deltat;

        Particle *particle = em->particles;
        while (particle)
        {
            // we calculate the new particle position
            if (box.VectorIn(particle->pos))
            {
                if (sizespeed)
                {
                    particle->m_Size += (size - particle->m_Size) * sizespeed;
                    particle->m_DeltaSize = 0;
                }
                if (colorspeed)
                {
                    particle->m_Color.r += (color.r - particle->m_Color.r) * colorspeed;
                    particle->m_Color.g += (color.g - particle->m_Color.g) * colorspeed;
                    particle->m_Color.b += (color.b - particle->m_Color.b) * colorspeed;
                    particle->m_Color.a += (color.a - particle->m_Color.a) * colorspeed;
                    particle->deltaColor.r = 0;
                    particle->deltaColor.g = 0;
                    particle->deltaColor.b = 0;
                    particle->deltaColor.a = 0;
                }
            }
            particle = particle->next;
        }
    }
}

void ParticleManager::ManageTunnel(ParticleEmitter *em, float deltat)
{
    // Rescaling deltat
    deltat *= 0.001f;

    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_TunnelAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_TunnelAttribute);
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
        float velocity;
        pout = (CKParameterOut *)CKGetObject(paramids[0]);
        pout->GetValue(&velocity);
        float magnetradius;
        pout = (CKParameterOut *)CKGetObject(paramids[1]);
        pout->GetValue(&magnetradius);
        float magnetforce;
        pout = (CKParameterOut *)CKGetObject(paramids[2]);
        pout->GetValue(&magnetforce);
        float attractionspeed;
        pout = (CKParameterOut *)CKGetObject(paramids[3]);
        pout->GetValue(&attractionspeed);
        float attractiondirection;
        pout = (CKParameterOut *)CKGetObject(paramids[4]);
        pout->GetValue(&attractiondirection);

        const VxMatrix &worldmatrix = ent->GetWorldMatrix();
        VxVector dirz;
        dirz.x = worldmatrix[2][0];
        dirz.y = worldmatrix[2][1];
        dirz.z = worldmatrix[2][2];
        float scalez = Magnitude(worldmatrix[2]);
        float scalex = Magnitude(worldmatrix[0]);
        float invscalex = 1.0f / scalex;
        dirz /= scalez;

        VxVector tunnelposition;
        ent->GetPosition(&tunnelposition);

        Particle *particle = em->particles;

        VxVector ppos;
        while (particle)
        {
            ent->InverseTransform(&ppos, &particle->pos);

            if (ppos.z < 1.0f)
            {
                // Something must be done
                if (ppos.z < 0.0f)
                {
                    // we are before the tunnel
                    // We must see if we are in the radius of the magnet
                    float worlddistance = Magnitude(particle->pos - tunnelposition);
                    if (worlddistance < magnetradius)
                    {
                        float fx = (float)fabs(ppos.x);
                        float fy = (float)fabs(ppos.y);

                        // Rescaling z to x : whatever the tunnel length, it will react the same at the entrance
                        float rescale = scalez * magnetforce * invscalex;
                        ppos.z *= rescale;
                        float squarez = (ppos.z) * (ppos.z);
                        if ((fx < squarez + 1) && (fy < squarez + 1))
                        {
                            // Inside the stream
                            float invsquarez = 1.0f / (squarez + 1);
                            float alphax = (ppos.x) * invsquarez;
                            float alphay = (ppos.y) * invsquarez;
                            VxVector inlocal(2 * alphax * (ppos.z), 2 * alphay * (ppos.z), 1.0f / rescale);
                            VxVector inworld;
                            ent->TransformVector(&inworld, &inlocal);
                            inworld.Normalize();

                            // the current particle speed
                            float speed = Magnitude(particle->dir);

                            // Linear interpolation
                            float newspeed = speed + particle->m_Weight * deltat * attractionspeed * (velocity - speed);

                            VxVector dir = particle->dir / speed;
                            VxVector newdir = dir + particle->m_Weight * deltat * attractiondirection * (inworld - dir);

                            particle->dir = newdir * newspeed;
                        }
                    }
                }
                else
                {
                    if ((fabs(ppos.x) < 1.0f) && (fabs(ppos.y) < 1.0f))
                    {
                        // Inside the tunnel
                        particle->dir = dirz * velocity;
                    }
                }
            }

            particle = particle->next;
        }
    }
}

void ParticleManager::ManageProjector(ParticleEmitter *em, float deltat)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(m_ProjectorAttribute);

    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CKSprite3D *ent = (CKSprite3D *)*it;
        Vx2DVector size;
        ent->GetSize(size);

        VxColor sprcolor = ent->GetMaterial()->GetDiffuse();

        CKTexture *tex = ent->GetMaterial()->GetTexture();
        float width = (float)tex->GetWidth();
        float height = (float)tex->GetHeight();

        // we get the attribute value
        CKParameterOut *pout = ent->GetAttributeParameter(m_ProjectorAttribute);
        VxVector value;
        pout->GetValue(&value);

        float nearplane = 0.5f / tanf(value.x * 0.5f);

        VxMatrix proj;
        proj.Perspective(value.x, value.y, nearplane, nearplane + value.z);

        VxMatrix world = ent->GetWorldMatrix();
        world[0] *= -size.x;
        world[1] *= size.y;
        world[2] *= -1.0f;
        world[3] -= nearplane * Normalize(world[2]);
        VxMatrix invworld;
        Vx3DInverseMatrix(invworld, world);

        VxMatrix viewproj;
        Vx3DMultiplyMatrix4(viewproj, proj, invworld);

        VxVector4 p;

        Particle *particle = em->particles;
        while (particle)
        {
            Vx3DMultiplyMatrixVector4(&p, viewproj, &particle->pos);

            if (p.z > 0.0f && p.z < p.w)
            {
                if ((p.x > -p.w) && (p.x < p.w))
                {
                    if ((p.y > -p.w) && (p.y < p.w))
                    {
                        float invw = 1.0f / p.w;
                        p.x = (-p.x * invw + 1.0f) * 0.5f;
                        p.y = (-p.y * invw + 1.0f) * 0.5f;
                        p.z = 1.0f - p.z * invw;
                        particle->m_Color.Set(tex->GetPixel((int)(p.x * width), (int)(p.y * height)));
                        particle->m_Color *= sprcolor;
                        particle->m_Color.a *= p.z;
                    }
                    else
                        particle->m_Color.Set((CKDWORD)0);
                }
                else
                    particle->m_Color.Set((CKDWORD)0);
            }
            else
                particle->m_Color.Set((CKDWORD)0);
            particle = particle->next;
        }
    }
}