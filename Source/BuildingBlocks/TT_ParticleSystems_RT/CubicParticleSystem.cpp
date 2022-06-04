/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlanarParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_ParticleSystems_RT.h"

#include "GeneralParticleSystem.h"
#include "ParticleEmitter.h"
#include "ParticleGuids.h"
#include "ParticleSystemRenderCallbacks.h"

CKObjectDeclaration *FillBehaviorCubicParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("CubicParticleSystem");
    od->SetDescription("Emits particles from inside a cube");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CUBICSYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from within a cube, defined in size by frame dimensions it is attached to.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
