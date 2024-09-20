#include "CKAll.h"

#include "TimePointEmitter.h"

void TimePointEmitter::ReadInputs(CKBehavior *beh)
{
    float delay = 0.0f;
    beh->GetInputParameterValue(EMISSIONDELAY, &delay);
    m_EmissionDelay = delay;
    ParticleEmitter::ReadInputs(beh);
}