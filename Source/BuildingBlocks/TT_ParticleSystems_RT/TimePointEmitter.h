#ifndef TIMEPOINTEMITTER_H
#define TIMEPOINTEMITTER_H

#include "PointEmitter.h"

class TimePointEmitter : public PointEmitter
{
public:
    TimePointEmitter(CKContext *ctx, CK_ID ent, char *name) : PointEmitter(ctx, ent, name)
    {
        m_Position = VxVector::axis0();
        m_Time = 0.0f;
        m_Flag0x10C = TRUE;
        m_IsTimePointEmitter = TRUE;
    }

    VxVector m_Position;
    float m_EmissionDelay;
    float m_Time;

    void ReadInputs(CKBehavior *beh);
};

#endif