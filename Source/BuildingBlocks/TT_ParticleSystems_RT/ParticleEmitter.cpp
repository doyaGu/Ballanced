#include "CKAll.h"

#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "ParticleSystemRenderCallbacks.h"

// 128 Bytes to have 16 bytes aligned data for Pentium3 optimisations 32 DWORDS
CKBYTE _PS_AlignedMemory[128];

CKDWORD *g_PS_AlignZone = (CKDWORD *)((CKDWORD)(_PS_AlignedMemory + 15) & 0xFFFFFFF0);
float *g_Min = (float *)g_PS_AlignZone;			 // 16  Bytes  (4 floats)	+4
float *g_Max = (float *)(g_PS_AlignZone + 4);	 // 16  Bytes  (4 floats)	+8
float *g_One = (float *)(g_PS_AlignZone + 8);	 // 16  Bytes  (4 floats)	+12 (1.0f,1.0f,1.0f,1.0f);
float *g_Delta = (float *)(g_PS_AlignZone + 12); // 16  Bytes  (4 floats)	+16 (delta,delta,delta,delta);

CKWORD *ParticleEmitter::m_GlobalIndices = NULL;
int ParticleEmitter::m_GlobalIndicesCount = 0;

// Constructor
ParticleEmitter::ParticleEmitter(CKContext *ctx, CK_ID entity, char *name)
{
    m_Context = ctx;

    g_One[0] = g_One[1] = g_One[2] = g_One[3] = 1.0f;
    m_MaximumParticles = 100;
    m_BackPool = NULL;
    m_InteractorsFlags = 0;

    m_YawVariation = 0;
    m_PitchVariation = 0;
    m_Speed = 0;
    m_SpeedVariation = 0;
    m_AngularSpeed = 0;
    m_AngularSpeedVariation = 0;

    totalParticles = 0;
    particleCount = 0;
    emitsPerFrame = 0;
    emitsVar = 0;
    m_Life = 0;
    m_LifeVariation = 0;
    m_StartSize = 0;
    m_StartSizeVar = 0;
    m_EndSize = 0;
    m_EndSizeVar = 0;
    m_Weight = 0;
    m_WeightVariation = 0;
    m_Surface = 0;
    m_SurfaceVariation = 0;
    m_Bounce = 0;
    m_BounceVariation = 0;
    m_StartColor.r = 0.6f;
    m_StartColor.g = 0.6f;
    m_StartColor.b = 0.8f;
    m_StartColor.b = 1.0f;
    m_StartColorVar.r = 0.0f;
    m_StartColorVar.g = 0.0f;
    m_StartColorVar.b = 0.0f;
    m_StartColorVar.a = 0.0f;
    m_EndColor.r = 0.0f;
    m_EndColor.g = 0.0f;
    m_EndColor.b = 0.0f;
    m_EndColor.a = 0.0f;
    m_EndColorVar.r = 0.0f;
    m_EndColorVar.g = 0.0f;
    m_EndColorVar.b = 0.0f;
    m_EndColorVar.a = 0.0f;
    m_InitialTextureFrame = 0;
    m_InitialTextureFrameVariance = 0;
    m_SpeedTextureFrame = 0;
    m_SpeedTextureFrameVariance = 0;
    m_TextureFrameCount = 0;
    m_TextureFrameloop = FALSE;
    m_EvolutionsFlags = 0;
    m_VariancesFlags = 0;
    m_InteractorsFlags = 0;
    m_DeflectorsFlags = 0;
    m_RenderMode = PR_SPRITE;

    m_Mesh = 0;
    m_Entity = entity;
    m_Texture = 0;
    m_Group = 0;
    m_MessageType = -1;

    m_CurrentImpact = 0;

    m_IsTimePointEmitter = FALSE;
    m_IsWaveEmitter = FALSE;
    m_Flag0x10C = TRUE;

    m_DeltaTime = 0.0f;
    m_Active = FALSE;

    InitParticleSystem();
};

// Destructeur
ParticleEmitter::~ParticleEmitter()
{
    delete[] m_BackPool;
    m_BackPool = NULL;
    m_Entity = 0;
}

void ParticleEmitter::InitParticleSystem()
{
    if (m_BackPool)
    {
        delete[] m_BackPool;
        m_BackPool = NULL;
    }
    m_BackPool = new CKBYTE[m_MaximumParticles * sizeof(Particle) + 16];

    m_Pool = (Particle *)((CKDWORD)(m_BackPool + 15) & 0xFFFFFFF0);
    for (int loop = 0; loop < m_MaximumParticles - 1; loop++)
    {
        m_Pool[loop].next = &m_Pool[loop + 1];
    }
    m_Pool[m_MaximumParticles - 1].next = NULL;
    particles = NULL;
    particleCount = 0;
}

void ParticleEmitter::UpdateParticles(float rdeltat)
{
    ParticleManager *pm = (ParticleManager *)m_Context->GetManagerByGuid(PARTICLE_MANAGER_GUID);

    // We clear the impact array
    m_Impacts.Clear();
    m_CurrentImpact = 0;

    float deltat;
    Particle *particle = particles;
    // If there is no particles yet, we buzz off
    if (!particles)
        return;

    VxVector minv(100000, 100000, 100000), maxv(-100000, -100000, -100000);
    g_Min[0] = g_Min[1] = g_Min[2] = g_Min[3] = 1e6f;
    g_Max[0] = g_Max[1] = g_Max[2] = g_Max[3] = -1e6f;

    ///
    // Interactors management

    // Gravity
    float gravityForce = 0.0f;
    if (m_InteractorsFlags & PI_GRAVITY)
    {
        const XObjectPointerArray &Array = m_Context->GetAttributeManager()->GetAttributeListPtr(pm->m_GravityAttribute);

        for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
        {
            // we get attribute owner
            CKBeObject *ent = (CKBeObject *)*it;

            // we get the attribute value
            CKParameterOut *pout = ent->GetAttributeParameter(pm->m_GravityAttribute);

            float f = 0.0f;
            pout->GetValue(&f);

            gravityForce += f;
        }

        gravityForce *= rdeltat;
    }

    if (m_InteractorsFlags & PI_ATMOSPHERE)
        pm->ManageAtmosphere(this, rdeltat);
    if (m_InteractorsFlags & PI_GLOBALWIND)
        pm->ManageGlobalWind(this, rdeltat);
    if (m_InteractorsFlags & PI_LOCALWIND)
        pm->ManageLocalWind(this, rdeltat);
    // The magnet act on the position, so it came after positioning
    if (m_InteractorsFlags & PI_MAGNET)
        pm->ManageMagnet(this, rdeltat);
    if (m_InteractorsFlags & PI_VORTEX)
        pm->ManageVortex(this, rdeltat);
    if (m_InteractorsFlags & PI_DISRUPTIONBOX)
        pm->ManageDisruptionBox(this, rdeltat);
    if (m_InteractorsFlags & PI_MUTATIONBOX)
        pm->ManageMutationBox(this, rdeltat);
    if (m_InteractorsFlags & PI_TUNNEL)
        pm->ManageTunnel(this, rdeltat);
    if (m_InteractorsFlags & PI_PROJECTOR)
        pm->ManageProjector(this, rdeltat);

    while (particle)
    {
        // IF THIS IS AN VALID PARTICLE
        if (particle->m_Life > 0)
        {
            if (particle->m_Life < rdeltat)
            {
                deltat = particle->m_Life;
            }
            else
            {
                deltat = rdeltat;
            }

            particle->prevpos = particle->pos;

            // Gravity management
            particle->dir.y += gravityForce * particle->m_Weight;

            // Calculating new angle
            particle->m_Angle += particle->m_DeltaAngle * deltat;

            if (particle->pos.x < minv.x)
            {
                minv.x = particle->pos.x;
            }
            if (particle->pos.y < minv.y)
            {
                minv.y = particle->pos.y;
            }
            if (particle->pos.z < minv.z)
            {
                minv.z = particle->pos.z;
            }
            if (maxv.x < particle->pos.x)
            {
                maxv.x = particle->pos.x;
            }
            if (maxv.y < particle->pos.y)
            {
                maxv.y = particle->pos.y;
            }
            if (maxv.z < particle->pos.z)
            {
                maxv.z = particle->pos.z;
            }

            // CALCULATE THE NEW
            particle->pos += (particle->dir * deltat);

            //////////////////////////////
            // BOUNDING BOX UPDATING
            //////////////////////////////

            if (particle->pos.x < minv.x)
            {
                minv.x = particle->pos.x;
            }
            if (particle->pos.y < minv.y)
            {
                minv.y = particle->pos.y;
            }
            if (particle->pos.z < minv.z)
            {
                minv.z = particle->pos.z;
            }
            if (maxv.x < particle->pos.x)
            {
                maxv.x = particle->pos.x;
            }
            if (maxv.y < particle->pos.y)
            {
                maxv.y = particle->pos.y;
            }
            if (maxv.z < particle->pos.z)
            {
                maxv.z = particle->pos.z;
            }

            /////////////////////////////
            // EVOLUTIONS MANAGEMENT
            /////////////////////////////

            // color management
            if (m_EvolutionsFlags & PE_COLOR)
            {
                particle->m_Color.r += particle->deltaColor.r * deltat;
                if (particle->m_Color.r < 0.0f)
                    particle->m_Color.r = 0.0f;
                else if (particle->m_Color.r > 1.0f)
                    particle->m_Color.r = 1.0f;
                particle->m_Color.g += particle->deltaColor.g * deltat;
                if (particle->m_Color.g < 0.0f)
                    particle->m_Color.g = 0.0f;
                else if (particle->m_Color.g > 1.0f)
                    particle->m_Color.g = 1.0f;
                particle->m_Color.b += particle->deltaColor.b * deltat;
                if (particle->m_Color.b < 0.0f)
                    particle->m_Color.b = 0.0f;
                else if (particle->m_Color.b > 1.0f)
                    particle->m_Color.b = 1.0f;
                particle->m_Color.a += particle->deltaColor.a * deltat;
                if (particle->m_Color.a < 0.0f)
                    particle->m_Color.a = 0.0f;
                else if (particle->m_Color.a > 1.0f)
                    particle->m_Color.a = 1.0f;
            }

            // Size management
            if (m_EvolutionsFlags & PE_SIZE)
                particle->m_Size += particle->m_DeltaSize * deltat;

            // texture management
            if (particle->m_DeltaFrametime)
                if (m_EvolutionsFlags & PE_TEXTURE && m_RenderMode != PR_OBJECT)
                {
                    particle->m_CurrentFrametime += deltat;
                    float dft = particle->m_DeltaFrametime;
                    int tfi = 1;
                    if (particle->m_DeltaFrametime < 0)
                    {
                        dft = -particle->m_DeltaFrametime;
                        tfi = -1;
                    }
                    while (particle->m_CurrentFrametime > dft)
                    {
                        particle->m_CurrentFrame += tfi;
                        if (particle->m_CurrentFrame >= m_TextureFrameCount)
                        {
                            switch (m_TextureFrameloop)
                            {
                            case PL_NOLOOP:
                                particle->m_CurrentFrame = m_TextureFrameCount - 1;
                                break;
                            case PL_LOOP:
                                particle->m_CurrentFrame = 0;
                                break;
                            case PL_PINGPONG:
                                particle->m_CurrentFrame = m_TextureFrameCount - 2;
                                particle->m_DeltaFrametime = -particle->m_DeltaFrametime;
                                break;
                            }
                        }
                        else
                        {
                            if (particle->m_CurrentFrame < 0)
                            {
                                switch (m_TextureFrameloop)
                                {
                                case PL_NOLOOP:
                                    particle->m_CurrentFrame = 0;
                                    break;
                                case PL_LOOP:
                                    particle->m_CurrentFrame = m_TextureFrameCount - 1;
                                    break;
                                case PL_PINGPONG:
                                    particle->m_CurrentFrame = 1;
                                    particle->m_DeltaFrametime = -particle->m_DeltaFrametime;
                                    break;
                                }
                            }
                        }
                        particle->m_CurrentFrametime -= dft;
                    }
                }

            // New lifespan
            particle->m_Life -= deltat;
            // new delta time
            particle->m_DeltaTime = deltat;

            particle = particle->next;
        }
        else
        {
            // Delete particle
            Particle *tmp = particle->next;
            if (particle->prev)
                particle->prev->next = particle->next;
            else
                particles = particle->next;
            if (particle->next)
                particle->next->prev = particle->prev;
            particle->next = m_Pool;
            m_Pool = particle; // NEW POOL POINTER
            particleCount--;   // ADD ONE TO POOL
            particle = tmp;
        }
    }

    VxBbox bbox(minv, maxv);
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->SetBoundingBox(&bbox);

    // Deflectors management
    if (m_DeflectorsFlags & PD_PLANE)
        pm->ManagePlaneDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_INFINITEPLANE)
        pm->ManageInfinitePlaneDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_CYLINDER)
        pm->ManageCylinderDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_SPHERE)
        pm->ManageSphereDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_BOX)
        pm->ManageBoxDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_OBJECT)
        pm->ManageObjectDeflectors(this, rdeltat);
}

void ParticleEmitter::UpdateParticles2(float rdeltat)
{
    ParticleManager *pm = (ParticleManager *)m_Context->GetManagerByGuid(PARTICLE_MANAGER_GUID);

    // We clear the impact array
    m_Impacts.Clear();
    m_CurrentImpact = 0;

    float deltat;
    Particle *particle = particles;
    // If there is no particles yet, we buzz off
    if (!particles)
        return;

    VxVector minv(100000, 100000, 100000), maxv(-100000, -100000, -100000);
    g_Min[0] = g_Min[1] = g_Min[2] = g_Min[3] = 1e6f;
    g_Max[0] = g_Max[1] = g_Max[2] = g_Max[3] = -1e6f;

    ///
    // Interactors management

    if (m_InteractorsFlags & PI_GRAVITY)
        pm->ManageGravity(this, rdeltat);
    if (m_InteractorsFlags & PI_ATMOSPHERE)
        pm->ManageAtmosphere(this, rdeltat);
    if (m_InteractorsFlags & PI_GLOBALWIND)
        pm->ManageGlobalWind(this, rdeltat);
    if (m_InteractorsFlags & PI_LOCALWIND)
        pm->ManageLocalWind(this, rdeltat);
    // The magnet act on the position, so it came after positioning
    if (m_InteractorsFlags & PI_MAGNET)
        pm->ManageMagnet(this, rdeltat);
    if (m_InteractorsFlags & PI_VORTEX)
        pm->ManageVortex(this, rdeltat);
    if (m_InteractorsFlags & PI_DISRUPTIONBOX)
        pm->ManageDisruptionBox(this, rdeltat);
    if (m_InteractorsFlags & PI_MUTATIONBOX)
        pm->ManageMutationBox(this, rdeltat);
    if (m_InteractorsFlags & PI_TUNNEL)
        pm->ManageTunnel(this, rdeltat);
    if (m_InteractorsFlags & PI_PROJECTOR)
        pm->ManageProjector(this, rdeltat);

    while (particle)
    {
        // IF THIS IS AN VALID PARTICLE
        if (particle->m_Life > 0)
        {
            if (particle->m_Life < rdeltat)
            {
                deltat = particle->m_Life;
            }
            else
            {
                deltat = rdeltat;
            }

            {
                if (particle->pos.x < minv.x)
                {
                    minv.x = particle->pos.x;
                }
                if (particle->pos.y < minv.y)
                {
                    minv.y = particle->pos.y;
                }
                if (particle->pos.z < minv.z)
                {
                    minv.z = particle->pos.z;
                }
                if (maxv.x < particle->pos.x)
                {
                    maxv.x = particle->pos.x;
                }
                if (maxv.y < particle->pos.y)
                {
                    maxv.y = particle->pos.y;
                }
                if (maxv.z < particle->pos.z)
                {
                    maxv.z = particle->pos.z;
                }

                /////////////////////////////
                // EVOLUTIONS MANAGEMENT
                /////////////////////////////

                // color management
                if (m_EvolutionsFlags & PE_COLOR)
                {
                    particle->m_Color.r += particle->deltaColor.r * deltat;
                    if (particle->m_Color.r < 0.0f)
                        particle->m_Color.r = 0.0f;
                    else if (particle->m_Color.r > 1.0f)
                        particle->m_Color.r = 1.0f;
                    particle->m_Color.g += particle->deltaColor.g * deltat;
                    if (particle->m_Color.g < 0.0f)
                        particle->m_Color.g = 0.0f;
                    else if (particle->m_Color.g > 1.0f)
                        particle->m_Color.g = 1.0f;
                    particle->m_Color.b += particle->deltaColor.b * deltat;
                    if (particle->m_Color.b < 0.0f)
                        particle->m_Color.b = 0.0f;
                    else if (particle->m_Color.b > 1.0f)
                        particle->m_Color.b = 1.0f;
                    particle->m_Color.a += particle->deltaColor.a * deltat;
                    if (particle->m_Color.a < 0.0f)
                        particle->m_Color.a = 0.0f;
                    else if (particle->m_Color.a > 1.0f)
                        particle->m_Color.a = 1.0f;
                }
            }
            // Size management
            if (m_EvolutionsFlags & PE_SIZE)
                particle->m_Size += particle->m_DeltaSize * deltat;

            // texture management
            if (particle->m_DeltaFrametime != 0)
                if (m_EvolutionsFlags & PE_TEXTURE && m_RenderMode != PR_OBJECT)
                {
                    particle->m_CurrentFrametime += deltat;
                    float dft = particle->m_DeltaFrametime;
                    int tfi = 1;
                    if (particle->m_DeltaFrametime < 0)
                    {
                        dft = -particle->m_DeltaFrametime;
                        tfi = -1;
                    }
                    while (particle->m_CurrentFrametime > dft)
                    {
                        particle->m_CurrentFrame += tfi;
                        if (particle->m_CurrentFrame >= m_TextureFrameCount)
                        {
                            switch (m_TextureFrameloop)
                            {
                            case PL_NOLOOP:
                                particle->m_CurrentFrame = m_TextureFrameCount - 1;
                                break;
                            case PL_LOOP:
                                particle->m_CurrentFrame = 0;
                                break;
                            case PL_PINGPONG:
                                particle->m_CurrentFrame = m_TextureFrameCount - 2;
                                particle->m_DeltaFrametime = -particle->m_DeltaFrametime;
                                break;
                            }
                        }
                        else
                        {
                            if (particle->m_CurrentFrame < 0)
                            {
                                switch (m_TextureFrameloop)
                                {
                                case PL_NOLOOP:
                                    particle->m_CurrentFrame = 0;
                                    break;
                                case PL_LOOP:
                                    particle->m_CurrentFrame = m_TextureFrameCount - 1;
                                    break;
                                case PL_PINGPONG:
                                    particle->m_CurrentFrame = 1;
                                    particle->m_DeltaFrametime = -particle->m_DeltaFrametime;
                                    break;
                                }
                            }
                        }
                        particle->m_CurrentFrametime -= dft;
                    }
                }

            // New lifespan
            particle->m_Life -= deltat;
            // new delta time
            particle->m_DeltaTime = deltat;

            particle = particle->next;
        }
        else
        {
            // Delete particle
            Particle *tmp = particle->next;
            if (particle->prev)
                particle->prev->next = particle->next;
            else
                particles = particle->next;
            if (particle->next)
                particle->next->prev = particle->prev;
            particle->next = m_Pool;
            m_Pool = particle; // NEW POOL POINTER
            particleCount--;   // ADD ONE TO POOL
            particle = tmp;
        }
    }

    VxBbox bbox(minv, maxv);
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    entity->SetBoundingBox(&bbox);

    // Deflectors management
    if (m_DeflectorsFlags & PD_PLANE)
        pm->ManagePlaneDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_INFINITEPLANE)
        pm->ManageInfinitePlaneDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_CYLINDER)
        pm->ManageCylinderDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_SPHERE)
        pm->ManageSphereDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_BOX)
        pm->ManageBoxDeflectors(this, rdeltat);
    if (m_DeflectorsFlags & PD_OBJECT)
        pm->ManageObjectDeflectors(this, rdeltat);

    entity->GetPosition(&((WaveEmitter *)this)->m_Position);
}

void ParticleEmitter::AddParticles()
{
    // emits particles for this frame
    int emits;
    if (m_VariancesFlags & PV_EMISSION)
    {
        emits = emitsPerFrame + (int)((float)emitsVar * RANDNUM);
    }
    else
    {
        emits = emitsPerFrame;
    }
    if (emits <= 0)
        return;

    do
    {
        if (m_Pool != NULL && particleCount < totalParticles)
        {
            Particle *p = m_Pool;
            m_Pool = m_Pool->next;

            if (particles != NULL)
                particles->prev = p;
            p->next = particles;
            p->prev = NULL;
            particleCount++;
            particles = p;

            // Calculate the initial Position
            InitiateParticle(p);

            // CALCULATE THE STARTING DIRECTION VECTOR
            InitiateDirection(p);

            // Initiate density
            p->m_Density = rand() * INV_RAND;

            // Calculate the speed
            if (m_VariancesFlags & PV_SPEED)
            {
                p->dir *= m_Speed + (m_SpeedVariation * RANDNUM);
            }
            else
            {
                p->dir *= m_Speed;
            }

            switch (m_RenderMode)
            {
            case PR_LINE:
            case PR_OSPRITE:
            case PR_CSPRITE:
                // we manage here latency
                p->m_Angle = m_AngularSpeed;
                p->m_DeltaAngle = m_AngularSpeedVariation;
                break;
            case PR_SPRITE:
            case PR_FSPRITE:
                p->m_Angle = 0;
                if (m_VariancesFlags & PV_ANGULARSPEED)
                {
                    p->m_DeltaAngle = m_AngularSpeed + m_AngularSpeedVariation * RANDNUM;
                }
                else
                {
                    p->m_DeltaAngle = 0;
                }
                break;
            case PR_OBJECT:
            {
                CKGroup *group = (CKGroup *)m_Context->GetObject(m_Group);
                int oc;
                if (group)
                    oc = group->GetObjectCount();
                else
                    oc = 0;
                p->m_GroupIndex = (int)(oc * ((float)rand() * INV_RAND));
                p->m_Angle = 0;
                if (m_VariancesFlags & PV_ANGULARSPEED)
                {
                    p->m_DeltaAngle = m_AngularSpeed + m_AngularSpeedVariation * RANDNUM;
                }
                else
                {
                    p->m_DeltaAngle = 0;
                }
            }
            break;
            default:
                p->m_Angle = 0;
                p->m_DeltaAngle = 0;
                break;
            }

            ////////////////////////
            // LifeSpan Management
            ////////////////////////
            if (m_VariancesFlags & PV_LIFESPAN)
            {
                p->m_Life = m_Life + m_LifeVariation * RANDNUM;
            }
            else
            {
                p->m_Life = m_Life;
            }
            float invlife = 1.0f / p->m_Life;

            ////////////////////////
            // Color Management
            ////////////////////////

            // Initial Color
            if (m_VariancesFlags & PV_INITIALCOLOR)
            {
                p->m_Color.r = m_StartColor.r + (m_StartColorVar.r * RANDNUM);
                p->m_Color.g = m_StartColor.g + (m_StartColorVar.g * RANDNUM);
                p->m_Color.b = m_StartColor.b + (m_StartColorVar.b * RANDNUM);
                p->m_Color.a = m_StartColor.a + (m_StartColorVar.a * RANDNUM);
            }
            else
            {
                p->m_Color = m_StartColor;
            }
            // Color Delta
            if (m_EvolutionsFlags & PE_COLOR)
            {
                if (m_VariancesFlags & PV_ENDINGCOLOR)
                {
                    p->deltaColor.r = ((m_EndColor.r + (m_EndColorVar.r * RANDNUM)) - p->m_Color.r) * invlife;
                    p->deltaColor.g = ((m_EndColor.g + (m_EndColorVar.g * RANDNUM)) - p->m_Color.g) * invlife;
                    p->deltaColor.b = ((m_EndColor.b + (m_EndColorVar.b * RANDNUM)) - p->m_Color.b) * invlife;
                    p->deltaColor.a = ((m_EndColor.a + (m_EndColorVar.a * RANDNUM)) - p->m_Color.a) * invlife;
                }
                else
                {
                    p->deltaColor.r = (m_EndColor.r - p->m_Color.r) * invlife;
                    p->deltaColor.g = (m_EndColor.g - p->m_Color.g) * invlife;
                    p->deltaColor.b = (m_EndColor.b - p->m_Color.b) * invlife;
                    p->deltaColor.a = (m_EndColor.a - p->m_Color.a) * invlife;
                }
            }
            else
            {
                p->deltaColor.r = 0;
                p->deltaColor.g = 0;
                p->deltaColor.b = 0;
                p->deltaColor.a = 0;
            }

            /////////////////////////
            // Size Management
            /////////////////////////

            // Initial Size
            if (m_VariancesFlags & PV_INITIALSIZE)
            {
                p->m_Size = m_StartSize + (m_StartSizeVar * RANDNUM);
            }
            else
            {
                p->m_Size = m_StartSize;
            }
            // Size Delta
            if (m_EvolutionsFlags & PE_SIZE)
            {
                if (m_VariancesFlags & PV_ENDINGSIZE)
                {
                    p->m_DeltaSize = (m_EndSize + (m_EndSizeVar * RANDNUM) - p->m_Size) * invlife;
                }
                else
                {
                    p->m_DeltaSize = (m_EndSize - p->m_Size) * invlife;
                }
            }
            else
            {
                p->m_DeltaSize = 0;
            }

            /////////////////////////
            // Texture management
            /////////////////////////

            switch (m_RenderMode)
            {
            case PR_SPRITE:
            case PR_FSPRITE:
            case PR_OSPRITE:
            case PR_CSPRITE:
            case PR_RSPRITE:
                // Initial Texture
                if (m_VariancesFlags & PV_INITIALTEXTURE)
                {
                    p->m_CurrentFrame = m_InitialTextureFrame + (int)(m_InitialTextureFrameVariance * RANDNUMP);
                }
                else
                {
                    p->m_CurrentFrame = m_InitialTextureFrame;
                }

                // Initialisation of the current frame time
                p->m_CurrentFrametime = 0;

                // Texture Delta time
                if ((m_EvolutionsFlags & PE_TEXTURE) && (m_TextureFrameCount > 1))
                {
                    if (m_VariancesFlags & PV_SPEEDTEXTURE)
                    {
                        p->m_DeltaFrametime = m_SpeedTextureFrame + m_SpeedTextureFrameVariance * RANDNUM;
                    }
                    else
                    {
                        p->m_DeltaFrametime = (float)m_SpeedTextureFrame;
                    }
                }
                else
                {
                    p->m_DeltaFrametime = 0;
                }
                break;
            default:
                p->m_DeltaFrametime = 0;
                break;
            }

            //////////////////////////
            // Deflectors Management
            //////////////////////////

            // Particle Bouncing factor
            if (m_DeflectorsFlags)
            {
                if (m_VariancesFlags & PV_BOUNCE)
                {
                    p->m_Bounce = m_Bounce + (m_BounceVariation * RANDNUM);
                }
                else
                {
                    p->m_Bounce = m_Bounce;
                }
            }

            //////////////////////////
            // Interactors Management
            //////////////////////////

            // Particle Weight
            if (m_InteractorsFlags & PI_GRAVITY)
            {
                if (m_VariancesFlags & PV_WEIGHT)
                {
                    p->m_Weight = m_Weight + (m_WeightVariation * RANDNUM);
                }
                else
                {
                    p->m_Weight = m_Weight;
                }
            }

            // Time Management
            p->m_DeltaTime = 0.01f;

            // Particle Surface
            if (m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND | PI_ATMOSPHERE))
            {
                if (m_VariancesFlags & PV_SURFACE)
                {
                    p->m_Surface = m_Surface + (m_SurfaceVariation * RANDNUM);
                }
                else
                {
                    p->m_Surface = m_Surface;
                }
            }
        }
        else
            break;
    } while (--emits);
}

void ParticleEmitter::AddParticles2()
{
    CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject(m_Entity);
    if (!ent)
    {
        m_Context->OutputToConsole("No Frame?");
        return;
    }

    if (m_Active)
    {
        TimePointEmitter *self = (TimePointEmitter *)this;
        float time = (self->m_Time + m_DeltaTime) * 0.001f;

        // emits particles for this frame
        float emits;
        if (m_VariancesFlags & PV_EMISSION)
        {
            emits = self->m_EmissionDelay + emitsVar * RANDNUM;
        }
        else
        {
            emits = self->m_EmissionDelay;
        }

        if (emits * time <= 0.9f || m_Flag0x10C)
        {
            self->m_Time += m_DeltaTime;
            if (m_Flag0x10C)
            {
                m_Flag0x10C = 0;
                ent->GetPosition(&self->m_Position);
            }
        }
        else
        {
            emits *= time;

            self->m_Time = 0.0f;

            VxVector pos;
            ent->GetPosition(&pos);

            do
            {
                if (m_Pool != NULL && particleCount < totalParticles)
                {
                    Particle *p = m_Pool;
                    m_Pool = m_Pool->next;

                    if (particles != NULL)
                        particles->prev = p;
                    p->next = particles;
                    p->prev = NULL;
                    particleCount++;
                    particles = p;

                    p->pos = pos;

                    // CALCULATE THE STARTING DIRECTION VECTOR
                    InitiateDirection(p);

                    // Initiate density
                    p->m_Density = rand() * INV_RAND;

                    // Calculate the speed
                    if (m_VariancesFlags & PV_SPEED)
                    {
                        p->dir *= m_Speed + (m_SpeedVariation * RANDNUM);
                    }
                    else
                    {
                        p->dir *= m_Speed;
                    }

                    switch (m_RenderMode)
                    {
                    case PR_LINE:
                    case PR_OSPRITE:
                    case PR_CSPRITE:
                        // we manage here latency
                        p->m_Angle = m_AngularSpeed;
                        p->m_DeltaAngle = m_AngularSpeedVariation;
                        break;
                    case PR_SPRITE:
                    case PR_FSPRITE:
                        p->m_Angle = 0;
                        if (m_VariancesFlags & PV_ANGULARSPEED)
                        {
                            p->m_DeltaAngle = m_AngularSpeed + m_AngularSpeedVariation * RANDNUM;
                        }
                        else
                        {
                            p->m_DeltaAngle = 0;
                        }
                        break;
                    case PR_OBJECT:
                    {
                        CKGroup *group = (CKGroup *)m_Context->GetObject(m_Group);
                        int oc;
                        if (group)
                            oc = group->GetObjectCount();
                        else
                            oc = 0;
                        p->m_GroupIndex = (int)(oc * ((float)rand() * INV_RAND));
                        p->m_Angle = 0;
                        if (m_VariancesFlags & PV_ANGULARSPEED)
                        {
                            p->m_DeltaAngle = m_AngularSpeed + m_AngularSpeedVariation * RANDNUM;
                        }
                        else
                        {
                            p->m_DeltaAngle = 0;
                        }
                    }
                    break;
                    default:
                        p->m_Angle = 0;
                        p->m_DeltaAngle = 0;
                        break;
                    }

                    ////////////////////////
                    // LifeSpan Management
                    ////////////////////////
                    if (m_VariancesFlags & PV_LIFESPAN)
                    {
                        p->m_Life = m_Life + m_LifeVariation * RANDNUM;
                    }
                    else
                    {
                        p->m_Life = m_Life;
                    }
                    float invlife = 1.0f / p->m_Life;

                    ////////////////////////
                    // Color Management
                    ////////////////////////

                    // Initial Color
                    if (m_VariancesFlags & PV_INITIALCOLOR)
                    {
                        p->m_Color.r = m_StartColor.r + (m_StartColorVar.r * RANDNUM);
                        p->m_Color.g = m_StartColor.g + (m_StartColorVar.g * RANDNUM);
                        p->m_Color.b = m_StartColor.b + (m_StartColorVar.b * RANDNUM);
                        p->m_Color.a = m_StartColor.a + (m_StartColorVar.a * RANDNUM);
                    }
                    else
                    {
                        p->m_Color = m_StartColor;
                    }
                    // Color Delta
                    if (m_EvolutionsFlags & PE_COLOR)
                    {
                        if (m_VariancesFlags & PV_ENDINGCOLOR)
                        {
                            p->deltaColor.r = ((m_EndColor.r + (m_EndColorVar.r * RANDNUM)) - p->m_Color.r) * invlife;
                            p->deltaColor.g = ((m_EndColor.g + (m_EndColorVar.g * RANDNUM)) - p->m_Color.g) * invlife;
                            p->deltaColor.b = ((m_EndColor.b + (m_EndColorVar.b * RANDNUM)) - p->m_Color.b) * invlife;
                            p->deltaColor.a = ((m_EndColor.a + (m_EndColorVar.a * RANDNUM)) - p->m_Color.a) * invlife;
                        }
                        else
                        {
                            p->deltaColor.r = (m_EndColor.r - p->m_Color.r) * invlife;
                            p->deltaColor.g = (m_EndColor.g - p->m_Color.g) * invlife;
                            p->deltaColor.b = (m_EndColor.b - p->m_Color.b) * invlife;
                            p->deltaColor.a = (m_EndColor.a - p->m_Color.a) * invlife;
                        }
                    }
                    else
                    {
                        p->deltaColor.r = 0;
                        p->deltaColor.g = 0;
                        p->deltaColor.b = 0;
                        p->deltaColor.a = 0;
                    }

                    /////////////////////////
                    // Size Management
                    /////////////////////////

                    // Initial Size
                    if (m_VariancesFlags & PV_INITIALSIZE)
                    {
                        p->m_Size = m_StartSize + (m_StartSizeVar * RANDNUM);
                    }
                    else
                    {
                        p->m_Size = m_StartSize;
                    }
                    // Size Delta
                    if (m_EvolutionsFlags & PE_SIZE)
                    {
                        if (m_VariancesFlags & PV_ENDINGSIZE)
                        {
                            p->m_DeltaSize = (m_EndSize + (m_EndSizeVar * RANDNUM) - p->m_Size) * invlife;
                        }
                        else
                        {
                            p->m_DeltaSize = (m_EndSize - p->m_Size) * invlife;
                        }
                    }
                    else
                    {
                        p->m_DeltaSize = 0;
                    }

                    /////////////////////////
                    // Texture management
                    /////////////////////////

                    switch (m_RenderMode)
                    {
                    case PR_SPRITE:
                    case PR_FSPRITE:
                    case PR_OSPRITE:
                    case PR_CSPRITE:
                    case PR_RSPRITE:
                        // Initial Texture
                        if (m_VariancesFlags & PV_INITIALTEXTURE)
                        {
                            p->m_CurrentFrame = m_InitialTextureFrame + (int)(m_InitialTextureFrameVariance * RANDNUMP);
                        }
                        else
                        {
                            p->m_CurrentFrame = m_InitialTextureFrame;
                        }

                        // Initialisation of the current frame time
                        p->m_CurrentFrametime = 0;

                        // Texture Delta time
                        if ((m_EvolutionsFlags & PE_TEXTURE) && (m_TextureFrameCount > 1))
                        {
                            if (m_VariancesFlags & PV_SPEEDTEXTURE)
                            {
                                p->m_DeltaFrametime = m_SpeedTextureFrame + m_SpeedTextureFrameVariance * RANDNUM;
                            }
                            else
                            {
                                p->m_DeltaFrametime = (float)m_SpeedTextureFrame;
                            }
                        }
                        else
                        {
                            p->m_DeltaFrametime = 0;
                        }
                        break;
                    default:
                        p->m_DeltaFrametime = 0;
                        break;
                    }

                    //////////////////////////
                    // Deflectors Management
                    //////////////////////////

                    // Particle Bouncing factor
                    if (m_DeflectorsFlags)
                    {
                        if (m_VariancesFlags & PV_BOUNCE)
                        {
                            p->m_Bounce = m_Bounce + (m_BounceVariation * RANDNUM);
                        }
                        else
                        {
                            p->m_Bounce = m_Bounce;
                        }
                    }

                    //////////////////////////
                    // Interactors Management
                    //////////////////////////

                    // Particle Weight
                    if (m_InteractorsFlags & PI_GRAVITY)
                    {
                        if (m_VariancesFlags & PV_WEIGHT)
                        {
                            p->m_Weight = m_Weight + (m_WeightVariation * RANDNUM);
                        }
                        else
                        {
                            p->m_Weight = m_Weight;
                        }
                    }

                    // Time Management
                    p->m_DeltaTime = 0.01f;

                    // Particle Surface
                    if (m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND | PI_ATMOSPHERE))
                    {
                        if (m_VariancesFlags & PV_SURFACE)
                        {
                            p->m_Surface = m_Surface + (m_SurfaceVariation * RANDNUM);
                        }
                        else
                        {
                            p->m_Surface = m_Surface;
                        }
                    }
                }
                else
                    break;
            } while (--emits > 0);

            self->m_Position = pos;
        }
    }
}


void ParticleEmitter::AddParticles3()
{
    // emits particles for this frame
    int emits;
    if (m_VariancesFlags & PV_EMISSION)
    {
        emits = emitsPerFrame + (int)((float)emitsVar * RANDNUM);
    }
    else
    {
        emits = emitsPerFrame;
    }
    if (emits <= 0)
        return;

    WaveEmitter *self = (WaveEmitter *)this;

    CK3dEntity *ent = (CK3dEntity *)m_Context->GetObject(m_Entity);
    if (!ent)
        return;

    VxVector pos;
    ent->GetPosition(&pos);

    do
    {
        if (m_Pool != NULL && particleCount < totalParticles)
        {
            Particle *p = m_Pool;
            m_Pool = m_Pool->next;

            if (particles != NULL)
                particles->prev = p;
            p->next = particles;
            p->prev = NULL;
            particleCount++;
            particles = p;

            p->pos = pos;

            p->dir = VxVector(0, 0, 0);

            VxVector dir;
            ent->GetOrientation(&dir, NULL);

            VxVector pp = pos - self->m_Position;
            if (pp == VxVector(0.0f))
            {
                p->m_Angle = self->m_Angle;
            }
            else
            {
                VxVector np = Normalize(pp);
                p->m_Angle = acosf(DotProduct(np, dir));
                if (np.x < 0.0f && p->m_Angle > 0.0f)
                    p->m_Angle = -p->m_Angle;
                self->m_Angle = p->m_Angle;
            }

            p->m_DeltaAngle = 0.0f;

            ////////////////////////
            // LifeSpan Management
            ////////////////////////
            if (m_VariancesFlags & PV_LIFESPAN)
            {
                p->m_Life = m_Life + m_LifeVariation * RANDNUM;
            }
            else
            {
                p->m_Life = m_Life;
            }
            float invlife = 1.0f / p->m_Life;

            ////////////////////////
            // Color Management
            ////////////////////////

            // Initial Color
            if (m_VariancesFlags & PV_INITIALCOLOR)
            {
                p->m_Color.r = m_StartColor.r + (m_StartColorVar.r * RANDNUM);
                p->m_Color.g = m_StartColor.g + (m_StartColorVar.g * RANDNUM);
                p->m_Color.b = m_StartColor.b + (m_StartColorVar.b * RANDNUM);
                p->m_Color.a = m_StartColor.a + (m_StartColorVar.a * RANDNUM);
            }
            else
            {
                p->m_Color = m_StartColor;
            }
            // Color Delta
            if (m_EvolutionsFlags & PE_COLOR)
            {
                if (m_VariancesFlags & PV_ENDINGCOLOR)
                {
                    p->deltaColor.r = ((m_EndColor.r + (m_EndColorVar.r * RANDNUM)) - p->m_Color.r) * invlife;
                    p->deltaColor.g = ((m_EndColor.g + (m_EndColorVar.g * RANDNUM)) - p->m_Color.g) * invlife;
                    p->deltaColor.b = ((m_EndColor.b + (m_EndColorVar.b * RANDNUM)) - p->m_Color.b) * invlife;
                    p->deltaColor.a = ((m_EndColor.a + (m_EndColorVar.a * RANDNUM)) - p->m_Color.a) * invlife;
                }
                else
                {
                    p->deltaColor.r = (m_EndColor.r - p->m_Color.r) * invlife;
                    p->deltaColor.g = (m_EndColor.g - p->m_Color.g) * invlife;
                    p->deltaColor.b = (m_EndColor.b - p->m_Color.b) * invlife;
                    p->deltaColor.a = (m_EndColor.a - p->m_Color.a) * invlife;
                }
            }
            else
            {
                p->deltaColor.r = 0;
                p->deltaColor.g = 0;
                p->deltaColor.b = 0;
                p->deltaColor.a = 0;
            }

            /////////////////////////
            // Size Management
            /////////////////////////

            // Initial Size
            if (m_VariancesFlags & PV_INITIALSIZE)
            {
                p->m_Size = m_StartSize + (m_StartSizeVar * RANDNUM);
            }
            else
            {
                p->m_Size = m_StartSize;
            }
            // Size Delta
            if (m_EvolutionsFlags & PE_SIZE)
            {
                if (m_VariancesFlags & PV_ENDINGSIZE)
                {
                    p->m_DeltaSize = (m_EndSize + (m_EndSizeVar * RANDNUM) - p->m_Size) * invlife;
                }
                else
                {
                    p->m_DeltaSize = (m_EndSize - p->m_Size) * invlife;
                }
            }
            else
            {
                p->m_DeltaSize = 0;
            }

            /////////////////////////
            // Texture management
            /////////////////////////

            switch (m_RenderMode)
            {
            case PR_SPRITE:
            case PR_FSPRITE:
            case PR_OSPRITE:
            case PR_CSPRITE:
            case PR_RSPRITE:
                // Initial Texture
                if (m_VariancesFlags & PV_INITIALTEXTURE)
                {
                    p->m_CurrentFrame = m_InitialTextureFrame + (int)(m_InitialTextureFrameVariance * RANDNUMP);
                }
                else
                {
                    p->m_CurrentFrame = m_InitialTextureFrame;
                }

                // Initialisation of the current frame time
                p->m_CurrentFrametime = 0;

                // Texture Delta time
                if ((m_EvolutionsFlags & PE_TEXTURE) && (m_TextureFrameCount > 1))
                {
                    if (m_VariancesFlags & PV_SPEEDTEXTURE)
                    {
                        p->m_DeltaFrametime = m_SpeedTextureFrame + m_SpeedTextureFrameVariance * RANDNUM;
                    }
                    else
                    {
                        p->m_DeltaFrametime = (float)m_SpeedTextureFrame;
                    }
                }
                else
                {
                    p->m_DeltaFrametime = 0;
                }
                break;
            default:
                p->m_DeltaFrametime = 0;
                break;
            }

            //////////////////////////
            // Deflectors Management
            //////////////////////////

            // Particle Bouncing factor
            if (m_DeflectorsFlags)
            {
                if (m_VariancesFlags & PV_BOUNCE)
                {
                    p->m_Bounce = m_Bounce + (m_BounceVariation * RANDNUM);
                }
                else
                {
                    p->m_Bounce = m_Bounce;
                }
            }

            //////////////////////////
            // Interactors Management
            //////////////////////////

            // Particle Weight
            if (m_InteractorsFlags & PI_GRAVITY)
            {
                if (m_VariancesFlags & PV_WEIGHT)
                {
                    p->m_Weight = m_Weight + (m_WeightVariation * RANDNUM);
                }
                else
                {
                    p->m_Weight = m_Weight;
                }
            }

            // Time Management
            p->m_DeltaTime = 0.01f;

            // Particle Surface
            if (m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND | PI_ATMOSPHERE))
            {
                if (m_VariancesFlags & PV_SURFACE)
                {
                    p->m_Surface = m_Surface + (m_SurfaceVariation * RANDNUM);
                }
                else
                {
                    p->m_Surface = m_Surface;
                }
            }
        }
        else
            break;
    } while (--emits);
}

void ParticleEmitter::InitiateDirection(Particle *p)
{
    VxVector dir;
    CK3dEntity *entity = (CK3dEntity *)m_Context->GetObject(m_Entity);
    dir.z = cosf(m_PitchVariation * RANDNUM);
    dir.x = dir.z * sinf(m_YawVariation * RANDNUM);
    dir.z *= cosf(m_YawVariation * RANDNUM);
    dir.y = sinf(m_PitchVariation * RANDNUM);
    entity->TransformVector(&(p->dir), &dir);
}

void ParticleEmitter::ReadInputs(CKBehavior *beh)
{
    beh->GetInputParameterValue(YAWVARIATION, &m_YawVariation);
    beh->GetInputParameterValue(PITCHVARIATION, &m_PitchVariation);
    beh->GetInputParameterValue(SPEED, &m_Speed);
    if (m_VariancesFlags & PV_SPEED)
        beh->GetInputParameterValue(SPEEDVARIATION, &m_SpeedVariation);
    beh->GetInputParameterValue(ANGSPEED, &m_AngularSpeed);
    if (m_VariancesFlags & PV_ANGULARSPEED)
        beh->GetInputParameterValue(ANGSPEEDVARIATION, &m_AngularSpeedVariation);

    ////////
    // Size
    ////////
    // Initial Size
    beh->GetInputParameterValue(STARTSIZE, &m_StartSize);
    if (m_VariancesFlags & PV_INITIALSIZE)
    {
        beh->GetInputParameterValue(STARTSIZEVARIANCE, &m_StartSizeVar);
    }
    // End Size
    if (m_EvolutionsFlags & PE_SIZE)
    {
        beh->GetInputParameterValue(ENDSIZE, &m_EndSize);
        if (m_VariancesFlags & PV_ENDINGSIZE)
        {
            beh->GetInputParameterValue(ENDSIZEVARIANCE, &m_EndSizeVar);
        }
    }

    beh->GetInputParameterValue(LIFE, &m_Life);
    if (m_VariancesFlags & PV_LIFESPAN)
        beh->GetInputParameterValue(LIFEVARIATION, &m_LifeVariation);

    beh->GetInputParameterValue(NUMBER, &totalParticles);

    beh->GetInputParameterValue(EMITION, &emitsPerFrame);
    if (m_VariancesFlags & PV_EMISSION)
        beh->GetInputParameterValue(EMITIONVARIATION, &emitsVar);

    ///////////
    // Colors
    ///////////
    // Initial Color
    beh->GetInputParameterValue(STARTCOLOR, &m_StartColor);
    if (m_VariancesFlags & PV_INITIALCOLOR)
    {
        beh->GetInputParameterValue(STARTCOLORVARIANCE, &m_StartColorVar);
    }
    // Ending Color
    if (m_EvolutionsFlags & PE_COLOR)
    {
        beh->GetInputParameterValue(ENDCOLOR, &m_EndColor);
        if (m_VariancesFlags & PV_ENDINGCOLOR)
        {
            beh->GetInputParameterValue(ENDCOLORVARIANCE, &m_EndColorVar);
        }
    }

    ///////////
    // Textures
    ///////////

    // We get the texture
    beh->GetInputParameterValue(TEXTURE, &m_Texture);

    // Initial Texture
    beh->GetInputParameterValue(STARTTEXTURE, &m_InitialTextureFrame);
    if (m_VariancesFlags & PV_INITIALTEXTURE)
    {
        beh->GetInputParameterValue(STARTTEXTUREVARIANCE, &m_InitialTextureFrameVariance);
    }

    // Speed Texture
    if (m_EvolutionsFlags & PE_TEXTURE)
    {
        beh->GetInputParameterValue(SPEEDTEXTURE, &m_SpeedTextureFrame);
        if (m_VariancesFlags & PV_SPEEDTEXTURE)
        {
            beh->GetInputParameterValue(SPEEDTEXTUREVARIANCE, &m_SpeedTextureFrameVariance);
        }
    }

    // Texture Count
    beh->GetInputParameterValue(TEXTURECOUNT, &m_TextureFrameCount);

    // Texture Loop
    beh->GetInputParameterValue(TEXTURELOOP, &m_TextureFrameloop);

    // Deflectors parameter
    if (m_DeflectorsFlags)
    {
        beh->GetInputParameterValue(BOUNCE, &m_Bounce);
        if (m_VariancesFlags & PV_BOUNCE)
            beh->GetInputParameterValue(BOUNCEVARIATION, &m_BounceVariation);
    }

    // Gravity parameters
    if (m_InteractorsFlags & PI_GRAVITY)
    {
        beh->GetInputParameterValue(WEIGHT, &m_Weight);
        if (m_VariancesFlags & PV_WEIGHT)
            beh->GetInputParameterValue(WEIGHTVARIATION, &m_WeightVariation);
    }

    // Wind parameters
    if (m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND))
    {
        beh->GetInputParameterValue(SURFACE, &m_Surface);
        if (m_VariancesFlags & PV_SURFACE)
            beh->GetInputParameterValue(SURFACEVARIATION, &m_SurfaceVariation);
    }
}

void ParticleEmitter::ReadSettings(CKBehavior *beh)
{
    // Patch to see if it's not an old behavior
    CKParameterLocal *pl = beh->GetLocalParameter(MESSAGE);
    if (pl && pl->GetGUID() != CKPGUID_MESSAGE)
    {
        char buffer[256];
        sprintf(buffer, "Wrong Particle System Version. Please Delete and Reattach : %s on object %s", beh->GetName(), beh->GetOwner()->GetName());
        m_Context->OutputToConsole(buffer);
        return;
    }

    ///////////////////////////
    // Memory Management
    ///////////////////////////
    int pn;
    beh->GetLocalParameterValue(PARTICLENUMBER, &pn);
    if (pn != m_MaximumParticles && pn > 0)
    {
        m_MaximumParticles = pn;
        InitParticleSystem();
    }

    ///////////////////////////
    // Interactors Display
    ///////////////////////////
    ParticleManager *pm = (ParticleManager *)m_Context->GetManagerByGuid(PARTICLE_MANAGER_GUID);

    CKBOOL display;
    beh->GetLocalParameterValue(DISPLAYINTERACTORS, &display);
    pm->ShowInteractors(display);

    ///////////////////////////
    // Render Mode Management
    ///////////////////////////
    beh->GetLocalParameterValue(RENDERMODES, &m_RenderMode);
    if (m_RenderMode == PR_POINT)
    {
        m_RenderParticlesCallback = RenderParticles_P;
    }
    else if (m_RenderMode == PR_LINE)
    {
        m_RenderParticlesCallback = RenderParticles_L;
    }
    else if (m_RenderMode == PR_SPRITE)
    {
        m_RenderParticlesCallback = RenderParticles_S;
    }
    else if (m_RenderMode == PR_OBJECT)
    {
        m_RenderParticlesCallback = RenderParticles_O;
    }
    else if (m_RenderMode == PR_FSPRITE)
    {
        m_RenderParticlesCallback = RenderParticles_FS;
    }
    else if (m_RenderMode == PR_OSPRITE)
    {
        m_RenderParticlesCallback = RenderParticles_OS;
    }
    else if (m_RenderMode == PR_CSPRITE)
    {
        m_RenderParticlesCallback = RenderParticles_CS;
    }
    else if (m_RenderMode == PR_RSPRITE)
    {
        m_RenderParticlesCallback = RenderParticles_RS;
    }
    else
    {
        m_RenderParticlesCallback = NULL;
    }

    /*
    CK3dEntity *entity = (CK3dEntity *)CKGetObject(m_Entity);
    entity->SetRenderCallBack(m_RenderParticlesCallback, this);
    */

    //////////////////////////////
    // Blend Factors management
    //////////////////////////////
    beh->GetLocalParameterValue(SRCBLEND, &m_SrcBlend);
    beh->GetLocalParameterValue(DESTBLEND, &m_DestBlend);

    ///////////////////////////
    // Flags Management
    ///////////////////////////

    // we clear the flags
    m_EvolutionsFlags = 0;
    m_VariancesFlags = 0;
    m_DeflectorsFlags = 0;
    m_InteractorsFlags = 0;

    //////////////////////////////
    // Evolution Management
    //////////////////////////////

    beh->GetLocalParameterValue(EVOLUTIONS, &m_EvolutionsFlags);

    // management of the input parameters
    beh->EnableInputParameter(ENDSIZE, m_EvolutionsFlags & PE_SIZE);
    beh->EnableInputParameter(ENDCOLOR, m_EvolutionsFlags & PE_COLOR);
    beh->EnableInputParameter(SPEEDTEXTURE, m_EvolutionsFlags & PE_TEXTURE);
    beh->EnableInputParameter(TEXTURELOOP, m_EvolutionsFlags & PE_TEXTURE);

    //////////////////////////////
    // Variances Management
    //////////////////////////////

    beh->GetLocalParameterValue(VARIANCES, &m_VariancesFlags);

    // management of the input parameters
    beh->EnableInputParameter(SPEEDVARIATION, m_VariancesFlags & PV_SPEED);
    beh->EnableInputParameter(ANGSPEEDVARIATION, m_VariancesFlags & PV_ANGULARSPEED);
    beh->EnableInputParameter(LIFEVARIATION, m_VariancesFlags & PV_LIFESPAN);
    beh->EnableInputParameter(EMITIONVARIATION, m_VariancesFlags & PV_EMISSION);
    beh->EnableInputParameter(STARTSIZEVARIANCE, m_VariancesFlags & PV_INITIALSIZE);
    beh->EnableInputParameter(BOUNCEVARIATION, m_VariancesFlags & PV_BOUNCE);
    beh->EnableInputParameter(WEIGHTVARIATION, m_VariancesFlags & PV_WEIGHT);
    beh->EnableInputParameter(SURFACEVARIATION, m_VariancesFlags & PV_SURFACE);
    beh->EnableInputParameter(STARTCOLORVARIANCE, m_VariancesFlags & PV_INITIALCOLOR);
    beh->EnableInputParameter(STARTTEXTUREVARIANCE, m_VariancesFlags & PV_INITIALTEXTURE);

    ////////////////////////////////
    // Combined Parameters
    ////////////////////////////////

    beh->EnableInputParameter(ENDCOLORVARIANCE, m_EvolutionsFlags & PE_COLOR && m_VariancesFlags & PV_ENDINGCOLOR);
    beh->EnableInputParameter(ENDSIZEVARIANCE, m_EvolutionsFlags & PE_SIZE && m_VariancesFlags & PV_ENDINGSIZE);
    beh->EnableInputParameter(SPEEDTEXTUREVARIANCE, m_EvolutionsFlags & PE_TEXTURE && m_VariancesFlags & PV_SPEEDTEXTURE);

    // DEFLECTORS
    //-----------
    beh->GetLocalParameterValue(MANAGEDEFLECTORS, &m_DeflectorsFlags);
    // Bounce Parameter Only if Deflector
    beh->EnableInputParameter(BOUNCE, m_DeflectorsFlags & PD_ALL);
    beh->EnableInputParameter(BOUNCEVARIATION, m_DeflectorsFlags & PD_ALL && m_VariancesFlags & PV_BOUNCE);

    // Collision Outputs

    if (m_DeflectorsFlags & PD_IMPACTS) // we have to create outputs param and ios
    {
        if (beh->GetInputCount() == 3)
        {
            // first we destroy all the outputs
            while (beh->GetOutputParameterCount())
            {
                CKDestroyObject(beh->RemoveOutputParameter(0));
            }

            beh->CreateInput("Impacts Loop In");
            beh->CreateOutput("Impacts Loop Out");
            beh->CreateOutputParameter("Impact Position", CKPGUID_VECTOR);
            beh->CreateOutputParameter("Impact Direction", CKPGUID_VECTOR);
            beh->CreateOutputParameter("Impact Object", CKPGUID_3DENTITY);
            beh->CreateOutputParameter("Impact Texture Coordinates", CKPGUID_2DVECTOR);
        }
    }
    else // we have to delete outputs param and ios
    {
        beh->DeleteInput(3);
        beh->DeleteOutput(3);

        CKDestroyObject(beh->RemoveOutputParameter(3));
        CKDestroyObject(beh->RemoveOutputParameter(2));
        CKDestroyObject(beh->RemoveOutputParameter(1));
        CKDestroyObject(beh->RemoveOutputParameter(0));
    }

    // INTERACTORS
    //------------

    beh->GetLocalParameterValue(MANAGEINTERACTORS, &m_InteractorsFlags);
    // Weight Parameter Only if Gravity
    beh->EnableInputParameter(WEIGHT, m_InteractorsFlags & PI_GRAVITY);
    beh->EnableInputParameter(WEIGHTVARIATION, m_InteractorsFlags & PI_GRAVITY && m_VariancesFlags & PV_WEIGHT);
    // Surface Parameter Only if Wind
    beh->EnableInputParameter(SURFACE, m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND | PI_ATMOSPHERE));
    beh->EnableInputParameter(SURFACEVARIATION, m_InteractorsFlags & (PI_LOCALWIND | PI_GLOBALWIND | PI_ATMOSPHERE) && m_VariancesFlags & PV_SURFACE);

    // Group of object to throw
    beh->GetLocalParameterValue(OBJECTS, &m_Group);

    // Message to deflectors...
    beh->GetLocalParameterValue(MESSAGE, &m_MessageType);
}

void ParticleEmitter::SetState(CKRenderContext *dev, CKBOOL gouraud)
{
    // The Texture
    CKTexture *tex = (CKTexture *)dev->GetCKContext()->GetObject(m_Texture);
    dev->SetTexture(tex);

    dev->SetState(VXRENDERSTATE_SPECULARENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_FILLMODE, VXFILL_SOLID);
    // Goraud / Flat
    if (gouraud)
        dev->SetState(VXRENDERSTATE_SHADEMODE, VXSHADE_GOURAUD);
    else
        dev->SetState(VXRENDERSTATE_SHADEMODE, VXSHADE_FLAT);

    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATEALPHA);
    dev->SetTextureStageState(CKRST_TSS_MAGFILTER, VXTEXTUREFILTER_LINEAR);
    dev->SetTextureStageState(CKRST_TSS_MINFILTER, VXTEXTUREFILTER_LINEARMIPLINEAR);

    // States
    dev->SetState(VXRENDERSTATE_WRAP0, 0);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, m_SrcBlend);
    dev->SetState(VXRENDERSTATE_DESTBLEND, m_DestBlend);
    if (m_DestBlend == VXBLEND_ZERO && ((m_SrcBlend == VXBLEND_SRCALPHA) || (m_SrcBlend == VXBLEND_SRCCOLOR) || (m_SrcBlend == VXBLEND_ONE)))
    {
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);
    }
    else
    {
        dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
        dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    }
    dev->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);
    dev->SetTextureStageState(CKRST_TSS_TEXTURETRANSFORMFLAGS, 0);
    dev->SetTextureStageState(CKRST_TSS_TEXCOORDINDEX, 0);
}