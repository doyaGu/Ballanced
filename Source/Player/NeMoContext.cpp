#include "StdAfx.h"

#include "NeMoContext.h"

#include <stdio.h>
#include <stdlib.h>

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "TT_InterfaceManager_RT/InterfaceManager.h"
#include "WinContext.h"

CNeMoContext *CNeMoContext::instance = NULL;

CNeMoContext::CNeMoContext()
    : m_MsgWindowClose(0),
      m_CKContext(NULL),
      m_RenderManager(NULL),
      m_TimeManager(NULL),
      m_PluginManager(NULL),
      m_MessageManager(NULL),
      m_RenderContext(NULL),
      m_WinContext(NULL),
      m_Width(DEFAULT_WIDTH),
      m_Height(DEFAULT_HEIGHT),
      m_Bpp(DEFAULT_BPP),
      m_RefreshRate(0),
      m_Fullscreen(false),
      m_DisplayChanged(false),
      m_DriverIndex(0),
      m_ScreenModeIndex(-1),
      m_MsgClick(0)
{
    m_RenderEngine = CKStrdup("CK2_3D");
    strcpy(m_ProgPath, "");
}

CNeMoContext::~CNeMoContext()
{
    delete[] m_RenderEngine;
}

void CNeMoContext::SetDriverIndex(int idx)
{
    m_DriverIndex = idx;
}

bool CNeMoContext::ApplyScreenMode(int idx)
{
    m_ScreenModeIndex = idx;
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
    if (!drDesc)
    {
        return false;
    }

    VxDisplayMode *displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
    m_Width = displayMode->Width;
    m_Height = displayMode->Height;
    m_Bpp = displayMode->Bpp;
    return true;
}

void CNeMoContext::SetFullscreen(bool fullscreen)
{
    m_Fullscreen = fullscreen;
}

void CNeMoContext::SetBPP(int bpp)
{
    m_Bpp = bpp;
}

void CNeMoContext::SetRefreshRate(int fps)
{
    m_RefreshRate = fps;
}

void CNeMoContext::SetProgPath(const char *path)
{
    if (path)
    {
        strcpy(m_ProgPath, path);
    }
}

void CNeMoContext::SetMsgClick(int msg)
{
    m_MsgClick = msg;
}

void CNeMoContext::SetRenderContext(CKRenderContext *renderContext)
{
    m_RenderContext = renderContext;
}

void CNeMoContext::SetWinContext(CWinContext *winContext)
{
    m_WinContext = winContext;
}

void CNeMoContext::SetResolution(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

void CNeMoContext::SetWidth(int width)
{
    m_Width = width;
}

void CNeMoContext::SetHeight(int height)
{
    m_Height = height;
}

int CNeMoContext::GetDriverIndex() const
{
    return m_DriverIndex;
}

int CNeMoContext::GetScreenModeIndex() const
{
    return m_ScreenModeIndex;
}

bool CNeMoContext::IsFullscreen() const
{
    return m_Fullscreen;
}

void CNeMoContext::GetResolution(int &width, int &height)
{
    width = m_Width;
    height = m_Height;
}

int CNeMoContext::GetWidth() const
{
    return m_Width;
}

int CNeMoContext::GetHeight() const
{
    return m_Height;
}

int CNeMoContext::GetBPP() const
{
    return m_Bpp;
}

int CNeMoContext::GetRefreshRate() const
{
    return m_RefreshRate;
}

char *CNeMoContext::GetProgPath() const
{
    return (char *)m_ProgPath;
}

int CNeMoContext::GetMsgClick() const
{
    return m_MsgClick;
}

bool CNeMoContext::IsRenderFullScreen() const
{
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool CNeMoContext::DoStartUp()
{
    if (CKStartUp() != CK_OK)
    {
        TT_ERROR("NemoContext.cpp", "DoStartUp()", "Virtools Engine is not available - Abort!");
        return false;
    }
    m_PluginManager = CKGetPluginManager();
    return true;
}

void CNeMoContext::Pause()
{
    m_CKContext->Pause();
}

void CNeMoContext::Play()
{
    m_CKContext->Play();
}

void CNeMoContext::MinimizeWindow()
{
    m_WinContext->MinimizeWindow();
}

CKERROR CNeMoContext::Reset()
{
    return m_CKContext->Reset();
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags)
{
    return m_RenderContext->Render(flags);
}

void CNeMoContext::Cleanup()
{
    Pause();
    Reset();
    m_CKContext->ClearAll();
}

void CNeMoContext::Shutdown()
{
    if (!m_RenderContext)
    {
        return;
    }

    Cleanup();

    m_RenderManager->DestroyRenderContext(m_RenderContext);
    m_RenderContext = NULL;

    if (m_CKContext)
    {
        CKCloseContext(m_CKContext);
    }
    m_CKContext = NULL;

    CKShutdown();
}

void CNeMoContext::GoFullscreen()
{
    if (!m_RenderContext)
    {
        return;
    }

    if (m_ScreenModeIndex >= 0)
    {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
        if (drDesc)
        {
            VxDisplayMode *displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
            m_RenderContext->GoFullScreen(displayMode->Width,
                                          displayMode->Height,
                                          displayMode->Bpp,
                                          m_DriverIndex,
                                          m_RefreshRate);
            ::SetFocus(m_WinContext->GetMainWindow());
        }
    }
    else
    {
        m_RenderContext->GoFullScreen(m_Width,
                                      m_Height,
                                      m_Bpp,
                                      m_DriverIndex,
                                      m_RefreshRate);
        ::SetFocus(m_WinContext->GetMainWindow());
    }
}

CKERROR CNeMoContext::Process()
{
    return m_CKContext->Process();
}

void CNeMoContext::Update()
{
    if (IsPlaying())
    {
        float beforeRender = 0.0f;
        float beforeProcess = 0.0f;
        m_TimeManager->GetTimeToWaitForLimits(beforeRender, beforeProcess);
        if (beforeProcess <= 0)
        {
            m_TimeManager->ResetChronos(FALSE, TRUE);
            Process();
        }
        if (beforeRender <= 0)
        {
            m_TimeManager->ResetChronos(TRUE, FALSE);
            Render();
        }
    }
}

void CNeMoContext::SwitchFullscreen()
{
    if (!m_RenderContext)
    {
        return;
    }

    if (IsRenderFullScreen())
    {
        RestoreWindow();
    }
    else if (m_ScreenModeIndex >= 0)
    {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
        if (drDesc)
        {
            VxDisplayMode *displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
            m_RenderContext->GoFullScreen(displayMode->Width,
                                          displayMode->Height,
                                          displayMode->Bpp,
                                          m_DriverIndex,
                                          m_RefreshRate);
            ::ShowWindow(m_WinContext->GetMainWindow(), SW_SHOWMINNOACTIVE);
        }
    }
    else
    {
        m_RenderContext->GoFullScreen(m_Width,
                                      m_Height,
                                      m_Bpp,
                                      m_DriverIndex,
                                      m_RefreshRate);
        ::ShowWindow(m_WinContext->GetMainWindow(), SW_SHOWMINNOACTIVE);
    }
}

bool CNeMoContext::RestoreWindow()
{
    if (!m_RenderContext || !IsRenderFullScreen() || m_RenderContext->StopFullScreen())
    {
        return false;
    }
    ::ShowWindow(m_WinContext->GetMainWindow(), SW_RESTORE);
    return true;
}

bool CNeMoContext::Init()
{
    int renderEnginePluginIdx = GetRenderEnginePluginIdx();
    if (renderEnginePluginIdx == -1)
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Found no render-engine! (Driver? Critical!!!)");
        return false;
    }

    CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), renderEnginePluginIdx, NULL);
    if (res != CK_OK)
    {
        if (res == CKERR_NODLLFOUND)
        {
            TT_ERROR("NemoContext.cpp", "Init()", "Dll not found");
            return false;
        }

        TT_ERROR("NemoContext.cpp", "Init()", "Create Context - render engine not loadable");
        return false;
    }

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_MessageManager = m_CKContext->GetMessageManager();
    m_TimeManager = m_CKContext->GetTimeManager();
    m_RenderManager = m_CKContext->GetRenderManager();

    AddMessageWindowClose();

    if (!FindScreenMode())
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Found no capable screen mode");
        return false;
    }

    if (!CreateRenderContext())
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Create Render Context Failed");
        return false;
    }

    m_WinContext->UpdateWindows();
    m_WinContext->ShowWindows();

    return true;
}

bool CNeMoContext::CreateRenderContext()
{
    CKRenderManager *renderManager = m_CKContext->GetRenderManager();
    CKRECT rect = {0, 0, m_Width, m_Height};
    m_RenderContext = renderManager->CreateRenderContext(m_WinContext->GetRenderWindow(), m_DriverIndex, &rect, FALSE, m_Bpp, -1, -1, m_RefreshRate);
    if (!m_RenderContext)
    {
        return false;
    }

    Play();

    if (m_Fullscreen && m_RenderContext->GoFullScreen(m_Width, m_Height, m_Bpp, m_DriverIndex, m_RefreshRate))
    {
        if (!RestoreWindow())
        {
            TT_ERROR("NemoContext.cpp", "CreateRenderContext()", "WindowMode cannot be changed");
            return false;
        }

        ::SetWindowPos(m_WinContext->GetMainWindow(), HWND_TOPMOST, 0, 0, m_Width, m_Height, 0);
        ::SetWindowPos(m_WinContext->GetRenderWindow(), HWND_TOPMOST, 0, 0, m_Width, m_Height, 0);
        return false;
    }

    Pause();

    if (!m_Fullscreen)
    {
        RestoreWindow();
    }
    ::SetFocus(m_WinContext->GetMainWindow());

    m_RenderContext->Clear();
    m_RenderContext->Clear(CK_RENDER_BACKGROUNDSPRITES);
    m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES);
    m_RenderContext->Clear(CK_RENDER_USECAMERARATIO);
    m_RenderContext->Clear(CK_RENDER_CLEARZ);
    m_RenderContext->Clear(CK_RENDER_CLEARBACK);
    m_RenderContext->Clear(CK_RENDER_DOBACKTOFRONT);
    m_RenderContext->Clear(CK_RENDER_DEFAULTSETTINGS);
    m_RenderContext->Clear(CK_RENDER_CLEARVIEWPORT);
    m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES);
    m_RenderContext->Clear(CK_RENDER_USECAMERARATIO);
    m_RenderContext->Clear(CK_RENDER_WAITVBL);
    m_RenderContext->Clear(CK_RENDER_PLAYERCONTEXT);
    m_RenderContext->Clear();

    m_RenderContext->SetClearBackground();
    m_RenderContext->BackToFront();
    m_RenderContext->SetClearBackground();
    m_RenderContext->Clear();

    ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
    return true;
}

int CNeMoContext::GetRenderEnginePluginIdx()
{
    if (!m_RenderEngine)
    {
        return -1;
    }

    char filename[512];
    const int pluginCount = m_PluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < pluginCount; ++i)
    {
        CKPluginEntry *entry = m_PluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!entry)
        {
            break;
        }

        CKPluginDll *dll = m_PluginManager->GetPluginDllInfo(entry->m_PluginDllIndex);
        if (!dll)
        {
            break;
        }

        char *dllname = dll->m_DllFileName.Str();
        if (!dllname)
        {
            break;
        }

        _splitpath(dllname, NULL, NULL, filename, NULL);
        if (!_strnicmp(m_RenderEngine, filename, strlen(filename)))
        {
            return i;
        }
    }
    return -1;
}

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
    if (!drDesc)
    {
        if (m_CKContext)
        {
            CKCloseContext(m_CKContext);
        }
        m_CKContext = NULL;

        TT_ERROR("NemoContext.cpp", "FindScreenMode()", "VxDriverDesc is NULL, critical");
        return false;
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
    for (int i = 0; i < dmCount; ++i)
    {
        if (dm[i].Width == m_Width &&
            dm[i].Height == m_Height &&
            dm[i].Bpp == m_Bpp)
        {
            m_ScreenModeIndex = i;
            break;
        }
    }

    return m_ScreenModeIndex >= 0;
}

bool CNeMoContext::ReInit()
{
    if (!FindScreenMode() || !GetRenderContext() || !CreateRenderContext())
    {
        return false;
    }

    m_WinContext->UpdateWindows();
    m_WinContext->ShowWindows();
    return true;
}

void CNeMoContext::SetScreen(CWinContext *wincontext,
                             bool fullscreen,
                             int driver,
                             int bpp,
                             int width,
                             int height)
{
    m_WinContext = wincontext;
    m_Fullscreen = fullscreen;
    m_Bpp = bpp;
    m_Width = width;
    m_Height = height;
    m_ScreenModeIndex = driver;
}

void CNeMoContext::SetWindow(CWinContext *wincontext,
                             bool fullscreen,
                             int bpp,
                             int width,
                             int height)
{
    m_WinContext = wincontext;
    m_Fullscreen = fullscreen;
    m_Bpp = bpp;
    m_Width = width;
    m_Height = height;
}

CKRenderContext *CNeMoContext::GetRenderContext() const
{
    return m_RenderContext;
}

CKERROR CNeMoContext::GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo)
{
    return m_CKContext->GetFileInfo(filename, fileinfo);
}

void CNeMoContext::AddSoundPath(const char *path)
{
    m_CKContext->GetPathManager()->AddPath(SOUND_PATH_IDX, XString(path));
}

void CNeMoContext::AddBitmapPath(const char *path)
{
    m_CKContext->GetPathManager()->AddPath(BITMAP_PATH_IDX, XString(path));
}

void CNeMoContext::AddDataPath(const char *path)
{
    m_CKContext->GetPathManager()->AddPath(DATA_PATH_IDX, XString(path));
}

CKERROR CNeMoContext::LoadFile(
    char *filename,
    CKObjectArray *liste,
    CK_LOAD_FLAGS loadFlags,
    CKGUID *readerGuid)
{
    return m_CKContext->Load(filename, liste, loadFlags, readerGuid);
}

CKLevel *CNeMoContext::GetCurrentLevel()
{
    return m_CKContext->GetCurrentLevel();
}

CK_ID *CNeMoContext::GetObjectsListByClassID(CK_CLASSID cid)
{
    return m_CKContext->GetObjectsListByClassID(cid);
}

CKObject *CNeMoContext::GetObject(CK_ID objID)
{
    return m_CKContext->GetObject(objID);
}

CKRenderManager *CNeMoContext::GetRenderManager()
{
    return m_RenderManager;
}

int CNeMoContext::GetObjectsCountByClassID(CK_CLASSID cid)
{
    return m_CKContext->GetObjectsCountByClassID(cid);
}

CKMessageType CNeMoContext::AddMessageType(CKSTRING msg)
{
    return m_MessageManager->AddMessageType(msg);
}

bool CNeMoContext::ParsePlugins(CKSTRING dir)
{
    m_PluginManager = CKGetPluginManager();
    if (!m_PluginManager)
    {
        return false;
    }
    return m_PluginManager->ParsePlugins(dir) != 0;
}

CTTInterfaceManager *CNeMoContext::GetInterfaceManager()
{
    if (m_CKContext)
    {
        return (CTTInterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    }
    return NULL;
}

CKMessage *CNeMoContext::SendMessageSingle(
    int msg,
    CKBeObject *dest,
    CKBeObject *sender)
{
    return m_MessageManager->SendMessageSingle(msg, dest, sender);
}

bool CNeMoContext::IsPlaying() const
{
    return m_CKContext->IsPlaying() == TRUE;
}

bool CNeMoContext::ChangeScreenMode(int driver, int screenMode)
{
    if (!m_RenderContext)
    {
        return false;
    }

    int driverBefore = m_DriverIndex;
    int screenModeBefore = m_ScreenModeIndex;
    bool fullscreenBefore = IsRenderFullScreen();

    m_DisplayChanged = true;
    m_DriverIndex = driver;

    if (!ApplyScreenMode(screenMode))
    {
        m_DriverIndex = driverBefore;
        m_ScreenModeIndex = screenModeBefore;
        m_DisplayChanged = false;
        return false;
    }

    m_RenderContext->StopFullScreen();
    ::Sleep(10);

    m_WinContext->SetResolution(m_Width, m_Height);
    m_RenderContext->Resize();

    if (fullscreenBefore && !m_RenderContext->IsFullScreen())
    {
        GoFullscreen();
    }

    ::Sleep(10);
    ::SetFocus(m_WinContext->GetMainWindow());

    m_DisplayChanged = false;
    return true;
}

bool CNeMoContext::SendMessageWindowCloseToAll()
{
    return m_MessageManager->SendMessageBroadcast(m_MsgWindowClose, CKCID_BEOBJECT) != NULL;
}

void CNeMoContext::AddMessageWindowClose()
{
    m_MsgWindowClose = m_MessageManager->AddMessageType("WM_CLOSE");
}

CKContext *CNeMoContext::GetCKContext()
{
    return m_CKContext;
}

void CNeMoContext::Refresh()
{
    if (m_RenderContext)
    {
        m_RenderContext->Clear();
        m_RenderContext->SetClearBackground();
        m_RenderContext->BackToFront();
        m_RenderContext->SetClearBackground();
        m_RenderContext->Clear();
    }
}

void CNeMoContext::ResizeWindow()
{
    RECT rect;
    ::GetClientRect(m_WinContext->GetMainWindow(), &rect);
    ::SetWindowPos(m_WinContext->GetRenderWindow(),
                   NULL,
                   0,
                   0,
                   rect.right - rect.left,
                   rect.bottom - rect.top,
                   SWP_NOMOVE | SWP_NOZORDER);
    m_RenderContext->Resize();
}