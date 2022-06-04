#include "TimeManager.h"

#include "TT_Toolbox_RT.h"

CTTTimeManager::CTTTimeManager(CKContext *context)
    : CKBaseManager(context, TT_TIME_MANAGER_GUID, "TT Time Manager")
{
    context->RegisterNewManager(this);
}

CTTTimeManager::~CTTTimeManager()
{
}