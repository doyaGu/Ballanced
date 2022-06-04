#include "DXManager.h"

#include "TT_Toolbox_RT.h"

CTTDXManager::CTTDXManager(CKContext *context)
    : CKBaseManager(context, TT_DX_MANAGER_GUID, "TT DX Manager")
{
    context->RegisterNewManager(this);
}

CTTDXManager::~CTTDXManager()
{
}