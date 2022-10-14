/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            TT WaveParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "ParticleGuids.h"
#include "GeneralParticleSystem.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4F4F4C53)

CKObjectDeclaration *FillBehaviorWaveParticleSystemDecl();

CKObjectDeclaration *FillBehaviorWaveParticleSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT WaveParticleSystem");
    od->SetDescription("Emits particles from a point and set Orientation from the Offset vector");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(WAVESYSTEM_GUID);
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TerraTools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGeneralParticleSystemProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("TT_Particles/Terratools");
    od->NeedManager(PARTICLE_MANAGER_GUID);
    return od;
}
