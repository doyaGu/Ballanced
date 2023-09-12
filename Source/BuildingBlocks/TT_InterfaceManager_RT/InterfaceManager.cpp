#include "InterfaceManager.h"

#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

InterfaceManager::InterfaceManager(CKContext *context)
    : CKBaseManager(context, TT_INTERFACE_MANAGER_GUID, "TT Interface Manager"),
      m_ScreenMode(0),
      m_Driver(0),
      m_TaskSwitchEnabled(true),
      m_Rookie(false),
      m_GameInfo(NULL),
      m_WindowActivated(false),
      m_ArrayList()
{
    context->RegisterNewManager(this);
    memset(m_CmoName, 0, sizeof(m_CmoName));

    char path[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    ::GetModuleFileNameA(NULL, path, MAX_PATH);
    _splitpath(path, drive, dir, filename, NULL);
}

InterfaceManager::~InterfaceManager() {}