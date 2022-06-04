/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       GeneralParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#ifndef BUILDINGBLOCKS_GENERALPARTICLESYSTEM_H
#define BUILDINGBLOCKS_GENERALPARTICLESYSTEM_H

#include "CKAll.h"

#include "ParticleManager.h"
#include "ParticleSystemRenderCallbacks.h"

#include "ParticleGuids.h"

CKERROR CreateGeneralParticleSystemProto(CKBehaviorPrototype **);
int GeneralParticleSystem(const CKBehaviorContext &behcontext);
CKERROR GeneralParticleSystemCallback(const CKBehaviorContext &behcontext);

#endif // BUILDINGBLOCKS_GENERALPARTICLESYSTEM_H