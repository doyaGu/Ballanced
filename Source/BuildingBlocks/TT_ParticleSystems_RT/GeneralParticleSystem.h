/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GeneralParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#ifndef GENERALPARTICLESYSTEM_H
#define GENERALPARTICLESYSTEM_H
#include "CKAll.h"

#include "ParticleSystemRenderCallbacks.h"
#include "ParticleManager.h"

#include "ParticleGuids.h"

CKERROR CreateGeneralParticleSystemProto(CKBehaviorPrototype **);
int GeneralParticleSystem(const CKBehaviorContext &behcontext);
CKERROR GeneralParticleSystemCallback(const CKBehaviorContext &behcontext);

#endif