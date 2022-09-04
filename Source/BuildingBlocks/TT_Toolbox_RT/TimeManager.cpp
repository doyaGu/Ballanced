#include "TimeManager.h"

#include "TT_Toolbox_RT.h"

TimeManager::TimeManager(CKContext *context) : CKBaseManager(context, TT_TIME_MANAGER_GUID, "TT Time Manager")
{
    context->RegisterNewManager(this);
}

TimeManager::~TimeManager()
{
}