#include "DebugManager.h"

#include "TT_Toolbox_RT.h"

CTTDebugManager::CTTDebugManager(CKContext *context)
    : CKBaseManager(context, TT_DEBUG_MANAGER_GUID, "TT Debug Manager")
{
    context->RegisterNewManager(this);
}

CTTDebugManager::~CTTDebugManager()
{
}