#include "CKAll.h"

#include "Particle.h"
#include "ParticleGuids.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"

char *ParticleManager::Name = "Particle Manager";

// This is the constructor of a class that has been exported.
// see ParticleManager.h for the class definition
ParticleManager::ParticleManager(CKContext *context) : CKBaseManager(context, PARTICLE_MANAGER_GUID, ParticleManager::Name)
{
    ParticleEmitter::m_GlobalIndices = NULL;
    ParticleEmitter::m_GlobalIndicesCount = 0;

    InitMeshes();
    m_ShowInteractors = TRUE;
    context->RegisterNewManager(this);
}

void ParticleManager::ShowInteractors(CKBOOL on)
{
    CK_ID *ids = m_Context->GetObjectsListByClassID(CKCID_BEHAVIOR);
    int count = m_Context->GetObjectsCountByClassID(CKCID_BEHAVIOR);

    m_ShowInteractors = on;

    for (int i = 0; i < count; ++i)
    {
        CKBehavior *beh = (CKBehavior *)CKGetObject(ids[i]);
        // it's a building block
        if (beh->GetType() == CKBEHAVIORTYPE_BASE)
        {
            CKGUID guid = beh->GetPrototypeGuid();
            // it's a particle system block
            if ((guid == POINTSYSTEM_GUID) ||
                (guid == PLANARSYSTEM_GUID) ||
                (guid == CUBICSYSTEM_GUID) ||
                (guid == LINEARSYSTEM_GUID) ||
                (guid == DISCSYSTEM_GUID) ||
                (guid == OBJECTSYSTEM_GUID) ||
                (guid == CURVESYSTEM_GUID) ||
                (guid == CYLINDRICALSYSTEM_GUID) ||
                (guid == SPHERICALSYSTEM_GUID) ||
                (guid == TIMEPOINTSYSTEM_GUID) ||
                (guid == WAVESYSTEM_GUID))
            {
                beh->SetLocalParameterValue(DISPLAYINTERACTORS, &m_ShowInteractors);
            }
        }
    }
}

void ParticleAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);

void ParticleManager::InitAttributes()
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();
    int att;

    // first we create the attribute category "Particle Systems"
    attman->AddCategory(ParticleSystemsInteractorsName);

    ///////////////////////////////////
    // INTERACTORS ATTRIBUTES
    ///////////////////////////////////

    // Gravity
    att = attman->RegisterNewAttributeType(ParticleGravityName, CKPGUID_FLOAT);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "-0.0001");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_GravityAttribute = att;

    // Global Wind
    att = attman->RegisterNewAttributeType(ParticleGlobalWindName, CKPGUID_FLOAT, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "0.001");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_GlobalWindAttribute = att;

    // Local Wind
    att = attman->RegisterNewAttributeType(ParticleLocalWindName, CKPGUID_2DVECTOR, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "0.001,0");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_LocalWindAttribute = att;

    // Magnet
    att = attman->RegisterNewAttributeType(ParticleMagnetName, CKPGUID_FLOAT, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0");
    m_MagnetAttribute = att;

    // Vortex
    att = attman->RegisterNewAttributeType(ParticleVortexName, CKPGUID_VECTOR, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0,1.0,1.0");
    m_VortexAttribute = att;

    // Disruption Box
    att = attman->RegisterNewAttributeType(ParticleDisruptionBoxName, CKPGUID_VECTOR, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "0.1,0.1,0.1");
    m_DisruptionBoxAttribute = att;

    // Mutation Box
    att = attman->RegisterNewAttributeType(ParticleMutationBoxName, CKPGUID_PARTICLEMUTATION, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;0.01;255,255,255,255;0.01");
    m_MutationBoxAttribute = att;

    // Atmosphere
    att = attman->RegisterNewAttributeType(ParticleAtmosphereName, CKPGUID_FLOAT);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "0.003");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_AtmosphereAttribute = att;

    // Tunnel
    att = attman->RegisterNewAttributeType(ParticleTunnelName, CKPGUID_PARTICLETUNNEL, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "0.05;200;0.1;0.5;10");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_TunnelAttribute = att;

    // Projector
    att = attman->RegisterNewAttributeType(ParticleProjectorName, CKPGUID_VECTOR, CKCID_SPRITE3D);
    attman->SetAttributeCategory(att, ParticleSystemsInteractorsName);
    attman->SetAttributeDefaultValue(att, "1,1,10");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_ProjectorAttribute = att;

    ///////////////////////////////////
    // DEFLECTORS ATTRIBUTES
    ///////////////////////////////////

    // first we create the attribute category "Particle Systems"
    attman->AddCategory(ParticleSystemsDeflectorsName);

    // Plane
    att = attman->RegisterNewAttributeType(ParticleDPlaneName, CKPGUID_PDEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100");
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    m_DPlaneAttribute = att;

    // Infinite Plane
    att = attman->RegisterNewAttributeType(ParticleDInfinitePlaneName, CKPGUID_PDEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100");
    m_DInfinitePlaneAttribute = att;

    // Sphere
    att = attman->RegisterNewAttributeType(ParticleDSphereName, CKPGUID_PDEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100");
    m_DSphereAttribute = att;

    /* Not working yet : so commented */
    // Cylinder
    att = attman->RegisterNewAttributeType(ParticleDCylinderName, CKPGUID_PDEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100");
    m_DCylinderAttribute = att;

    // Box
    att = attman->RegisterNewAttributeType(ParticleDBoxName, CKPGUID_PDEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100");
    m_DBoxAttribute = att;

    // Object
    att = attman->RegisterNewAttributeType(ParticleDObjectName, CKPGUID_PODEFLECTORS, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, ParticleSystemsDeflectorsName);
    attman->SetAttributeCallbackFunction(att, ParticleAttributeCallback, this);
    attman->SetAttributeDefaultValue(att, "1.0;1.0;100;FALSE");
    m_DObjectAttribute = att;
}

void ParticleManager::DestroyMeshes()
{
    // Particles Systems Meshes
    CKDestroyObject(m_PointMesh);
    CKDestroyObject(m_LinearMesh);
    CKDestroyObject(m_PlanarMesh);
    CKDestroyObject(m_CubicMesh);
    CKDestroyObject(m_DiscMesh);
    CKDestroyObject(m_CylindricalMesh);
    CKDestroyObject(m_SphericalMesh);

    // Interactor Meshes
    CKDestroyObject(m_GlobalWindMesh);
    CKDestroyObject(m_LocalWindMesh);
    CKDestroyObject(m_MagnetMesh);
    CKDestroyObject(m_VortexMesh);
    CKDestroyObject(m_DisruptionBoxMesh);
    CKDestroyObject(m_MutationBoxMesh);
    CKDestroyObject(m_TunnelMesh);

    // Deflector Meshes
    CKDestroyObject(m_DPlaneMesh);
    CKDestroyObject(m_DInfinitePlaneMesh);
    CKDestroyObject(m_DCylinderMesh);
    CKDestroyObject(m_DSphereMesh);
    CKDestroyObject(m_DBoxMesh);

    InitMeshes();
}

void ParticleManager::InitMeshes()
{
    m_GlobalWindMesh = 0;
    m_LocalWindMesh = 0;
    m_MagnetMesh = 0;
    m_VortexMesh = 0;
    m_DisruptionBoxMesh = 0;
    m_MutationBoxMesh = 0;
    m_TunnelMesh = 0;

    m_DPlaneMesh = 0;
    m_DInfinitePlaneMesh = 0;
    m_DCylinderMesh = 0;
    m_DSphereMesh = 0;
    m_DBoxMesh = 0;

    m_PointMesh = 0;
    m_LinearMesh = 0;
    m_PlanarMesh = 0;
    m_CubicMesh = 0;
    m_DiscMesh = 0;
    m_CylindricalMesh = 0;
    m_SphericalMesh = 0;
}

CKERROR ParticleManager::OnCKInit()
{
    m_TotalParticleCount = 0;

    // We create the attributes
    InitAttributes();
    DestroyMeshes();

    return CK_OK;
}

ParticleManager::~ParticleManager()
{
    delete[] ParticleEmitter::m_GlobalIndices;
    ParticleEmitter::m_GlobalIndices = NULL;
}

ParticleEmitter *ParticleManager::CreateNewEmitter(CKGUID guid, CK_ID entity)
{
    // Creation of the emitter
    ParticleEmitter *em = NULL;

    if (guid == POINTSYSTEM_GUID)
    {
        em = new PointEmitter(m_Context, entity, "PointEmitter");
        // POINT MESH
        CreatePointMesh();
        em->m_Mesh = m_PointMesh;
    }
    else if (guid == LINEARSYSTEM_GUID)
    {
        em = new LineEmitter(m_Context, entity, "LinearEmitter");
        // LINEAR MESH
        CreateLinearMesh();
        em->m_Mesh = m_LinearMesh;
    }
    else if (guid == PLANARSYSTEM_GUID)
    {
        em = new PlanarEmitter(m_Context, entity, "PlanarEmitter");
        // PLANAR MESH
        CreatePlanarMesh();
        em->m_Mesh = m_PlanarMesh;
    }
    else if (guid == CUBICSYSTEM_GUID)
    {
        em = new CubicEmitter(m_Context, entity, "CubicEmitter");
        // CUBIC MESH
        CreateCubicMesh();
        em->m_Mesh = m_CubicMesh;
    }
    else if (guid == DISCSYSTEM_GUID)
    {
        em = new DiscEmitter(m_Context, entity, "DiscEmitter");
        // DISC MESH
        CreateDiscMesh();
        em->m_Mesh = m_DiscMesh;
    }
    else if (guid == OBJECTSYSTEM_GUID)
    {
        em = new ObjectEmitter(m_Context, entity, "ObjectEmitter");
    }
    else if (guid == CURVESYSTEM_GUID)
    {
        em = new CurveEmitter(m_Context, entity, "CurveEmitter");
    }
    else if (guid == CYLINDRICALSYSTEM_GUID)
    {
        em = new CylindricalEmitter(m_Context, entity, "CylindricalEmitter");
        // CYLINDRICAL MESH
        CreateCylindricalMesh();
        em->m_Mesh = m_CylindricalMesh;
    }
    else if (guid == SPHERICALSYSTEM_GUID)
    {
        em = new SphericalEmitter(m_Context, entity, "SphericalEmitter");
        // SPHERICAL MESH
        CreateSphericalMesh();
        em->m_Mesh = m_SphericalMesh;
    }
    else if (guid == TIMEPOINTSYSTEM_GUID)
    {
        em = new TimePointEmitter(m_Context, entity, "TimePointEmitter");
    }
    else if (guid == WAVESYSTEM_GUID)
    {
        em = new WaveEmitter(m_Context, entity, "WaveEmitter");
    }

    ++m_TotalParticleCount;
    return em;
}

void ParticleManager::DeleteEmitter(ParticleEmitter *iEmitter)
{
    delete iEmitter;
}

void ParticleManager::InteractorsSetRemoveMesh(CKBOOL iAdd)
{
    _InteractorsSetRemoveMesh(m_GlobalWindAttribute, m_GlobalWindMesh, iAdd);
    _InteractorsSetRemoveMesh(m_LocalWindAttribute, m_LocalWindMesh, iAdd);
    _InteractorsSetRemoveMesh(m_MagnetAttribute, m_MagnetMesh, iAdd);
    _InteractorsSetRemoveMesh(m_VortexAttribute, m_VortexMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DisruptionBoxAttribute, m_DisruptionBoxMesh, iAdd);
    _InteractorsSetRemoveMesh(m_MutationBoxAttribute, m_MutationBoxMesh, iAdd);
    _InteractorsSetRemoveMesh(m_TunnelAttribute, m_TunnelMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DPlaneAttribute, m_DPlaneMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DInfinitePlaneAttribute, m_DInfinitePlaneMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DCylinderAttribute, m_DCylinderMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DSphereAttribute, m_DSphereMesh, iAdd);
    _InteractorsSetRemoveMesh(m_DBoxAttribute, m_DBoxMesh, iAdd);
}

int InteractorsRC(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *ent = (CK3dEntity *)obj;
    ParticleManager *pm = (ParticleManager *)arg;
    CKContext *ctx = pm->m_Context;

    if (!ctx->IsPlaying() || pm->m_ShowInteractors)
    {
        for (int i = 0; i < ent->GetMeshCount(); i++)
        {
            CKMesh *mesh = ent->GetMesh(i);
            if (mesh) mesh->Render(dev, ent);
        }
    }

    return 1;
}

void ParticleManager::_InteractorsSetRemoveMesh(int iAttribute, CK_ID iMeshID, CKBOOL iAdd)
{
    CKAttributeManager *attman = m_Context->GetAttributeManager();

    const XObjectPointerArray &array = attman->GetAttributeListPtr(iAttribute);
    for (CKObject **o = array.Begin(); o != array.End(); ++o)
    {
        // we get the globalWind
        CK3dEntity *ent = (CK3dEntity *)*o;
        if (iAdd)
        {
            ent->SetRenderCallBack(InteractorsRC, this);
            ent->SetCurrentMesh((CKMesh *)m_Context->GetObject(iMeshID));
        }
        else
        {
            ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)m_Context->GetObject(iMeshID));
        }
    }
}

void ParticleAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg)
{
    CKContext *ctx = obj->GetCKContext();
    ParticleManager *pm = (ParticleManager *)arg;
    CK3dEntity *ent = (CK3dEntity *)obj;

    if (Set)
    {
        if (AttribType == pm->m_GlobalWindAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateGlobalWindMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_GlobalWindMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_LocalWindAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateLocalWindMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_LocalWindMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_MagnetAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateMagnetMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_MagnetMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_VortexAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateVortexMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_VortexMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DisruptionBoxAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDisruptionBoxMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DisruptionBoxMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_MutationBoxAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateMutationBoxMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_MutationBoxMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_TunnelAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateTunnelMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_TunnelMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DPlaneAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDPlaneMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DPlaneMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DInfinitePlaneAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDInfinitePlaneMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DInfinitePlaneMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DCylinderAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDCylinderMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DCylinderMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DSphereAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDSphereMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DSphereMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
        if (AttribType == pm->m_DBoxAttribute)
        {
            if (ent->GetFlags() & CK_3DENTITY_FRAME || ctx->IsInLoad())
            {
                pm->CreateDBoxMesh();
                ent->SetRenderCallBack(InteractorsRC, pm);
                ent->SetCurrentMesh((CKMesh *)ctx->GetObject(pm->m_DBoxMesh));
            }
            else
                ent->RemoveAttribute(AttribType);
            return;
        }
    }
    else
    {
        CKAttributeManager *attman = ctx->GetAttributeManager();

        const XObjectPointerArray &array = attman->GetGlobalAttributeListPtr(AttribType);
        int arraycount = array.Size();

        if (AttribType == pm->m_GlobalWindAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_GlobalWindMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_GlobalWindMesh));
                pm->m_GlobalWindMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_LocalWindAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_LocalWindMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_LocalWindMesh));
                pm->m_LocalWindMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_MagnetAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_MagnetMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_MagnetMesh));
                pm->m_MagnetMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_VortexAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_VortexMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_VortexMesh));
                pm->m_VortexMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DisruptionBoxAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_DisruptionBoxMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_DisruptionBoxMesh));
                pm->m_DisruptionBoxMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_MutationBoxAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_MutationBoxMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_MutationBoxMesh));
                pm->m_MutationBoxMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_TunnelAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_TunnelMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_TunnelMesh));
                pm->m_TunnelMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DPlaneAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_DPlaneMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_DPlaneMesh));
                pm->m_DPlaneMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DInfinitePlaneAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            ent->RemoveMesh((CKMesh *)ctx->GetObject(pm->m_DInfinitePlaneMesh));
            if (!arraycount)
            {
                CKDestroyObject(ctx->GetObject(pm->m_DInfinitePlaneMesh));
                pm->m_DInfinitePlaneMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DCylinderAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            CKMesh *mesh = (CKMesh *)ctx->GetObject(pm->m_DCylinderMesh);
            ent->RemoveMesh(mesh);
            if (!arraycount)
            {
                CKDestroyObject(mesh);
                pm->m_DCylinderMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DSphereAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            CKMesh *mesh = (CKMesh *)ctx->GetObject(pm->m_DSphereMesh);
            ent->RemoveMesh(mesh);
            if (!arraycount)
            {
                CKDestroyObject(mesh);
                pm->m_DSphereMesh = 0;
            }
            return;
        }
        if (AttribType == pm->m_DBoxAttribute)
        {
            if (!(ent->GetFlags() & CK_3DENTITY_FRAME))
                return;
            if (ent->GetMeshCount() == 1)
                ent->RemoveRenderCallBack();
            CKMesh *mesh = (CKMesh *)ctx->GetObject(pm->m_DBoxMesh);
            ent->RemoveMesh(mesh);
            if (!arraycount)
            {
                CKDestroyObject(mesh);
                pm->m_DBoxMesh = 0;
            }
            return;
        }
    }
}
