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
    : m_RenderContext(NULL),
      m_CKContext(NULL),
      m_FrameRateSprite(NULL),
      m_MadeWithSprite(NULL),
      m_RenderEnginePath("CK2_3D"),
      field_25C(0),
      m_IsCleared(false),
      m_DriverIndex(0),
      m_StartTime(1),
      m_ScreenModeIndex(-1),
      field_4C(82.0f),
      m_PluginManager(NULL),
      m_DisplayChanged(0)
{
    m_ProgPath[0] = '\0';
    SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

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
    m_FrameRateSprite->SetPosition(Vx2DVector(x, y), FALSE, FALSE, NULL);
}

void CNeMoContext::SetMadeWithSpritePosition(int x, int y)
{
    m_MadeWithSprite->SetPosition(Vx2DVector(x, y), FALSE, FALSE, NULL);
}

void CNeMoContext::SetFrameRateSpriteText(CKSTRING text)
{
    m_FrameRateSprite->SetText(text);
}

void CNeMoContext::SetMadeWithSpriteText(CKSTRING text)
{
    m_MadeWithSprite->SetText(text);
}

bool CNeMoContext::MoveFrameRateSpriteToLeftTop()
{
    m_MadeWithSprite->SetPosition(Vx2DVector((float)(m_Width * 0.5 - 60.0), (float)(m_Height * 0.5 - 15.0)), FALSE, FALSE, NULL);
    return true;
}

void CNeMoContext::SetStartTime(int time)
{
    m_StartTime = time;
}

void CNeMoContext::SetField0x25C(bool val)
{
    field_25C = val;
}

void CNeMoContext::SetCleared(bool clear)
{
    m_IsCleared = clear;
}

int CNeMoContext::GetStartTime() const
{
    return m_StartTime;
}

int CNeMoContext::GetField0x25C() const
{
    return field_25C;
}

bool CNeMoContext::IsCleared() const
{
    return m_IsCleared;
}

void CNeMoContext::SetDriverIndex(int idx)
{
    m_DriverIndex = idx;
}

bool CNeMoContext::SetScreenMode(int idx)
{
    VxDriverDesc *drDesc;
    VxDisplayMode *displayMode;

    m_ScreenModeIndex = idx;
    drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
    if (drDesc)
    {
        displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
        m_Width = displayMode->Width;
        m_Height = displayMode->Height;
        m_ColorBPP = displayMode->Bpp;
        return true;
    }
    return false;
}

void CNeMoContext::SetFullscreen(bool fullscreen)
{
    m_Fullscreen = fullscreen;
}

void CNeMoContext::SetColorBPP(int bpp)
{
    m_ColorBPP = bpp;
}

void CNeMoContext::SetField0x30(int val)
{
    field_30 = val;
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

void CNeMoContext::SetField0x254(int val)
{
    field_254 = val;
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

int CNeMoContext::GetColorBPP() const
{
    return m_ColorBPP;
}

int CNeMoContext::GetField0x30() const
{
    return field_30;
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

int CNeMoContext::GetField0x254() const
{
    return field_254;
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
    if (CKStartUp())
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

void CNeMoContext::ShowWindow()
{
    if (m_RenderContext &&
        IsRenderFullScreen() &&
        !m_RenderContext->IsFullScreen())
    {
        m_WinContext->GetMainWindow();
        ShowWindow();
    }
}

CKERROR CNeMoContext::Reset()
{
    return m_CKContext->Reset();
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags = CK_RENDER_USECURRENTSETTINGS)
{
    return m_RenderContext->Render(flags);
}

void CNeMoContext::HideFrameRateSprite()
{
    m_FrameRateSprite->Show(CKHIDE);
}

void CNeMoContext::ShowFrameRateSprite()
{
    m_FrameRateSprite->Show(CKSHOW);
}

void CNeMoContext::HideMadeWithSprite()
{
    m_MadeWithSprite->Show(CKHIDE);
}

void CNeMoContext::ShowMadeWithSprite()
{
    m_MadeWithSprite->Show(CKSHOW);
}

void CNeMoContext::Cleanup()
{
    Pause();
    Reset();
    m_CKContext->ClearAll();
}

void CNeMoContext::Shutdown()
{
    if (m_RenderContext)
    {
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
}

void CNeMoContext::GoFullscreen()
{
    VxDriverDesc *drDesc;
    VxDisplayMode *displayMode;

    if (m_RenderContext)
    {
        if (m_ScreenModeIndex >= 0)
        {
            drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
            if (drDesc)
            {
                displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
                m_RenderContext->GoFullScreen(displayMode->Width,
                                              displayMode->Height,
                                              displayMode->Bpp,
                                              m_DriverIndex,
                                              0);
                ::SetFocus(m_WinContext->GetMainWindow());
            }
        }
        else
        {
            m_RenderContext->GoFullScreen(m_Width,
                                          m_Height,
                                          m_ColorBPP,
                                          m_DriverIndex,
                                          0);
            ::SetFocus(m_WinContext->GetMainWindow());
        }
    }
}

void CNeMoContext::Process()
{
    float timeBeforeRender, timeBeforeProcess;

    try
    {
        if (IsPlaying())
        {
            if (IsCleared() && 0 < m_StartTime && m_StartTime < ::GetTickCount())
            {
                HideMadeWithSprite();
                m_StartTime = 0;
            }
            timeBeforeRender = 0.0f;
            timeBeforeProcess = 0.0f;
            m_TimeManager->GetTimeToWaitForLimits(timeBeforeRender, timeBeforeProcess);
            if (timeBeforeProcess <= 0)
            {
                m_TimeManager->ResetChronos(FALSE, TRUE);
                m_CKContext->Process();
            }
            if (timeBeforeRender <= 0)
            {
                m_TimeManager->ResetChronos(TRUE, FALSE);
                m_RenderContext->Render(CK_RENDER_USECURRENTSETTINGS);
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
        if (IsCleared() && IsReseted())
        {
            m_StartTime = ::GetTickCount() + 3000;
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
            if (IsCleared() && IsReseted())
            {
                m_StartTime = ::GetTickCount() + 3000;
                HideMadeWithSprite();
            }
            Play();
        }
        Pause();
        break;
    }
}

void CNeMoContext::SwitchFullscreen()
{
    VxDriverDesc *drDesc;
    VxDisplayMode *displayMode;

    if (m_RenderContext)
    {
        if (IsRenderFullScreen())
        {
            RestoreWindow();
        }
        else if (m_ScreenModeIndex >= 0)
        {
            drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
            if (drDesc)
            {
                displayMode = &drDesc->DisplayModes[m_ScreenModeIndex];
                m_RenderContext->GoFullScreen(displayMode->Width,
                                              displayMode->Height,
                                              displayMode->Bpp,
                                              m_DriverIndex,
                                              0);
                ::ShowWindow(m_WinContext->GetMainWindow(), SW_SHOWMINNOACTIVE);
            }
        }
        else
        {
            m_RenderContext->GoFullScreen(m_Width,
                                          m_Height,
                                          m_ColorBPP,
                                          m_DriverIndex,
                                          0);
            ::ShowWindow(m_WinContext->GetMainWindow(), SW_SHOWMINNOACTIVE);
        }
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
    try
    {
        if (renderEnginePluginIdx == -1)
        {
            TT_ERROR("NemoContext.cpp", "Init()", "Found no render-engine! (Driver? Critical!!!)");
            throw CNeMoContextException(1);
        }

        CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), renderEnginePluginIdx, NULL);
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
        {
            return false;
        }

        m_WinContext->UpdateWindows();
        m_WinContext->ShowWindows();

        CreateInterfaceSprite();

        return true;
    }
    catch (const CNeMoContextException &nmce)
    {
        if (nmce.error == 5)
        {
            TT_ERROR("NemoContext.cpp", "Init()", "Problems with the render driver description");
        }
        throw CNeMoContextException(1);
    }
}

void CNeMoContext::CreateInterfaceSprite()
{
    try
    {
        m_FrameRateSprite = (CKSpriteText *)m_CKContext->CreateObject(CKCID_SPRITETEXT,
                                                                      "FrameRateSprite",
                                                                      CK_OBJECTCREATION_NONAMECHECK,
                                                                      NULL);
        m_FrameRateSprite->Create(60, 20, 32, 0);
        m_FrameRateSprite->SetTransparent(FALSE);
        m_FrameRateSprite->SetFont("Arial", 8, 400, FALSE, FALSE);
        m_FrameRateSprite->SetBackgroundColor(0);
        m_FrameRateSprite->SetTextColor(0xFFFFFF);
        m_FrameRateSprite->SetPosition(Vx2DVector(350.0f, 225.0f), FALSE, FALSE, NULL);
        m_FrameRateSprite->Show(CKHIDE);

        m_MadeWithSprite = (CKSpriteText *)m_CKContext->CreateObject(CKCID_SPRITETEXT,
                                                                     "MadeWithSprite",
                                                                     CK_OBJECTCREATION_NONAMECHECK,
                                                                     NULL);
        m_MadeWithSprite->Create(m_Width, m_Height, 32, 0);
        m_MadeWithSprite->SetTransparent(FALSE);
        m_MadeWithSprite->SetFont("Arial", 14, 400, FALSE, FALSE);
        m_MadeWithSprite->SetBackgroundColor(0xFFFFFF);
        m_MadeWithSprite->SetTextColor(0xFFFF00);
        m_MadeWithSprite->SetZOrder(2000000);
        m_MadeWithSprite->Show(CKHIDE);

        if (!m_RenderContext)
        {
            throw CNeMoContextException(2);
        }

        m_RenderContext->AddObject(m_MadeWithSprite);
        m_RenderContext->AddObject(m_FrameRateSprite);
        m_FrameRateSprite->SetPosition(Vx2DVector(0.0f, 0.0f), FALSE, FALSE, NULL);
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
        m_RenderContext = renderManager->CreateRenderContext(m_WinContext->GetMainWindow(), m_DriverIndex, &rect, FALSE, -1, -1, -1, 0);
        if (!m_RenderContext)
        {
            return false;
        }

        Play();
        if (IsFullscreen() && m_RenderContext->GoFullScreen(m_Width, m_Height, m_ColorBPP, m_DriverIndex, 0))
        {
            if (!RestoreWindow())
            {
                TT_ERROR("NemoContext.cpp", "CreateRenderContext()", "WindowMode cannot be changed");
                throw CNeMoContextException(2);
            }

            ::SetWindowPos(m_WinContext->GetMainWindow(), (HWND)-1, 0, 0, m_Width, m_Height, 0);
            ::SetWindowPos(m_WinContext->GetRenderWindow(), (HWND)-1, 0, 0, m_Width, m_Height, 0);
            return false;
        }

        Pause();
        if (!IsFullscreen())
        {
            RestoreWindow();
        }
        ::SetFocus(m_WinContext->GetMainWindow());

        m_RenderContext->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
        m_RenderContext->Clear(CK_RENDER_BACKGROUNDSPRITES, 0);
        m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES, 0);
        m_RenderContext->Clear(CK_RENDER_USECAMERARATIO, 0);
        m_RenderContext->Clear(CK_RENDER_CLEARZ, 0);
        m_RenderContext->Clear(CK_RENDER_CLEARBACK, 0);
        m_RenderContext->Clear(CK_RENDER_DOBACKTOFRONT, 0);
        m_RenderContext->Clear(CK_RENDER_DEFAULTSETTINGS, 0);
        m_RenderContext->Clear(CK_RENDER_CLEARVIEWPORT, 0);
        m_RenderContext->Clear(CK_RENDER_FOREGROUNDSPRITES, 0);
        m_RenderContext->Clear(CK_RENDER_USECAMERARATIO, 0);
        m_RenderContext->Clear(CK_RENDER_WAITVBL, 0);
        m_RenderContext->Clear(CK_RENDER_PLAYERCONTEXT, 0);
        m_RenderContext->Clear(CK_RENDER_USECURRENTSETTINGS, 0);

        m_RenderContext->SetClearBackground(TRUE);
        m_RenderContext->BackToFront(CK_RENDER_USECURRENTSETTINGS);
        m_RenderContext->SetClearBackground(TRUE);
        m_RenderContext->Clear(CK_RENDER_USECURRENTSETTINGS, 0);

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
    if (m_RenderEnginePath.Length() == 0)
    {
        return -1;
    }

    char filename[512];
    const int pluginCount = m_PluginManager->GetPluginCount(CKPLUGIN_RENDERENGINE_DLL);
    for (int i = 0; i < pluginCount; ++i)
    {
        CKPluginEntry* plugin = m_PluginManager->GetPluginInfo(CKPLUGIN_RENDERENGINE_DLL, i);
        if (!plugin)
        {
            return -1;
        }

        char* dllname = m_PluginManager->GetPluginDllInfo(plugin->m_PluginDllIndex)->m_DllFileName.Str();
        if (!dllname)
        {
            return -1;
        }

        _splitpath(dllname, NULL, NULL, filename, NULL);
        if (!_strnicmp(m_RenderEnginePath.Str(), filename, strlen(filename)))
        {
            return i;
        }
    }
    return -1;
}

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc;

    drDesc = m_RenderManager->GetRenderDriverDescription(m_DriverIndex);
    if (!drDesc)
    {
        if (m_CKContext)
        {
            CKCloseContext(m_CKContext);
        }
        m_CKContext = NULL;

        TT_ERROR("NemoContext.cpp", "FindScreenMode()", "VxDriverDesc ist NULL, critical");
        throw CNeMoContextException(5);
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
    if (dmCount > 0)
    {
        for (int i = 0; i < dmCount; ++i)
        {
            if (dm[i].Width == m_Width &&
                dm[i].Height == m_Height &&
                dm[i].Bpp == m_ColorBPP)
            {
                m_ScreenModeIndex = i;
            }
        }
    }

    return m_ScreenModeIndex >= 0;
}

bool CNeMoContext::ReInit()
{
    if (!FindScreenMode() || !m_RenderContext || !CreateRenderContext())
    {
        return false;
    }

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
    m_ColorBPP = bpp;
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
    m_ColorBPP = bpp;
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
    m_CKContext->DestroyObject(GetMadeWithSprite()->m_ID, 0, NULL);
}

void CNeMoContext::DestroyFrameRateSprite()
{
    m_CKContext->DestroyObject(GetFrameRateSprite()->m_ID, 0, NULL);
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
    CK_LOAD_FLAGS flags,
    CKGUID *readerGuid)
{
    return m_CKContext->Load(filename, liste, CK_LOAD_DEFAULT, NULL);
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

CTTInterfaceManager *CNeMoContext::GetInterfaceManager()
{
    if (m_CKContext)
    {
        return (CTTInterfaceManager *)m_CKContext->GetManagerByGuid(CKGUID(0x30833801, 0x6DEE620D));
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

bool CNeMoContext::SwitchScreenMode(int driver, int screenMode)
{
    bool isRenderFullscreenBefore = IsRenderFullScreen();
    int screenModeBefore = GetScreenModeIndex();
    int driverBefore = m_DriverIndex;

    m_DisplayChanged = true;
    m_DriverIndex = driver;

    if (SetScreenMode(screenMode))
    {
        GetRenderContext()->StopFullScreen();
        ::Sleep(10);
        if (isRenderFullscreenBefore && !GetRenderContext()->IsFullScreen())
        {
            GoFullscreen();
        }
        ::Sleep(10);
        ::SetFocus(m_WinContext->GetMainWindow());
        m_DisplayChanged = false;
        return true;
    }
    else
    {
        m_DriverIndex = driverBefore;
        SetScreenMode(screenModeBefore);
        if (IsRenderFullScreen())
        {
            GoFullscreen();
        }
        m_DisplayChanged = false;
        return false;
    }
}

bool CNeMoContext::SendMessageWindowCloseToAll()
{
    return m_MessageManager->SendMessageBroadcast(m_MsgWindowClose, CKCID_BEOBJECT, NULL) != NULL;
}

void CNeMoContext::AddMessageWindowClose()
{
    m_MsgWindowClose = m_MessageManager->AddMessageType("WM_CLOSE");
}

CKContext *CNeMoContext::GetCKContext()
{
    return m_CKContext;
}