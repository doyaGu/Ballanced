#include "TimeManager.h"

TimeManager::TimeManager(CKContext *context) : CKBaseManager(context, TT_TIME_MANAGER_GUID, "TT Time Manager")
{
    context->RegisterNewManager(this);
}

TimeManager::~TimeManager()
{
}