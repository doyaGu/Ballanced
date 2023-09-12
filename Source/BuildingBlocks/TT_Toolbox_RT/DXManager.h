#ifndef TOOLBOX_DXMANAGER_H
#define TOOLBOX_DXMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#define TT_DX_MANAGER_GUID CKGUID(0x6B7D4793, 0x423C6AD4)

class DXManager : public CKBaseManager
{
public:
    DXManager(CKContext *ctx);
    ~DXManager();

    CKERROR OnCKInit() { return CK_OK; }
    CKERROR OnCKEnd() { return CK_OK; }
    CKERROR OnCKPlay() { return CK_OK; }
    CKERROR OnCKReset() { return CK_OK; }
    CKERROR PreClearAll() { return CK_OK; }
    CKERROR PostClearAll() { return CK_OK; }
    CKERROR PostProcess() { return CK_OK; }
    CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_PostProcess |
               CKMANAGER_FUNC_PreClearAll |
               CKMANAGER_FUNC_PostClearAll |
               CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_OnCKReset;
    }

    static DXManager *GetManager(CKContext *context)
    {
        return (DXManager *)context->GetManagerByGuid(TT_DX_MANAGER_GUID);
    }

protected:

};

#endif // TOOLBOX_DXMANAGER_H