#include "DXManager.h"

DXManager::DXManager(CKContext *context) : CKBaseManager(context, TT_DX_MANAGER_GUID, "TT DX Manager")
{
    context->RegisterNewManager(this);
}

DXManager::~DXManager()
{
}