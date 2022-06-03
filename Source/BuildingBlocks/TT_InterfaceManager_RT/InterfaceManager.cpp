#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

#include <string.h>

CTTInterfaceManager::CTTInterfaceManager(CKContext *context)
    : CKBaseManager(context, TT_INTERFACE_MANAGER_GUID, "TT Interface Manager"),
      m_ScreenModeIndex(0),
      m_DriverIndex(0),
      m_TaskSwitchEnabled(true),
      m_Rookie(false),
      m_GameInfo(NULL),
      m_WindowActivated(false),
      m_ArrayList()
{
    context->RegisterNewManager(this);
    memset(m_CmoName, 0, sizeof(m_CmoName));
    memset(m_IniName, 0, sizeof(m_IniName));
}

CTTInterfaceManager::~CTTInterfaceManager() {}