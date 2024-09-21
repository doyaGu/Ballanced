#include "NeMoContext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "WinContext.h"

#include "InterfaceManager.h"

#include "config.h"

CNeMoContext *CNeMoContext::instance = NULL;

CNeMoContext::CNeMoContext()
{
    m_RenderContext = NULL;
    m_CKContext = NULL;
    m_FrameRateSprite = NULL;
    m_MadeWithSprite = NULL;
    m_ProgPath[0] = '\0';
    m_RenderEngine = "CK2_3D";
    m_RefreshRate = 0;
    m_ShowMadeWith = false;
    m_DriverIndex = 0;
    m_TimeToHideSprite = 1;
    m_ScreenModeIndex = -1;
    SetResolution(PLAYER_DEFAULT_WIDTH, PLAYER_DEFAULT_HEIGHT);
    field_4C = 82.0f;
    m_PluginManager = NULL;
    m_DisplayChanged = false;
}

CNeMoContext::~CNeMoContext() {}

float CNeMoContext::GetField0x4C() const
{
    return field_4C;
}

void CNeMoContext::SetField0x4C(float val)
{
    field_4C = val;
}

void CNeMoContext::SetFrameRateSpritePosition(int x, int y)
{
    m_FrameRateSprite->SetPosition(Vx2DVector(x, y));
}

void CNeMoContext::SetMadeWithSpritePosition(int x, int y)
{
    m_MadeWithSprite->SetPosition(Vx2DVector(x, y));
}

void CNeMoContext::SetFrameRateSpriteText(CKSTRING text)
{
    m_FrameRateSprite->SetText(text);
}

void CNeMoContext::SetMadeWithSpriteText(CKSTRING text)
{
    m_MadeWithSprite->SetText(text);
}

void CNeMoContext::AdjustFrameRateSpritePosition()
{
    SetFrameRateSpritePosition((int)(m_Width * 0.5 - 60.0), (int)(m_Height * 0.5 - 15.0));
}

void CNeMoContext::SetTimeToHideSprite(int time)
{
    m_TimeToHideSprite = time;
}

void CNeMoContext::ShowFrameRate(bool show)
{
    m_ShowFrameRate = show;
}

void CNeMoContext::ShowMadeWith(bool show)
{
    m_ShowMadeWith = show;
}

int CNeMoContext::GetTimeToHideSprite() const
{
    return m_TimeToHideSprite;
}

bool CNeMoContext::IsShowingFrameRate() const
{
    return m_ShowFrameRate;
}

bool CNeMoContext::IsShowingMadeWith() const
{
    return m_ShowMadeWith;
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
        return false;

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
    if (!path)
    {
        TT_ERROR("NemoContext.cpp", "SetProgPath()", "empty paramter pa_progpath");
        throw CNeMoContextException(3);
    }
    strcpy(m_ProgPath, path);
}

void CNeMoContext::SetMsgDoubleClick(int msg)
{
    m_MsgDoubleClick = msg;
}

void CNeMoContext::SetMsgClick(int msg)
{
    m_MsgClick = msg;
}

void CNeMoContext::SetRenderContext(CKRenderContext *renderContext)
{
    m_RenderContext = renderContext;
}

void CNeMoContext::SetFrameRateSprite(CKSpriteText *sprite)
{
    m_FrameRateSprite = sprite;
}

void CNeMoContext::SetMadeWithSprite(CKSpriteText *sprite)
{
    m_MadeWithSprite = sprite;
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
    if (m_ProgPath[0] == '\0')
    {
        TT_ERROR("NemoContext.cpp", "CNemoContext::GetProgPath()", "empty prog path");
        throw CNeMoContextException(3);
    }
    return (char *)m_ProgPath;
}

int CNeMoContext::GetMsgDoubleClick() const
{
    return m_MsgDoubleClick;
}

int CNeMoContext::GetMsgClick() const
{
    return m_MsgClick;
}

bool CNeMoContext::IsRenderFullScreen() const
{
    return m_RenderContext->IsFullScreen() == TRUE;
}

void CNeMoContext::DoStartUp()
{
    if (CKStartUp() != CK_OK)
    {
        TT_ERROR("NemoContext.cpp", "DoStartUp()", "Virtools Engine is not available - Abort!");
        throw CNeMoContextException(6);
    }
    m_PluginManager = CKGetPluginManager();
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
    if (!m_RenderContext || !IsRenderFullScreen() || m_RenderContext->StopFullScreen() != CK_OK)
        return;
    ::ShowWindow(m_WinContext->GetMainWindow(), SW_MINIMIZE);
}

CKERROR CNeMoContext::Reset()
{
    return m_CKContext->Reset();
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags)
{
    return m_RenderContext->Render(flags);
}

void CNeMoContext::HideFrameRateSprite()
{
    m_FrameRateSprite->Show(CKHIDE);
}

void CNeMoContext::ShowFrameRateSprite()
{
    m_FrameRateSprite->Show();
}

void CNeMoContext::HideMadeWithSprite()
{
    m_MadeWithSprite->Show(CKHIDE);
}

void CNeMoContext::ShowMadeWithSprite()
{
    m_MadeWithSprite->Show();
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
        return;

    Cleanup();

    m_RenderManager->DestroyRenderContext(m_RenderContext);
    m_RenderContext = NULL;

    if (m_CKContext)
        CKCloseContext(m_CKContext);
    m_CKContext = NULL;

    CKShutdown();
}

void CNeMoContext::GoFullscreen()
{
    if (!m_RenderContext)
        return;

    if (m_ScreenModeIndex < 0)
    {
        m_RenderContext->GoFullScreen(m_Width,
                                      m_Height,
                                      m_Bpp,
                                      m_DriverIndex,
                                      m_RefreshRate);
        ::SetFocus(m_WinContext->GetMainWindow());
    }
    else
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
}

void CNeMoContext::Process()
{
    try
    {
        if (IsPlaying())
        {
            if (IsShowingMadeWith() && 0 < m_TimeToHideSprite && m_TimeToHideSprite < ::GetTickCount())
            {
                HideMadeWithSprite();
                m_TimeToHideSprite = 0;
            }
            float timeBeforeRender = 0.0f;
            float timeBeforeProcess = 0.0f;
            m_TimeManager->GetTimeToWaitForLimits(timeBeforeRender, timeBeforeProcess);
            if (timeBeforeProcess <= 0)
            {
                m_TimeManager->ResetChronos(FALSE, TRUE);
                m_CKContext->Process();
            }
            if (timeBeforeRender <= 0)
            {
                m_TimeManager->ResetChronos(TRUE, FALSE);
                m_RenderContext->Render();
            }
        }
    }
    catch (...)
    {
        TT_ERROR("NemoContext.cpp", "Process()", "Unknown exception");
        throw CNeMoContextException();
    }
}

void CNeMoContext::Update(int state)
{
    switch (state)
    {
    case 1:
        if (IsShowingMadeWith() && m_CKContext->IsReseted())
        {
            m_TimeToHideSprite = ::GetTickCount() + 3000;
            ShowMadeWithSprite();
        }
        Play();
        break;
    case 2:
        Pause();
        break;
    default:
        if (!IsPlaying())
        {
            if (IsShowingMadeWith() && m_CKContext->IsReseted())
            {
                m_TimeToHideSprite = ::GetTickCount() + 3000;
                HideMadeWithSprite();
            }
            Play();
            break;
        }
        Pause();
        break;
    }
}

void CNeMoContext::SwitchFullscreen()
{
    if (!m_RenderContext)
        return;

    if (IsRenderFullScreen())
    {
        RestoreWindow();
    }
    else
    {
        if (m_ScreenModeIndex < 0)
        {
            m_RenderContext->GoFullScreen(m_Width,
                                          m_Height,
                                          m_Bpp,
                                          m_DriverIndex,
                                          m_RefreshRate);
            ::ShowWindow(m_WinContext->GetMainWindow(), SW_SHOWMINNOACTIVE);
        }
        else
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
    }
}

bool CNeMoContext::RestoreWindow()
{
    if (!m_RenderContext || !IsRenderFullScreen() || m_RenderContext->StopFullScreen() != CK_OK)
        return false;
    ::ShowWindow(m_WinContext->GetMainWindow(), SW_RESTORE);
    return true;
}

bool CNeMoContext::Init()
{
    int renderEnginePluginIdx = GetRenderEnginePluginIdx();

    try
    {
        if (renderEnginePluginIdx == -1)
        {
            TT_ERROR("NemoContext.cpp", "Init()", "Found no render-engine! (Driver? Critical!!!)");
            throw CNeMoContextException(1);
        }

#if CKVERSION == 0x13022002
        CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), renderEnginePluginIdx, 0);
#else
        CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), 0);
#endif
        if (res != CK_OK)
        {
            if (res == CKERR_NODLLFOUND)
            {
                TT_ERROR("NemoContext.cpp", "Init()", "Dll not found");
                throw CNeMoContextException(1);
            }

            TT_ERROR("NemoContext.cpp", "Init()", "Create Context - render engine not loadable");
            throw CNeMoContextException(1);
        }

        m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
        m_MessageManager = m_CKContext->GetMessageManager();
        m_TimeManager = m_CKContext->GetTimeManager();
        m_RenderManager = m_CKContext->GetRenderManager();

        AddMessageWindowClose();

        if (!FindScreenMode() || !CreateRenderContext())
            return false;

        m_WinContext->UpdateWindows();
        m_WinContext->ShowWindows();

        CreateInterfaceSprite();

        return true;
    }
    catch (const CNeMoContextException &nmce)
    {
        if (nmce.error == 5)
            TT_ERROR("NemoContext.cpp", "Init()", "Problems with the render driver description");
        throw CNeMoContextException(1);
    }
}

void CNeMoContext::CreateInterfaceSprite()
{
    try
    {
        m_FrameRateSprite = (CKSpriteText *)m_CKContext->CreateObject(CKCID_SPRITETEXT, "FrameRateSprite");
        m_FrameRateSprite->Create(60, 20);
        m_FrameRateSprite->SetTransparent(FALSE);
        m_FrameRateSprite->SetFont("Arial", 8);
        m_FrameRateSprite->SetBackgroundColor(0);
        m_FrameRateSprite->SetTextColor(0x00FFFFFF);
        m_FrameRateSprite->SetPosition(Vx2DVector(350.0f, 225.0f));
        m_FrameRateSprite->Show(CKHIDE);

        m_MadeWithSprite = (CKSpriteText *)m_CKContext->CreateObject(CKCID_SPRITETEXT, "MadeWithSprite");
        m_MadeWithSprite->Create(m_Width, m_Height);
        m_MadeWithSprite->SetTransparent(FALSE);
        m_MadeWithSprite->SetFont("Arial", 14);
        m_MadeWithSprite->SetBackgroundColor(0x00FFFFFF);
        m_MadeWithSprite->SetTextColor(0x00FFFF00);
        m_MadeWithSprite->SetZOrder(2000000);
        m_MadeWithSprite->Show(CKHIDE);

        if (!m_RenderContext)
            throw CNeMoContextException(2);

        m_RenderContext->AddObject(m_MadeWithSprite);
        m_RenderContext->AddObject(m_FrameRateSprite);
        m_FrameRateSprite->SetPosition(Vx2DVector(0.0f, 0.0f));
        HideMadeWithSprite();
    }
    catch (...)
    {
        TT_ERROR("NemoContext.cpp", "CreateInterfaceSprite()", "Create Sprite Exception");
        throw CNeMoContextException(4);
    }
}

bool CNeMoContext::CreateRenderContext()
{
    m_RenderContext = NULL;

    try
    {
        CKRenderManager *renderManager = m_CKContext->GetRenderManager();
        CKRECT rect = {0, 0, m_Width, m_Height};
        m_RenderContext = renderManager->CreateRenderContext(m_WinContext->GetRenderWindow(), m_DriverIndex, &rect, FALSE, m_Bpp, -1, -1, m_RefreshRate);
        if (!m_RenderContext)
            return false;

        Play();

        if (IsFullscreen() && m_RenderContext->GoFullScreen(m_Width, m_Height, m_Bpp, m_DriverIndex, 0))
        {
            if (!RestoreWindow())
            {
                TT_ERROR("NemoContext.cpp", "CreateRenderContext()", "WindowMode cannot be changed");
                throw CNeMoContextException(2);
            }

            ::SetWindowPos(m_WinContext->GetMainWindow(), HWND_TOPMOST, 0, 0, m_Width, m_Height, 0);
            ::SetWindowPos(m_WinContext->GetRenderWindow(), HWND_TOPMOST, 0, 0, m_Width, m_Height, 0);
            return false;
        }

        Pause();

        if (!m_Fullscreen)
            RestoreWindow();

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
    catch (...)
    {
        TT_ERROR("NemoContext.cpp", "CreateRenderContext()", "critical");
        throw CNeMoContextException(2);
    }
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

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
    if (!drDesc)
    {
        if (m_CKContext)
            CKCloseContext(m_CKContext);
        m_CKContext = NULL;

        TT_ERROR("NemoContext.cpp", "FindScreenMode()", "VxDriverDesc ist NULL, critical");
        throw CNeMoContextException(5);
    }

#if CKVERSION == 0x13022002
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
#else
    XArray<VxDisplayMode> &dm = drDesc->DisplayModes;
    const int dmCount = dm.Size();
#endif
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
        return false;

    m_WinContext->UpdateWindows();
    m_WinContext->ShowWindows();
    CreateInterfaceSprite();
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

CKSpriteText *CNeMoContext::GetFrameRateSprite() const
{
    return m_FrameRateSprite;
}

CKSpriteText *CNeMoContext::GetMadeWithSprite() const
{
    return m_MadeWithSprite;
}

bool CNeMoContext::IsReseted() const
{
    return m_CKContext->IsReseted() == TRUE;
}

void CNeMoContext::DestroyMadeWithSprite()
{
    m_CKContext->DestroyObject(GetMadeWithSprite()->GetID());
}

void CNeMoContext::DestroyFrameRateSprite()
{
    m_CKContext->DestroyObject(GetFrameRateSprite()->GetID());
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
    try
    {
        m_PluginManager = CKGetPluginManager();
        m_PluginManager->ParsePlugins(dir);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

InterfaceManager *CNeMoContext::GetInterfaceManager()
{
    if (m_CKContext)
        return (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
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
    int driverBefore = m_DriverIndex;
    int screenModeBefore = m_ScreenModeIndex;
    bool fullscreenBefore = IsRenderFullScreen();

    m_DisplayChanged = true;
    m_DriverIndex = driver;

    if (!ApplyScreenMode(screenMode))
    {
        m_DriverIndex = driverBefore;
        ApplyScreenMode(screenModeBefore);
        if (IsRenderFullScreen())
            GoFullscreen();

        m_DisplayChanged = false;
        return false;
    }

    m_RenderContext->StopFullScreen();
    ::Sleep(10);
    if (fullscreenBefore && !m_RenderContext->IsFullScreen())
        GoFullscreen();
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