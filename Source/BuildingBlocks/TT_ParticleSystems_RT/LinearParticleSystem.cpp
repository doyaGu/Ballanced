/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            LinearParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "GeneralParticleSystem.h"

CKObjectDeclaration *FillBehaviorLinearParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("LinearParticleSystem");
    od->SetDescription("Emits particles from a line");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(LINEARSYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("TT_Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from a line, oriented in the X local direction of the frame it is attached to.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
