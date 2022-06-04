#ifndef BUILDINGBLOCKS_DXMANAGER_H
#define BUILDINGBLOCKS_DXMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#define TT_DX_MANAGER_GUID CKGUID(0x6B7D4793, 0x423C6AD4)

class CTTDXManager : public CKBaseManager
{
public:
    CTTDXManager(CKContext *ctx);
    ~CTTDXManager();

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

    static CTTDXManager *GetManager(CKContext *context)
    {
        return (CTTDXManager *)context->GetManagerByGuid(TT_DX_MANAGER_GUID);
    }

protected:

};

#endif // BUILDINGBLOCKS_DXMANAGER_H