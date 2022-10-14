/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DiscParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "GeneralParticleSystem.h"

CKObjectDeclaration *FillBehaviorDiscParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("DiscParticleSystem");
    od->SetDescription("Emits particles from a disc");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(DISCSYSTEM_GUID);
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("TT_Particles");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    /* rem:
    Particle System which emits particles from a disc, oriented in the Z local direction of the frame it is attached to and defined in size by the X and Y frame dimensions.
    For a complete description of Particle Systems, click <a href="Particle Systems.html">here</a>.
    */
    return od;
}
