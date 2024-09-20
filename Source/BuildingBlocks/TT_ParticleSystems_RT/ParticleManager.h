#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "CKBaseManager.h"

#include "ParticleTools.h"

#include "PointEmitter.h"
#include "PlanarEmitter.h"
#include "CubicEmitter.h"
#include "LineEmitter.h"
#include "DiscEmitter.h"
#include "ObjectEmitter.h"
#include "CurveEmitter.h"
#include "CylindricalEmitter.h"
#include "SphericalEmitter.h"
#include "TimePointEmitter.h"
#include "WaveEmitter.h"

#define PARTICLE_MANAGER_GUID CKGUID(0x1dd91197, 0x1f703f3)

class ParticleManager : public CKBaseManager
{
public:
    // Ctor
    ParticleManager(CKContext *ctx);
    // Dtor
    ~ParticleManager();

    CKERROR OnCKInit();
    CKERROR PostClearAll() { return OnCKInit(); }

    CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_OnCKInit | CKMANAGER_FUNC_PostClearAll;
    }

    static ParticleManager *GetManager(CKContext *context)
    {
        return (ParticleManager *)context->GetManagerByGuid(PARTICLE_MANAGER_GUID);
    }

    // Methods

    // Particle System Creation
    ParticleEmitter *CreateNewEmitter(CKGUID guid, CK_ID entity);
    void DeleteEmitter(ParticleEmitter *iEmitter);

    // Init
    void InitAttributes();
    void InitMeshes();
    void DestroyMeshes();
    void ShowInteractors(CKBOOL on = TRUE);

    // Deflectors Management
    void ManagePlaneDeflectors(ParticleEmitter *em, float deltat);
    void ManageInfinitePlaneDeflectors(ParticleEmitter *em, float deltat);
    void ManageCylinderDeflectors(ParticleEmitter *em, float deltat);
    void ManageSphereDeflectors(ParticleEmitter *em, float deltat);
    void ManageBoxDeflectors(ParticleEmitter *em, float deltat);
    void ManageObjectDeflectors(ParticleEmitter *em, float deltat);

    // Interactors management
    void ManageGravity(ParticleEmitter *em, float deltat);
    void ManageAtmosphere(ParticleEmitter *em, float deltat);
    void ManageGlobalWind(ParticleEmitter *em, float deltat);
    void ManageLocalWind(ParticleEmitter *em, float deltat);
    void ManageMagnet(ParticleEmitter *em, float deltat);
    void ManageVortex(ParticleEmitter *em, float deltat);
    void ManageDisruptionBox(ParticleEmitter *em, float deltat);
    void ManageMutationBox(ParticleEmitter *em, float deltat);
    void ManageTunnel(ParticleEmitter *em, float deltat);
    void ManageProjector(ParticleEmitter *em, float deltat);

    ///
    // Members
    // Particle Systems Attributes
    // Interactors
    int m_GravityAttribute;
    int m_GlobalWindAttribute;
    int m_LocalWindAttribute;
    int m_MagnetAttribute;
    int m_VortexAttribute;
    int m_DisruptionBoxAttribute;
    int m_MutationBoxAttribute;
    int m_AtmosphereAttribute;
    int m_TunnelAttribute;
    int m_ProjectorAttribute;
    // deflectors
    int m_DPlaneAttribute;
    int m_DInfinitePlaneAttribute;
    int m_DCylinderAttribute;
    int m_DSphereAttribute;
    int m_DBoxAttribute;
    int m_DObjectAttribute;

    // Meshes Creation
    void CreateGlobalWindMesh();
    void CreateLocalWindMesh();
    void CreateMagnetMesh();
    void CreateVortexMesh();
    void CreateDisruptionBoxMesh();
    void CreateMutationBoxMesh();
    void CreateTunnelMesh();

    void CreateDPlaneMesh();
    void CreateDInfinitePlaneMesh();
    void CreateDCylinderMesh();
    void CreateDSphereMesh();
    void CreateDBoxMesh();

    void CreatePointMesh();
    void CreateLinearMesh();
    void CreatePlanarMesh();
    void CreateCubicMesh();
    void CreateDiscMesh();
    void CreateCylindricalMesh();
    void CreateSphericalMesh();

    void InteractorsSetRemoveMesh(CKBOOL iAdd);

    // meshes
    CK_ID m_GlobalWindMesh;
    CK_ID m_LocalWindMesh;
    CK_ID m_MagnetMesh;
    CK_ID m_VortexMesh;
    CK_ID m_DisruptionBoxMesh;
    CK_ID m_MutationBoxMesh;
    CK_ID m_TunnelMesh;

    CK_ID m_DPlaneMesh;
    CK_ID m_DInfinitePlaneMesh;
    CK_ID m_DCylinderMesh;
    CK_ID m_DSphereMesh;
    CK_ID m_DBoxMesh;

    CK_ID m_PointMesh;
    CK_ID m_LinearMesh;
    CK_ID m_PlanarMesh;
    CK_ID m_CubicMesh;
    CK_ID m_DiscMesh;
    CK_ID m_CylindricalMesh;
    CK_ID m_SphericalMesh;

    static char *Name;

    CKBOOL m_ShowInteractors;

    int m_TotalParticleCount;

protected:
    void _InteractorsSetRemoveMesh(int iAttribute, CK_ID iMeshID, CKBOOL iAdd);
};

extern ParticleManager TheParticleManager;

#endif