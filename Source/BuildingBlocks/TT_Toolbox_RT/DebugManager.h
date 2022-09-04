#ifndef BUILDINGBLOCKS_DEBUGMANAGER_H
#define BUILDINGBLOCKS_DEBUGMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#include <stdio.h>

#define TT_DEBUG_MANAGER_GUID CKGUID(0x47510373, 0x711473D7)

class DebugManager : public CKBaseManager
{
public:
    DebugManager(CKContext *ctx);
    ~DebugManager();

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

    static DebugManager *GetManager(CKContext *context)
    {
        return (DebugManager *)context->GetManagerByGuid(TT_DEBUG_MANAGER_GUID);
    }

protected:
    FILE *m_LogFile;
    bool m_DebugMode;
    bool m_LogFileNotOpened;
};

#endif // BUILDINGBLOCKS_DEBUGMANAGER_H