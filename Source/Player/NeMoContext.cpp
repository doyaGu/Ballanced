#include "NeMoContext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "WinContext.h"

#include "TT_InterfaceManager_RT/InterfaceManager.h"

#include "config.h"

CNeMoContext *CNeMoContext::instance = NULL;

CNeMoContext::CNeMoContext()
    : m_CKContext(NULL),
      m_RenderManager(NULL),
      m_BehaviorManager(NULL),
      m_ParameterManager(NULL),
      m_AttributeManager(NULL),
      m_PathManager(NULL),
      m_MessageManager(NULL),
      m_TimeManager(NULL),
      m_PluginManager(NULL),
      m_SoundManager(NULL),
      m_InputManager(NULL),
      m_CollisionManager(NULL),
      m_InterfaceManager(NULL),
      m_RenderContext(NULL),
      m_DebugContext(NULL),
      m_WinContext(NULL),
      m_RenderEngine("CK2_3D"),
      m_Width(DEFAULT_WIDTH),
      m_Height(DEFAULT_HEIGHT),
      m_Bpp(DEFAULT_BPP),
      m_RefreshRate(0),
      m_Fullscreen(false),
      m_DisplayChanged(false),
      m_Driver(0),
      m_ScreenMode(-1),
      m_MsgWindowClose(0),
      m_DebugMode(FALSE),
      m_Debugging(FALSE)
{
    strcpy(m_ProgPath, "");
}

CNeMoContext::~CNeMoContext()
{
}

CKERROR CNeMoContext::Init()
{
    int renderEnginePluginIdx = GetRenderEnginePluginIdx();
    if (renderEnginePluginIdx == -1)
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Found no render-engine! (Driver? Critical!!!)");
        return CKERR_NORENDERENGINE;
    }

    CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), renderEnginePluginIdx, NULL);
    if (res != CK_OK)
    {
        if (res == CKERR_NODLLFOUND)
        {
            TT_ERROR("NemoContext.cpp", "Init()", "Dll not found");
            return CKERR_NODLLFOUND;
        }

        TT_ERROR("NemoContext.cpp", "Init()", "Create Context - render engine not loadable");
        return CKERR_NORENDERENGINE;
    }

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_RenderManager = m_CKContext->GetRenderManager();
    m_BehaviorManager = m_CKContext->GetBehaviorManager();
    m_ParameterManager = m_CKContext->GetParameterManager();
    m_AttributeManager = m_CKContext->GetAttributeManager();
    m_PathManager = m_CKContext->GetPathManager();
    m_MessageManager = m_CKContext->GetMessageManager();
    m_TimeManager = m_CKContext->GetTimeManager();
    m_PluginManager = CKGetPluginManager();

    m_SoundManager = (CKSoundManager *)m_CKContext->GetManagerByGuid(SOUND_MANAGER_GUID);
    m_InputManager = (CKInputManager *)m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    m_CollisionManager = (CKCollisionManager *)m_CKContext->GetManagerByGuid(COLLISION_MANAGER_GUID);
    m_InterfaceManager = (CTTInterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (!m_InterfaceManager)
    {
        TT_ERROR("NemoContext.cpp", "Init()", "No InterfaceManager");
        return CKERR_NODLLFOUND;
    }

    if (!FindScreenMode())
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Found no capable screen mode");
        return CKERR_INVALIDPARAMETER;
    }

    if (!CreateRenderContext())
    {
        TT_ERROR("NemoContext.cpp", "Init()", "Create Render Context Failed");
        return CKERR_INVALIDPARAMETER;
    }

    AddCloseMessage();

    return CK_OK;
}

bool CNeMoContext::ReInit()
{
    if (!FindScreenMode() || !GetRenderContext() || !CreateRenderContext())
        return false;

    m_WinContext->UpdateWindows();
    m_WinContext->ShowWindows();
    return true;
}

bool CNeMoContext::StartUp()
{
    if (CKStartUp() != CK_OK)
    {
        TT_ERROR("NemoContext.cpp", "DoStartUp()", "Virtools Engine is not available - Abort!");
        return false;
    }
    m_PluginManager = CKGetPluginManager();
    return true;
}

void CNeMoContext::Shutdown()
{
    if (!m_RenderContext)
        return;

    Cleanup();

    m_RenderManager->DestroyRenderContext(m_RenderContext);
    m_RenderContext = NULL;

    if (m_CKContext)
        CKCloseContext(m_CKContext);
    m_CKContext = NULL;

    CKShutdown();
}

void CNeMoContext::Play()
{
    m_CKContext->Play();
}

void CNeMoContext::Pause()
{
    m_CKContext->Pause();
}

void CNeMoContext::Reset()
{
    m_CKContext->Reset();
}

void CNeMoContext::Cleanup()
{
    Pause();
    Reset();
    m_CKContext->ClearAll();
}

bool CNeMoContext::IsPlaying() const
{
    return m_CKContext->IsPlaying() == TRUE;
}

bool CNeMoContext::IsReseted() const
{
    return m_CKContext->IsReseted() == TRUE;
}

void CNeMoContext::Update()
{
    if (m_CKContext && IsPlaying())
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

CKERROR CNeMoContext::Process()
{
    if (m_DebugMode)
    {
        if (!m_Debugging)
            m_CKContext->ProcessDebugStart();

        m_Debugging = m_CKContext->ProcessDebugStep();
        m_DebugContext = m_CKContext->GetDebugContext();

        if (!m_Debugging)
            m_CKContext->ProcessDebugEnd();
    }
    else
    {
        return m_CKContext->Process();
    }

    return CK_OK;
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags)
{
    return m_RenderContext->Render(flags);
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

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
    {
        if (m_CKContext)
            CKCloseContext(m_CKContext);
        m_CKContext = NULL;

        TT_ERROR("NemoContext.cpp", "FindScreenMode()", "VxDriverDesc is NULL, critical");
        return false;
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;

    int i;
    int maxRefreshRate = 0;
    for (i = 0; i < dmCount; ++i)
    {
        if (dm[i].RefreshRate > maxRefreshRate)
            maxRefreshRate = dm[i].RefreshRate;
        else
            break;
    }

    for (i = 0; i < dmCount; ++i)
    {
        if (dm[i].Width == m_Width &&
            dm[i].Height == m_Height &&
            dm[i].Bpp == m_Bpp &&
            dm[i].RefreshRate == maxRefreshRate)
        {
            m_ScreenMode = i;
            break;
        }
    }

    return m_ScreenMode >= 0;
}

bool CNeMoContext::ApplyScreenMode(int screenMode)
{
    m_ScreenMode = screenMode;
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
        return false;

    VxDisplayMode *displayMode = &drDesc->DisplayModes[m_ScreenMode];
    m_Width = displayMode->Width;
    m_Height = displayMode->Height;
    m_Bpp = displayMode->Bpp;
    return true;
}

bool CNeMoContext::ChangeScreenMode(int driver, int screenMode)
{
    if (!m_RenderContext)
        return false;

    int driverBefore = m_Driver;
    int screenModeBefore = m_ScreenMode;
    bool fullscreenBefore = IsRenderFullscreen();

    m_DisplayChanged = true;
    m_Driver = driver;

    if (!ApplyScreenMode(screenMode))
    {
        m_Driver = driverBefore;
        ApplyScreenMode(screenModeBefore);

        m_WinContext->SetResolution(m_Width, m_Height);
        m_RenderContext->Resize();

        if (IsRenderFullscreen())
            GoFullscreen();

        m_DisplayChanged = false;
        return false;
    }

    m_RenderContext->StopFullScreen();
    ::Sleep(10);

    m_WinContext->SetResolution(m_Width, m_Height);
    m_RenderContext->Resize();

    if (fullscreenBefore && !m_RenderContext->IsFullScreen())
        GoFullscreen();
    ::Sleep(10);

    m_WinContext->FocusMainWindow();

    m_DisplayChanged = false;
    return true;
}

void CNeMoContext::GoFullscreen()
{
    if (!m_RenderContext)
        return;

    if (m_ScreenMode < 0)
    {
        m_RenderContext->GoFullScreen(m_Width,
                                      m_Height,
                                      m_Bpp,
                                      m_Driver,
                                      m_RefreshRate);
    }
    else
    {
        VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
        if (drDesc)
        {
            VxDisplayMode *displayMode = &drDesc->DisplayModes[m_ScreenMode];
            m_RenderContext->GoFullScreen(displayMode->Width,
                                          displayMode->Height,
                                          displayMode->Bpp,
                                          m_Driver,
                                          m_RefreshRate);
        }
    }

    m_WinContext->FocusMainWindow();
}

void CNeMoContext::SwitchFullscreen()
{
    if (!m_RenderContext)
        return;

    if (IsRenderFullscreen())
        RestoreWindow();
    else
        GoFullscreen();
}

bool CNeMoContext::IsRenderFullscreen() const
{
    return m_RenderContext->IsFullScreen() == TRUE;
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

bool CNeMoContext::RestoreWindow()
{
    if (!m_RenderContext || !IsRenderFullscreen() || m_RenderContext->StopFullScreen() != CK_OK)
        return false;
    m_WinContext->SetResolution(m_Width, m_Height);
    m_RenderContext->Resize();
    m_WinContext->RestoreWindow();
    return true;
}

void CNeMoContext::MinimizeWindow()
{
    if (!m_RenderContext || !IsRenderFullscreen() || m_RenderContext->StopFullScreen() != CK_OK)
        return;
    m_WinContext->MinimizeWindow();
}

bool CNeMoContext::CreateRenderContext()
{
    CKRenderManager *renderManager = m_CKContext->GetRenderManager();
    CKRECT rect = {0, 0, m_Width, m_Height};
    m_RenderContext = renderManager->CreateRenderContext(m_WinContext->GetRenderWindow(), m_Driver, &rect, FALSE, m_Bpp, -1, -1, m_RefreshRate);
    if (!m_RenderContext)
        return false;

    Play();

    if (m_Fullscreen && m_RenderContext->GoFullScreen(m_Width, m_Height, m_Bpp, m_Driver, m_RefreshRate))
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
        RestoreWindow();

    m_WinContext->FocusMainWindow();

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

    Refresh();

    ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
    return true;
}

int CNeMoContext::GetRenderEnginePluginIdx()
{
    if (!m_RenderEngine)
        return -1;

    char filename[512];
    const int pluginCount = m_PluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < pluginCount; ++i)
    {
        CKPluginEntry *entry = m_PluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!entry)
            break;

        CKPluginDll *dll = m_PluginManager->GetPluginDllInfo(entry->m_PluginDllIndex);
        if (!dll)
            break;

        char *dllname = dll->m_DllFileName.Str();
        if (!dllname)
            break;

        _splitpath(dllname, NULL, NULL, filename, NULL);
        if (!_strnicmp(m_RenderEngine, filename, strlen(filename)))
            return i;
    }

    return -1;
}

bool CNeMoContext::ParsePlugins(CKSTRING dir)
{
    if (!m_PluginManager)
        return false;
    return m_PluginManager->ParsePlugins(dir) != 0;
}

void CNeMoContext::AddSoundPath(const char *path)
{
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(SOUND_PATH_IDX, str);
}

void CNeMoContext::AddBitmapPath(const char *path)
{
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(BITMAP_PATH_IDX, str);
}

void CNeMoContext::AddDataPath(const char *path)
{
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(DATA_PATH_IDX, str);
}

void CNeMoContext::SetProgPath(const char *path)
{
    if (path)
        strcpy(m_ProgPath, path);
}

char *CNeMoContext::GetProgPath() const
{
    return (char *)m_ProgPath;
}

CKERROR CNeMoContext::GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo)
{
    return m_CKContext->GetFileInfo(filename, fileinfo);
}

CKERROR CNeMoContext::LoadFile(
    char *filename,
    CKObjectArray *liste,
    CK_LOAD_FLAGS loadFlags,
    CKGUID *readerGuid)
{
    return m_CKContext->Load(filename, liste, loadFlags, readerGuid);
}

CKObject *CNeMoContext::GetObject(CK_ID objID)
{
    return m_CKContext->GetObject(objID);
}

CKObject *CNeMoContext::GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, CKObject *previous)
{
    return m_CKContext->GetObjectByNameAndClass(name, cid, previous);
}

CK_ID *CNeMoContext::GetObjectsListByClassID(CK_CLASSID cid)
{
    return m_CKContext->GetObjectsListByClassID(cid);
}

int CNeMoContext::GetObjectsCountByClassID(CK_CLASSID cid)
{
    return m_CKContext->GetObjectsCountByClassID(cid);
}

CKLevel *CNeMoContext::GetCurrentLevel()
{
    return m_CKContext->GetCurrentLevel();
}

CKScene *CNeMoContext::GetCurrentScene()
{
    return m_CKContext->GetCurrentScene();
}

CKMessageType CNeMoContext::AddMessageType(CKSTRING msg)
{
    return m_MessageManager->AddMessageType(msg);
}

CKMessage *CNeMoContext::SendMessageSingle(
    int msg,
    CKBeObject *dest,
    CKBeObject *sender)
{
    return m_MessageManager->SendMessageSingle(msg, dest, sender);
}

void CNeMoContext::AddCloseMessage()
{
    m_MsgWindowClose = m_MessageManager->AddMessageType("WM_CLOSE");
}

bool CNeMoContext::BroadcastCloseMessage()
{
    return m_MessageManager->SendMessageBroadcast(m_MsgWindowClose, CKCID_BEOBJECT) != NULL;
}