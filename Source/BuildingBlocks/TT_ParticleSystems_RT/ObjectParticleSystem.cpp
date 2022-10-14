/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ObjectParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ParticleGuids.h"
#include "GeneralParticleSystem.h"

CKObjectDeclaration *FillBehaviorObjectParticleSystemDecl();

CKObjectDeclaration *FillBehaviorObjectParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("ObjectParticleSystem");
    od->SetDescription("Emits particles from an object surface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(OBJECTSYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("TT_Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from random faces of the object its attached to, following faces normal direction.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
