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

// ACC - July 10,2002
#ifndef THREADPARAM
#define THREADPARAM
#include "BlockingQueue.h"

class ThreadParam
{
public:
    ParticleEmitter *pe;
    float DeltaTime;
};

int UpdateParticleSystemEnqueue(ParticleEmitter *aPE, float aDeltaTime);

#ifdef WIN32
DWORD WINAPI PSWorkerThreadFunc(LPVOID junk);
#endif

#endif // !THREADPARAM

#endif