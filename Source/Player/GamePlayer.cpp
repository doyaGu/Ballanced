#include "GamePlayer.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "Splash.h"

#include "TT_InterfaceManager_RT/InterfaceManager.h"

#include "config.h"
#include "resource.h"

static bool CheckPrerequisite()
{
    char buffer[80];

    {
        SYSTEM_INFO sysinfo;
        ::GetSystemInfo(&sysinfo);

        TT_LOG("ProcessorCheck ... ");

        if (sysinfo.wProcessorArchitecture)
        {
            TT_LOG("No Intel architecture ... failed");
            ::MessageBoxA(NULL, "This program requires a PC with an Intel compatible processor!", "Error", MB_ICONERROR);
            return false;
        }
        if (sysinfo.dwProcessorType != PROCESSOR_INTEL_PENTIUM)
        {
            TT_LOG("No Intel processor ... failed");
            ::MessageBoxA(NULL, "This program requires a PC with an Intel Pentium compatible (or higher) processor!", "Error", MB_ICONERROR);
            return false;
        }

        TT_LOG("Intel CPU found ... OK");
    }

    {
        OSVERSIONINFOA osvi;
        ::ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        ::GetVersionExA(&osvi);

        TT_LOG("OS Check ... ");

        if (osvi.dwMajorVersion < 4)
        {
            TT_LOG("No supported OS ... failed");
            return false;
        }

        TT_LOG("Windows95/98 found ... OK");
    }

    {
        TT_LOG("Checking available memory ... ");

        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        ::GlobalMemoryStatusEx(&statex);

        sprintf(buffer, "memory usage: %ld percent\n", statex.dwMemoryLoad);
        TT_LOG(buffer);
        sprintf(buffer, "%llu total %sbytes of physical memory.\n", statex.ullTotalPhys / (1024 * 1024), "M");
        TT_LOG(buffer);
        sprintf(buffer, "%llu free %sbytes of physical memory.\n", statex.ullAvailPhys / (1024 * 1024), "M");
        TT_LOG(buffer);
        sprintf(buffer, "%llu total %sbytes of paging file.\n", statex.ullTotalPageFile / (1024 * 1024), "M");
        TT_LOG(buffer);
        sprintf(buffer, "%llu free %sbytes of paging file.\n", statex.ullAvailPageFile / (1024 * 1024), "M");
        TT_LOG(buffer);
        sprintf(buffer, "%llu total %sbytes of virtual memory.\n", statex.ullTotalVirtual / (1024 * 1024), "M");
        TT_LOG(buffer);
        sprintf(buffer, "%llu free %sbytes of virtual memory.\n", statex.ullAvailVirtual / (1024 * 1024), "M");
        TT_LOG(buffer);

        if (statex.ullAvailVirtual < 64000000)
        {
            TT_LOG("Memory check failed (below 64MB)!");
            ::MessageBoxA(NULL, "This program requires a PC with at least 64 MB RAM!", "Error", MB_ICONERROR);
            return false;
        }

        TT_LOG("Memory check successful!");
    }

    return true;
}

static void OnInitDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char buffer[256];
    UINT lbStrId;
    int drCount;
    VxDriverDesc *drDesc;
    VxDisplayMode *displayMode;

    CNeMoContext *context = CNeMoContext::GetInstance();

    drCount = context->GetRenderManager()->GetRenderDriverCount();
    if (drCount > 0)
    {
        for (int i = 0; i < drCount; ++i)
        {
            drDesc = context->GetRenderManager()->GetRenderDriverDescription(i);
            if (drDesc->IsHardware)
            {
                lbStrId = ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_ADDSTRING, 0, (LPARAM)drDesc->DriverName);
                ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETITEMDATA, lbStrId, i);
                if (i == context->GetDriver())
                    ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, lbStrId, 0);
            }
        }
    }

    drDesc = context->GetRenderManager()->GetRenderDriverDescription(context->GetDriver());
    for (int i = 0, j = 0; i < drDesc->DisplayModeCount; ++i, ++j)
    {
        displayMode = drDesc->DisplayModes;
        if (displayMode[i].Bpp > 8)
        {
            if (j == 0)
            {
                sprintf(buffer, "%d x %d x %d", displayMode[j].Width, displayMode[j].Height, displayMode[j].Bpp);
                lbStrId = SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)buffer);
                SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, lbStrId, i);
                if (i == context->GetScreenMode())
                {
                    SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, lbStrId, 0);
                    SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETTOPINDEX, lbStrId, 0);
                }
            }

            int k = 0;
            for (VxDisplayMode *displayModeNext = displayMode + 1;
                 displayMode[j].Width != displayModeNext->Width ||
                 displayMode[j].Height != displayModeNext->Height ||
                 displayMode[j].Bpp != displayModeNext->Bpp;
                 ++displayModeNext)
            {
                if (++k >= i)
                {
                    sprintf(buffer, "%d x %d x %d", displayMode[j].Width, displayMode[j].Height, displayMode[j].Bpp);
                    lbStrId = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)buffer);
                    ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, lbStrId, i);
                    if (i == context->GetScreenMode())
                    {
                        ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, lbStrId, 0);
                        ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETTOPINDEX, lbStrId, 0);
                    }
                }
            }
        }
    }
}

static BOOL CALLBACK FullscreenSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        OnInitDialog(hWnd, uMsg, wParam, lParam);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            BOOL fError = wParam;
            CNeMoContext *context = CNeMoContext::GetInstance();

            int curIdx = ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
            if (curIdx >= 0)
                context->SetDriver(SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, curIdx, 0));

            curIdx = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
            if (curIdx >= 0)
                fError = context->ApplyScreenMode(SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, curIdx, 0));

            CTTInterfaceManager *im = context->GetInterfaceManager();
            im->SetDriver(context->GetDriver());
            im->SetScreenMode(context->GetScreenMode());

            if (fError)
                EndDialog(hWnd, 1);

            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hWnd, 2);
            break;
        }
        break;
    }
    return FALSE;
}

CGamePlayer::CGamePlayer(CGameInfo *gameInfo, int n, HANDLE hMutex)
    : m_State(eInitial),
      m_hMutex(hMutex),
      m_Cleared(false),
      m_DataManager(),
      m_NeMoContext(),
      m_WinContext(),
      m_Stack(),
      m_Config(),
      m_Game()
{
    memset(m_RenderPath, 0, sizeof(m_RenderPath));
    memset(m_PluginPath, 0, sizeof(m_PluginPath));
    memset(m_ManagerPath, 0, sizeof(m_ManagerPath));
    memset(m_BehaviorPath, 0, sizeof(m_BehaviorPath));
    memset(m_Path, 0, sizeof(m_Path));
    memset(m_IniPath, 0, sizeof(m_IniPath));

    Construct();

    for (int i = 0; i < n; i++)
        m_DataManager.Save(&gameInfo[i]);
}

CGamePlayer::~CGamePlayer()
{
}

void CGamePlayer::Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc)
{
    m_State = eInitial;

    if (!CheckPrerequisite())
        return;

    CNeMoContext::RegisterInstance(&m_NeMoContext);
    m_Game.SetNeMoContext(&m_NeMoContext);

    int x = m_Config.posX;
    int y = m_Config.posY;

    if (!m_WinContext.Init(hInstance, lpfnWndProc, m_Config.fullscreen, m_Config.borderless, m_Config.resizable))
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "WinContext Initialization Failed");
        return;
    }

    switch (InitEngine())
    {
    case CK_OK:
        break;
    case CKERR_NODLLFOUND:
        ::MessageBoxA(NULL, "Necessary dll is not found", "Error", MB_OK);
        return;
    case CKERR_NORENDERENGINE:
        ::MessageBoxA(NULL, "No RenderEngine", "Error", MB_OK);
        return;
    case CKERR_INVALIDPARAMETER:
    {
        m_NeMoContext.SetDriver(0);
        m_NeMoContext.SetBPP(DEFAULT_BPP);
        m_NeMoContext.SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);

        if (!ReInitEngine())
        {
            if (::DialogBoxParamA(m_WinContext.GetAppInstance(), (LPCSTR)IDD_FULLSCREEN_SETUP, NULL, FullscreenSetup, 0) != 1)
                return;
            if (!ReInitEngine())
                return;
        }

        m_Config.bpp = m_NeMoContext.GetBPP();
        m_Config.driver = m_NeMoContext.GetDriver();
        m_Config.width = m_NeMoContext.GetWidth();
        m_Config.height = m_NeMoContext.GetHeight();
    }
    break;
    default:
        return;
    }

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetDriver(m_NeMoContext.GetDriver());
    im->SetScreenMode(m_NeMoContext.GetScreenMode());
    im->SetIniName(m_IniPath);
    im->SetRookie(m_Config.rookie);
    im->SetTaskSwitchEnabled(m_Config.taskSwitchEnabled);

    if (!m_Config.fullscreen)
        m_WinContext.SetPosition(x, y);

    m_WinContext.ShowWindows();
    m_WinContext.UpdateWindows();

    m_NeMoContext.Refresh();

    ::SendMessageA(m_WinContext.GetMainWindow(), TT_MSG_EXIT_TO_TITLE, NULL, NULL);
    m_WinContext.FocusMainWindow();

    m_State = eInitialized;
}

void CGamePlayer::Run()
{
    while (Step())
        continue;
}

bool CGamePlayer::Step()
{
    MSG msg;
    if (::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        if (!::TranslateAcceleratorA(msg.hwnd, m_WinContext.GetAccelTable(), &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }
    }

    m_NeMoContext.Update();

    return true;
}

void CGamePlayer::Done()
{
    m_Config.SaveToIni(m_IniPath);

    if (m_hMutex)
    {
        ::ReleaseMutex(m_hMutex);
        m_hMutex = NULL;
    }

    if (!m_Cleared)
    {
        m_NeMoContext.Cleanup();
        if (m_NeMoContext.RestoreWindow())
            m_NeMoContext.Refresh();

        m_NeMoContext.Shutdown();

        CGameInfo *gameInfo = m_Game.GetGameInfo();
        if (gameInfo && gameInfo->fileName)
        {
            if (!m_Stack.GetGameInfo(gameInfo->fileName))
                delete gameInfo;
            m_Game.SetGameInfo(NULL);
        }
        m_Stack.ClearAll();
    }

    m_Cleared = true;
}

bool CGamePlayer::LoadCMO(const char *filename)
{
    m_NeMoContext.Refresh();

    CGameInfo *gameInfoNow = m_Game.GetGameInfo();
    CGameInfo *gameInfo = m_Stack.GetGameInfo(filename);
    if (gameInfo)
    {
        m_Game.SetGameInfo(gameInfo);
    }
    else
    {
        gameInfo = m_Game.NewGameInfo();
        m_DataManager.Load(gameInfo, filename);
        if (gameInfoNow)
        {
            switch (gameInfoNow->type)
            {
            case 1:
                gameInfo->next = gameInfoNow;
                break;
            case 2:
                gameInfo->next = gameInfoNow->next;
                break;
            default:
                gameInfo->next = NULL;
                break;
            }
        }
        else
        {
            gameInfo->next = NULL;
        }
    }

    if (gameInfoNow && !m_Stack.GetGameInfo(gameInfoNow->fileName))
        m_Stack.Push(gameInfoNow);

    RegisterGameInfo();
    if (!m_Game.Load(m_Config))
        return false;

    ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));

    m_Game.Play();
    m_WinContext.FocusMainWindow();

    return true;
}

void CGamePlayer::OnDestroy()
{
    m_WinContext.GetPosition(m_Config.posX, m_Config.posY);
    m_NeMoContext.BroadcastCloseMessage();
    ::PostQuitMessage(0);
}

void CGamePlayer::OnSized()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext && !renderContext->IsFullScreen())
        m_NeMoContext.ResizeWindow();
}

void CGamePlayer::OnPaint()
{
    if (m_NeMoContext.GetRenderContext() && !m_NeMoContext.IsRenderFullscreen())
        m_NeMoContext.Render();
}

void CGamePlayer::OnClose()
{
}

LRESULT CGamePlayer::OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (im && !im->IsTaskSwitchEnabled())
        return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);

    if (wParam != WA_ACTIVE)
    {
        if (m_NeMoContext.GetCKContext())
        {
            if (firstDeActivate)
                wasPlaying = m_NeMoContext.IsPlaying();

            if (m_Config.pauseOnTaskSwitch)
                m_NeMoContext.Pause();

            if (m_NeMoContext.GetRenderContext() && m_NeMoContext.IsRenderFullscreen())
            {
                if (firstDeActivate)
                    wasFullscreen = true;
                m_NeMoContext.RestoreWindow();
                m_NeMoContext.MinimizeWindow();
            }
            else if (firstDeActivate)
            {
                wasFullscreen = false;
            }
        }
        firstDeActivate = false;
        return DefWindowProcA(hWnd, WM_ACTIVATEAPP, 0, lParam);
    }
    else
    {
        if (wasPlaying)
            m_NeMoContext.Play();

        if (wasFullscreen && !firstDeActivate)
            m_NeMoContext.GoFullscreen();

        firstDeActivate = true;
        return DefWindowProcA(hWnd, WM_ACTIVATEAPP, wParam, lParam);
    }
}

void CGamePlayer::OnSetCursor()
{
    if (IsInitialized() && !m_NeMoContext.IsPlaying())
        ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
}

void CGamePlayer::OnGetMinMaxInfo(LPMINMAXINFO lpmmi)
{
    if (lpmmi)
    {
        lpmmi->ptMinTrackSize.x = 400;
        lpmmi->ptMinTrackSize.y = 200;
    }
}

int CGamePlayer::OnKeyDown(UINT uKey)
{
    return 0;
}

int CGamePlayer::OnSysKeyDown(UINT uKey)
{
    // Manage system key (ALT + KEY)
    switch (uKey)
    {
    case VK_RETURN:
        // ALT + ENTER -> SwitchFullscreen
        m_NeMoContext.SwitchFullscreen();
        m_Config.fullscreen = m_NeMoContext.IsFullscreen();
        if (!m_Config.fullscreen)
            m_WinContext.SetPosition(m_Config.posX, m_Config.posY);
        break;

    case VK_F4:
        // ALT + F4 -> Quit the application
        OnDestroy();
        return 1;
    }
    return 0;
}

void CGamePlayer::OnCommand(UINT id, UINT code)
{
}

void CGamePlayer::OnExceptionCMO(WPARAM wParam, LPARAM lParam)
{
    m_NeMoContext.RestoreWindow();
    m_NeMoContext.Cleanup();
    TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnExceptionCMO()", "Exception in the CMO - Abort");
    OnDestroy();
}

void CGamePlayer::OnReturn(WPARAM wParam, LPARAM lParam)
{
    RegisterGameInfo();
    if (!m_Game.Load(m_Config))
        OnDestroy();
    m_Game.Play();

    m_WinContext.SetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
}

bool CGamePlayer::OnLoadCMO(WPARAM wParam, LPARAM lParam)
{
    return LoadCMO((const char *)wParam);
}

void CGamePlayer::OnExitToSystem(WPARAM wParam, LPARAM lParam)
{
    OnDestroy();
}

void CGamePlayer::OnExitToTitle(WPARAM wParam, LPARAM lParam)
{
}

LRESULT CGamePlayer::OnChangeScreenMode(WPARAM wParam, LPARAM lParam)
{
    if (!m_NeMoContext.ChangeScreenMode(lParam, wParam))
        return 0;

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im)
    {
        TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
        return 1;
    }
    im->SetDriver(m_NeMoContext.GetDriver());
    im->SetScreenMode(m_NeMoContext.GetScreenMode());

    m_Config.bpp = m_NeMoContext.GetBPP();
    m_Config.driver = m_NeMoContext.GetDriver();
    m_Config.width = m_NeMoContext.GetWidth();
    m_Config.height = m_NeMoContext.GetHeight();

    m_WinContext.FocusMainWindow();

    return 1;
}

void CGamePlayer::OnGoFullscreen()
{
    if (!m_NeMoContext.IsRenderFullscreen())
    {
        m_Config.fullscreen = true;
        m_NeMoContext.GoFullscreen();
        m_WinContext.FocusMainWindow();
        m_NeMoContext.Play();
        m_WinContext.FocusMainWindow();
    }
}

void CGamePlayer::OnStopFullscreen()
{
    if (m_NeMoContext.IsFullscreen() && m_NeMoContext.GetRenderContext()->IsFullScreen())
    {
        m_Config.fullscreen = false;
        m_NeMoContext.GetRenderContext()->StopFullScreen();
        m_WinContext.MinimizeWindow();
    }
}

LRESULT CGamePlayer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        OnDestroy();
        break;

    case WM_SIZE:
        OnSized();
        break;

    case WM_PAINT:
        OnPaint();
        break;

    case WM_CLOSE:
        OnClose();
        break;

    case WM_ACTIVATEAPP:
        return OnActivateApp(hWnd, uMsg, wParam, lParam);

    case WM_SETCURSOR:
        OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
        OnGetMinMaxInfo((LPMINMAXINFO)lParam);
        break;

    case WM_KEYDOWN:
        return OnKeyDown(wParam);

    case WM_SYSKEYDOWN:
        return OnSysKeyDown(wParam);

    case WM_COMMAND:
        OnCommand(LOWORD(wParam), HIWORD(wParam));
        break;

    case TT_MSG_NO_GAMEINFO:
        OnExceptionCMO(wParam, lParam);
        break;

    case TT_MSG_CMO_RESTART:
        OnReturn(wParam, lParam);
        break;

    case TT_MSG_CMO_LOAD:
        OnLoadCMO(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_SYS:
        OnExitToSystem(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_TITLE:
        OnExitToTitle(wParam, lParam);
        break;

    case TT_MSG_SCREEN_MODE_CHG:
        return OnChangeScreenMode(wParam, lParam);

    case TT_MSG_GO_FULLSCREEN:
        OnGoFullscreen();
        break;

    case TT_MSG_STOP_FULLSCREEN:
        OnStopFullscreen();
        return 1;

    default:
        break;
    }

    return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void CGamePlayer::Construct()
{
    char fullPath[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    char rootPath[512];

    ::GetModuleFileNameA(NULL, fullPath, MAX_PATH);
    _splitpath(fullPath, drive, dir, filename, NULL);
    strcpy(m_Path, "..\\");
    sprintf(m_IniPath, "%s%s%s.ini", drive, dir, filename);
    sprintf(rootPath, "%s%s%s", drive, dir, m_Path);

    TT_ERROR_OPEN(filename, rootPath, true);
    TT_LOG_OPEN(filename, rootPath, false);

    m_Config.langId = 1;
    m_Config.posX = 2147483647;
    m_Config.posY = 2147483647;
    m_Config.width = DEFAULT_WIDTH;
    m_Config.height = DEFAULT_HEIGHT;
    m_Config.bpp = DEFAULT_BPP;
    m_Config.driver = 0;
    m_Config.fullscreen = false;
    m_Config.borderless = false;
    m_Config.resizable = false;
    m_Config.unlockFramerate = false;
    m_Config.taskSwitchEnabled = true;
    m_Config.pauseOnTaskSwitch = false;
    m_Config.playerActive = false;
    m_Config.godmode = false;
    m_Config.debug = false;
    m_Config.rookie = false;

    m_Config.LoadFromIni(m_IniPath);
    m_Config.LoadFromCmdline(__argc, __argv);

    m_NeMoContext.SetScreen(&m_WinContext, m_Config.fullscreen, m_Config.driver, m_Config.bpp, m_Config.width, m_Config.height);
    m_WinContext.SetResolution(m_Config.width, m_Config.height);

    m_State = eInitialized;
}

int CGamePlayer::InitEngine()
{
    char drive[4];
    char fullpath[512];
    char buffer[MAX_PATH];
    char dir[MAX_PATH];

    {
        CSplash splash(m_WinContext.GetAppInstance());
        splash.Show();
    }

    if (!m_NeMoContext.StartUp())
        return CKERR_INVALIDPARAMETER;

    ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
    _splitpath(buffer, drive, dir, NULL, NULL);
    sprintf(fullpath, "%s%s%s", drive, dir, m_Path);
    if (strcmp(fullpath, "") == 0)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "Unable to set ProgPath");
        return CKERR_INVALIDPARAMETER;
    }
    m_NeMoContext.SetProgPath(fullpath);

    sprintf(m_PluginPath, "%s%s%s%s", drive, dir, m_Path, "Plugins");
    sprintf(m_RenderPath, "%s%s%s%s", drive, dir, m_Path, "RenderEngines");
    sprintf(m_ManagerPath, "%s%s%s%s", drive, dir, m_Path, "Managers");
    sprintf(m_BehaviorPath, "%s%s%s%s", drive, dir, m_Path, "BuildingBlocks");

    if (!LoadEngineDLL())
        return CKERR_INVALIDPARAMETER;
    if (!LoadStdDLL())
        return CKERR_INVALIDPARAMETER;

    return m_NeMoContext.Init();
}

bool CGamePlayer::ReInitEngine()
{
    m_State = eInitial;

    if (!m_NeMoContext.ReInit())
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::ReInitEngine()", "ReInit NemoContext");
        return false;
    }

    m_State = eInitialized;
    return true;
}

bool CGamePlayer::LoadEngineDLL()
{

    if (!m_NeMoContext.ParsePlugins(m_RenderPath) || _access(m_RenderPath, 0) == -1)
        return false;

    return true;
}

bool CGamePlayer::LoadStdDLL()
{
    if (_access(m_ManagerPath, 0) == -1 || !m_NeMoContext.ParsePlugins(m_ManagerPath))
    {
        m_NeMoContext.RestoreWindow();
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "manager parse error");
        return false;
    }

    if (_access(m_BehaviorPath, 0) == -1 || !m_NeMoContext.ParsePlugins(m_BehaviorPath))
    {
        m_NeMoContext.RestoreWindow();
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "behavior parse error");
        return false;
    }

    if (_access(m_PluginPath, 0) == -1 || !m_NeMoContext.ParsePlugins(m_PluginPath))
    {
        m_NeMoContext.RestoreWindow();
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "plugin parse error");
        return false;
    }

    return true;
}

void CGamePlayer::RegisterGameInfo()
{
    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetGameInfo(m_Game.GetGameInfo());
}