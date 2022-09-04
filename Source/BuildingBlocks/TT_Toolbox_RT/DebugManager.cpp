#include "DebugManager.h"

DebugManager::DebugManager(CKContext *context) : CKBaseManager(context, TT_DEBUG_MANAGER_GUID, "TT Debug Manager")
{
    context->RegisterNewManager(this);
}

DebugManager::~DebugManager()
{
}