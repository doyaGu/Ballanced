#ifndef BUILDINGBLOCKS_INTERFACEMANAGER_H
#define BUILDINGBLOCKS_INTERFACEMANAGER_H

#include "CKBaseManager.h"
#include "CKContext.h"

#include "GameInfo.h"
#include "NemoArray.h"

#define TT_INTERFACE_MANAGER_GUID CKGUID(0x30833801, 0x6DEE620D)

enum TT_MSG
{
    TT_MSG_NO_GAMEINFO = 0x5F7,
    TT_MSG_CMO_RESTART = 0x5F8,
    TT_MSG_CMO_LOAD = 0x5F9,
    TT_MSG_EXIT_TO_SYS = 0x5FA,
    TT_MSG_EXIT_TO_TITLE = 0x5FB,
    TT_MSG_LIMIT_FPS = 0x5FC,
    TT_MSG_SCREEN_MODE_CHG = 0x5FD,
    TT_MSG_GO_FULLSCREEN = 0x678,
    TT_MSG_STOP_FULLSCREEN = 0x679,
};

class CTTInterfaceManager : public CKBaseManager
{
public:
    CTTInterfaceManager(CKContext *ctx);
    ~CTTInterfaceManager();

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

    int GetScreenMode() const
    {
        return m_ScreenMode;
    }

    void SetScreenMode(int screenMode)
    {
        m_ScreenMode = screenMode;
    }

    int GetDriver() const
    {
        return m_Driver;
    }

    void SetDriver(int driver)
    {
        m_Driver = driver;
    }

    bool IsTaskSwitchEnabled() const
    {
        return m_TaskSwitchEnabled;
    }

    void SetTaskSwitchEnabled(bool enable)
    {
        m_TaskSwitchEnabled = enable;
    }

    bool IsRookie() const
    {
        return m_Rookie;
    }

    void SetRookie(bool rookie)
    {
        m_Rookie = rookie;
    }

    CGameInfo *GetGameInfo() const
    {
        return m_GameInfo;
    }

    void SetGameInfo(CGameInfo *gameInfo)
    {
        m_GameInfo = gameInfo;
    }

    char *GetCmoName()
    {
        return &m_CmoName[0];
    }

    void SetCmoName(const char *name)
    {
        strncpy(m_CmoName, name, sizeof(m_CmoName));
    }

    char *GetIniName()
    {
        return &m_IniName[0];
    }

    void SetIniName(const char *name)
    {
        strncpy(m_IniName, name, sizeof(m_IniName));
    }

    bool IsWindowActivated() const
    {
        return m_WindowActivated;
    }

    void SetWindowActivated(bool enable = true)
    {
        m_WindowActivated = enable;
    }

    CNemoArrayList *GetNemoArrayList()
    {
        return &m_ArrayList;
    }

    static CTTInterfaceManager *GetManager(CKContext *context)
    {
        return (CTTInterfaceManager *)context->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    }

protected:
    int m_ScreenMode;
    int m_Driver;
    bool m_TaskSwitchEnabled;
    bool m_Rookie;
    CGameInfo *m_GameInfo;
    char m_CmoName[512];
    char m_IniName[128];
    bool m_WindowActivated;
    CNemoArrayList m_ArrayList;
};

#endif // BUILDINGBLOCKS_INTERFACEMANAGER_H