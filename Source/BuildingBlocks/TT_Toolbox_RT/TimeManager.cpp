#include "TimeManager.h"

TimeManager::TimeManager(CKContext *context) : CKBaseManager(context, TT_TIME_MANAGER_GUID, "TT Time Manager")
{
    context->RegisterNewManager(this);
    m_Timing = true;
    m_Time = 0.0f;
}

TimeManager::~TimeManager()
{
}