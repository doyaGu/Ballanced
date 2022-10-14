/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SphericalParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ParticleGuids.h"
#include "GeneralParticleSystem.h"

CKObjectDeclaration *FillBehaviorSphericalParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("SphericalParticleSystem");
    od->SetDescription("Emits particles from a sphere");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(SPHERICALSYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("TT_Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from a sphere, which size is defined  by the X,Y and Z frame dimensions.
    The particles directions are the normals at the point they are generated on the surface of the sphere.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
