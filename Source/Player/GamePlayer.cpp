#include "StdAfx.h"

#include "GamePlayer.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"
#include "XString.h"
#include "TerraTools.h"

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "ResDll.h"
#include "Splash.h"
#include "TT_InterfaceManager_RT/InterfaceManager.h"

#define MAXOPTIONS 32

extern RESOURCEMAP g_ResMap;

struct PlayerOptions
{
    int width;
    int height;
    int bpp;
    int driver;
    bool fullscreen;
    bool taskSwitchEnabled;
    bool pauseOnTaskSwitch;
};

static inline bool IsNoSettingsInIni()
{
    return (g_ResMap.fKey & 2) == 2;
}

static inline bool IsUsingCommandLine()
{
    return (g_ResMap.fKey & 4) == 4;
}

static char **GetArgs(int *argc)
{
    int optind = 0;
    static char *argv[MAXOPTIONS] = {0};

    char *cmdline = ::GetCommandLineA();
    if (!cmdline || strcmp(cmdline, "") == 0)
    {
        return NULL;
    }

    char *progend = NULL;
    if (cmdline[0] == '\"')
    {
        progend = strchr(cmdline + 1, '\"');
    }
    else
    {
        progend = strchr(cmdline, ' ');
        if (!progend)
        {
            argv[optind] = cmdline;
            ++optind;
            *argc = optind;
            return argv;
        }
    }

    *progend = '\0';
    if (cmdline[0] == '\"')
    {
        char *sep = strrchr(cmdline, '\\');
        if (sep)
        {
            argv[optind] = sep + 1;
        }
        else
        {
            argv[optind] = cmdline + 1;
        }
        ++optind;
        ++progend;
    }
    else
    {
        argv[optind] = cmdline;
        ++optind;
        *progend = ' ';
    }

    char *optstart = strchr(progend, ' ');
    if (!optstart)
    {
        argv[optind] = cmdline;
        ++optind;
        *argc = optind;
        return argv;
    }

    char *token = strtok(optstart, " ");
    while (token)
    {
        while (isspace(*token))
        {
            ++token;
        }
        argv[optind++] = token;
        token = strtok(NULL, " ");
    }

    *argc = optind;
    return argv;
}

static bool ParseCommandLine(PlayerOptions &options)
{
    int argc = 1;
    char **argv = ::GetArgs(&argc);
    if (!argv || argc == 1)
    {
        return false;
    }

    int ch;
    while ((ch = getopt(argc, argv, "f::d::p::w::h::b::v::")) != -1)
    {
        switch (ch)
        {
        case 'f':
            options.fullscreen = true;
            options.taskSwitchEnabled = false;
            break;
        case 'd':
            options.taskSwitchEnabled = false;
            break;
        case 'p':
            options.pauseOnTaskSwitch = true;
        case 'w':
            if (optarg)
            {
                options.width = atoi(optarg);
            }
            break;
        case 'h':
            if (optarg)
            {
                options.height = atoi(optarg);
            }
            break;
        case 'b':
            if (optarg)
            {
                options.bpp = atoi(optarg);
            }
            break;
        case 'v':
            if (optarg)
            {
                options.driver = atoi(optarg);
            }
            break;
        default:
            return false;
        }
    }

    return true;
}

static bool IniSetBPPAndDriver(int bpp, int driver)
{
    if (bpp == 32)
        g_ResMap.dwVideoDriverSetting = (driver << 16) | 1;
    else if (bpp == 16)
        g_ResMap.dwVideoDriverSetting = (driver << 16) | 0;

    char buffer[64];
    sprintf(buffer, "%d", g_ResMap.dwVideoDriverSetting);
    return ::WritePrivateProfileStringA("Settings", g_ResMap.videoDriver, buffer, g_ResMap.pathSetting);
}

static bool IniSetFullscreen(bool fullscreen)
{
    g_ResMap.fullScreenSetting = fullscreen;

    char buffer[64];
    sprintf(buffer, "%d", g_ResMap.fullScreenSetting);
    return ::WritePrivateProfileStringA("Settings", g_ResMap.fullScreen, buffer, g_ResMap.pathSetting);
}

static bool IniSetResolution(int width, int height)
{
    g_ResMap.dwVideoModeSetting = (width << 16) | height;

    char buffer[64];
    sprintf(buffer, "%d", g_ResMap.dwVideoModeSetting);
    return ::WritePrivateProfileStringA("Settings", g_ResMap.videoMode, buffer, g_ResMap.pathSetting);
}

static bool IniGetBPPAndDriver(int *bpp, int *driver)
{
    DWORD dwVideoDriver = ::GetPrivateProfileIntA("Settings", g_ResMap.videoDriver, -1, g_ResMap.pathSetting);
    if (dwVideoDriver == -1)
    {
        return false;
    }

    *bpp = (dwVideoDriver & 1) != 0 ? 32 : 16;
    *driver = dwVideoDriver >> 16;
    g_ResMap.dwVideoDriverSetting = dwVideoDriver;

    return true;
}

static bool IniGetFullScreen()
{
    return g_ResMap.fullScreenSetting == TRUE;
}

static int IniGetWidth()
{
    return HIWORD(g_ResMap.dwVideoModeSetting);
}

static int IniGetHeight()
{
    return LOWORD(g_ResMap.dwVideoModeSetting);
}

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
            ::MessageBoxA(NULL, g_ResMap.needIntelPC, "Error", MB_ICONERROR);
            return false;
        }
        if (sysinfo.dwProcessorType != PROCESSOR_INTEL_PENTIUM)
        {
            TT_LOG("No Intel processor ... failed");
            ::MessageBoxA(NULL, g_ResMap.needPentiumCPU, "Error", MB_ICONERROR);
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

        if (statex.ullAvailVirtual < g_ResMap.dwMinMem)
        {
            TT_LOG("Memory check failed (below 64MB)!");
            ::MessageBoxA(NULL, g_ResMap.needMEM64MB, "Error", MB_ICONERROR);
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

    drCount = CNeMoContext::GetInstance()->GetRenderManager()->GetRenderDriverCount();
    if (drCount > 0)
    {
        for (int i = 0; i < drCount; ++i)
        {
            drDesc = CNeMoContext::GetInstance()->GetRenderManager()->GetRenderDriverDescription(i);
            if (drDesc->IsHardware)
            {
                lbStrId = ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_ADDSTRING, 0, (LPARAM)drDesc->DriverName);
                ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETITEMDATA, lbStrId, i);
                if (i == CNeMoContext::GetInstance()->GetDriverIndex())
                {
                    ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, lbStrId, 0);
                }
            }
        }
    }

    drDesc = CNeMoContext::GetInstance()->GetRenderManager()->GetRenderDriverDescription(CNeMoContext::GetInstance()->GetDriverIndex());
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
                if (i == CNeMoContext::GetInstance()->GetScreenMode())
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
                    if (i == CNeMoContext::GetInstance()->GetScreenMode())
                    {
                        ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, lbStrId, 0);
                        ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETTOPINDEX, lbStrId, 0);
                    }
                }
            }
        }
    }
}

static BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            int curIdx = ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
            if (curIdx >= 0)
            {
                CNeMoContext::GetInstance()->SetDriverIndex(SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, curIdx, 0));
            }

            curIdx = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
            if (curIdx >= 0)
            {
                fError = CNeMoContext::GetInstance()->ApplyScreenMode(SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, curIdx, 0));
            }

            CTTInterfaceManager *im = CNeMoContext::GetInstance()->GetInterfaceManager();
            if (!im)
            {
                TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
                return TRUE;
            }
            im->SetDriverIndex(CNeMoContext::GetInstance()->GetDriverIndex());
            im->SetScreenModeIndex(CNeMoContext::GetInstance()->GetScreenMode());

            if (fError)
            {
                EndDialog(hWnd, 1);
            }
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hWnd, 2);
        }
    }
    return FALSE;
}

CGamePlayer::CGamePlayer(CGameInfo *gameInfo, int n, bool defaultSetting, HANDLE hMutex, bool rookie)
    : m_State(eInitial),
      m_hMutex(hMutex),
      m_DefaultSetting(defaultSetting),
      m_Cleared(false),
      m_DataManager(),
      m_NeMoContext(),
      m_WinContext(),
      m_Stack(defaultSetting),
      m_Game(),
      m_IsRookie(rookie),
      m_TaskSwitchEnabled(true),
      m_PauseOnTaskSwitch(false)
{
    memset(m_RenderPath, 0, sizeof(m_RenderPath));
    memset(m_PluginPath, 0, sizeof(m_PluginPath));
    memset(m_ManagerPath, 0, sizeof(m_ManagerPath));
    memset(m_BehaviorPath, 0, sizeof(m_BehaviorPath));
    memset(m_Path, 0, sizeof(m_Path));

    Construct();

    for (int i = 0; i < n; i++)
    {
        m_DataManager.Save(&gameInfo[i]);
    }
}

CGamePlayer::~CGamePlayer()
{
}

void CGamePlayer::Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc)
{
    m_State = eInitial;

    if (!CheckPrerequisite())
    {
        return;
    }

    CNeMoContext::RegisterInstance(&m_NeMoContext);
    m_Game.SetNeMoContext(&m_NeMoContext);

    if (!m_WinContext.Init(hInstance, lpfnWndProc, m_NeMoContext.IsFullscreen()))
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "WinContext Initilizaition Failed");
        return;
    }

    if (!InitEngine())
    {
        bool settingChanged = false;
        bool engineReinitialized = false;

        if (m_DefaultSetting)
        {
            m_NeMoContext.SetDriverIndex(0);
            m_NeMoContext.SetBPP(DEFAULT_BPP);
            m_NeMoContext.SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
            engineReinitialized = ReInitEngine();
            settingChanged = true;
        }

        if (!m_DefaultSetting || !engineReinitialized)
        {
            if (::DialogBoxParamA(m_WinContext.GetAppInstance(), (LPCSTR)IDD_FULLSCREEN_SETUP, NULL, DialogProc, 0) != 1)
            {
                return;
            }

            if (!ReInitEngine())
            {
                return;
            }
            settingChanged = true;
        }

        if (settingChanged)
        {
            IniSetBPPAndDriver(m_NeMoContext.GetBPP(), m_NeMoContext.GetScreenMode());
            IniSetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
        }
    }

    m_WinContext.ShowWindows();
    m_WinContext.UpdateWindows();

    m_NeMoContext.Refresh();

    ::SendMessageA(m_WinContext.GetMainWindow(), TT_MSG_EXIT_TO_TITLE, NULL, NULL);
    ::SetFocus(m_WinContext.GetMainWindow());

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
    BOOL bRet;

    if (m_NeMoContext.IsPlaying())
    {
        bRet = ::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
    }
    else
    {
        bRet = ::GetMessageA(&msg, NULL, 0, 0);
    }

    if (!bRet)
    {
        m_NeMoContext.Update();
        return true;
    }

    if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
    {
        return false;
    }

    HACCEL hAccel = m_WinContext.GetAccelTable();
    if (!::TranslateAcceleratorA(msg.hwnd, hAccel, &msg))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
    }
    return true;
}

void CGamePlayer::Done()
{
    if (m_hMutex)
    {
        ::ReleaseMutex(m_hMutex);
        m_hMutex = NULL;
    }

    if (!m_Cleared)
    {
        m_NeMoContext.Cleanup();
        if (m_NeMoContext.RestoreWindow())
        {
            m_NeMoContext.Refresh();
        }

        m_NeMoContext.Shutdown();

        CGameInfo *gameInfo = m_Game.GetGameInfo();
        if (gameInfo && gameInfo->fileName)
        {
            if (!m_Stack.GetGameInfo(gameInfo->fileName))
            {
                delete gameInfo;
            }
            m_Game.SetGameInfo(NULL);
        }
        m_Stack.ClearAll();
    }

    m_Cleared = true;
    ::PostQuitMessage(0);
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
            if (gameInfoNow->type == 1)
            {
                gameInfo->next = gameInfoNow;
            }
            else if (gameInfoNow->type == 2)
            {
                gameInfo->next = gameInfoNow->next;
            }
            else
            {
                gameInfo->next = NULL;
            }
        }
        else
        {
            gameInfo->next = NULL;
        }
    }

    if (gameInfoNow && !m_Stack.GetGameInfo(gameInfoNow->fileName))
    {
        m_Stack.Push(gameInfoNow);
    }

    if (!RegisterGameInfoToInterfaceManager())
    {
        return false;
    }

    if (!m_Game.Load())
    {
        return false;
    }
    ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));

    m_Game.Play();
    ::SetFocus(m_WinContext.GetMainWindow());

    return true;
}

void CGamePlayer::Play()
{
    m_NeMoContext.Play();
}

void CGamePlayer::Pause()
{
    m_NeMoContext.Pause();
}

void CGamePlayer::Reset()
{
    m_NeMoContext.Reset();
    m_NeMoContext.Play();
}

void CGamePlayer::OnDestroy()
{
    Done();
}

void CGamePlayer::OnSized()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext && !renderContext->IsFullScreen())
    {
        RECT rect;
        ::GetClientRect(m_WinContext.GetMainWindow(), &rect);
        ::SetWindowPos(m_WinContext.GetRenderWindow(), NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER);
        renderContext->Resize();
    }
}

void CGamePlayer::OnPaint()
{
    if (m_NeMoContext.GetRenderContext() && !m_NeMoContext.IsRenderFullscreen())
    {
        m_NeMoContext.Render();
    }
}

void CGamePlayer::OnClose()
{
    m_NeMoContext.BroadcastCloseMessage();
}

void CGamePlayer::OnActivateApp(WPARAM wParam, LPARAM lParam)
{
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im || !m_NeMoContext.GetInterfaceManager()->IsTaskSwitchEnabled())
    {
        return;
    }

    if (wParam != WA_ACTIVE)
    {
        if (m_NeMoContext.GetCKContext())
        {
            if (firstDeActivate)
            {
                wasPlaying = m_NeMoContext.IsPlaying();
            }
            if (m_PauseOnTaskSwitch)
            {
                m_NeMoContext.Pause();
            }
            if (m_NeMoContext.GetRenderContext() && m_NeMoContext.IsRenderFullscreen())
            {
                if (firstDeActivate)
                {
                    wasFullscreen = true;
                }
                m_NeMoContext.SwitchFullscreen();
                m_NeMoContext.MinimizeWindow();
            }
            else if (firstDeActivate)
            {
                wasFullscreen = false;
            }
        }
        firstDeActivate = false;
    }
    else
    {
        if (wasPlaying)
        {
            m_NeMoContext.Play();
        }
        if (wasFullscreen && !firstDeActivate)
        {
            m_NeMoContext.GoFullscreen();
        }
        firstDeActivate = true;
    }
}

void CGamePlayer::OnSetCursor()
{
    if (IsInitialized() && !m_NeMoContext.IsPlaying())
    {
        ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
    }
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
        break;

    case VK_F4:
        // ALT + F4 -> Quit the application
        Done();
        return 1;
    }
    return 0;
}

void CGamePlayer::OnCommand(UINT id, UINT code)
{
}

void CGamePlayer::OnMouseClick(UINT uMsg)
{
    if (!m_NeMoContext.GetRenderContext())
    {
        return;
    }

    int msg = (uMsg == WM_LBUTTONDOWN) ? m_NeMoContext.GetClickMessage() : m_NeMoContext.GetDoubleClickMessage();

    POINT pt;
    GetCursorPos(&pt);
    if (!m_NeMoContext.IsRenderFullscreen())
    {
        ::ScreenToClient(m_WinContext.GetRenderWindow(), &pt);
    }

    CKPOINT ckpt = {pt.x, pt.y};
    CKPICKRESULT res;
    CKObject *obj = m_NeMoContext.GetRenderContext()->Pick(ckpt, &res, FALSE);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
    {
        m_NeMoContext.SendMessageSingle(msg, (CKBeObject *)obj);
    }
    if (res.Sprite)
    {
        CKObject *sprite = m_NeMoContext.GetObject(res.Sprite);
        if (sprite)
        {
            m_NeMoContext.SendMessageSingle(msg, (CKBeObject *)sprite);
        }
    }
}

void CGamePlayer::OnExceptionCMO(WPARAM wParam, LPARAM lParam)
{
    m_NeMoContext.RestoreWindow();
    m_NeMoContext.Cleanup();
    TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnExceptionCMO()", "Exception in the CMO - Abort");
    ::PostQuitMessage(-1);
}

void CGamePlayer::OnReturn(WPARAM wParam, LPARAM lParam)
{
    if (!RegisterGameInfoToInterfaceManager())
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnReturn()", "Unable to register gameInfo");
    }

    if (!m_Game.Load())
    {
        Done();
    }
    m_Game.Play();

    m_WinContext.SetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
}

bool CGamePlayer::OnLoadCMO(WPARAM wParam, LPARAM lParam)
{
    return LoadCMO((const char *)wParam);
}

void CGamePlayer::OnExitToSystem(WPARAM wParam, LPARAM lParam)
{
    Done();
}

void CGamePlayer::OnExitToTitle(WPARAM wParam, LPARAM lParam)
{
}

void CGamePlayer::OnScreenModeChanged(WPARAM wParam, LPARAM lParam)
{
    m_NeMoContext.ChangeScreenMode(lParam, wParam);

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im)
    {
        TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
    }
    else
    {
        im->SetDriverIndex(m_NeMoContext.GetDriverIndex());
        im->SetScreenModeIndex(m_NeMoContext.GetScreenMode());
    }

    IniSetBPPAndDriver(m_NeMoContext.GetBPP(), m_NeMoContext.GetDriverIndex());
    IniSetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());

    ::SetFocus(m_WinContext.GetMainWindow());
}

void CGamePlayer::OnGoFullscreen()
{
    if (!m_NeMoContext.IsRenderFullscreen())
    {
        m_NeMoContext.GoFullscreen();
        ::SetFocus(m_WinContext.GetMainWindow());
        m_NeMoContext.Play();
        ::SetFocus(m_WinContext.GetMainWindow());
    }
}

void CGamePlayer::OnStopFullscreen()
{
    if (m_NeMoContext.IsFullscreen() && m_NeMoContext.GetRenderContext()->IsFullScreen())
    {
        m_NeMoContext.GetRenderContext()->StopFullScreen();
        ::ShowWindow(m_WinContext.GetMainWindow(), SW_MINIMIZE);
    }
}

LRESULT CGamePlayer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        Done();
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
        OnActivateApp(wParam, lParam);
        break;

    case WM_SETCURSOR:
        OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
        OnGetMinMaxInfo((LPMINMAXINFO)lParam);
        break;

    case WM_KEYDOWN:
        OnKeyDown(wParam);
        break;

    case WM_SYSKEYDOWN:
        OnSysKeyDown(wParam);
        break;

    case WM_COMMAND:
        OnCommand(LOWORD(wParam), HIWORD(wParam));
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        OnMouseClick(uMsg);
        break;

    case TT_MSG_NO_GAMEINFO:
        OnExceptionCMO(wParam, lParam);
        return TRUE;

    case TT_MSG_CMO_RESTART:
        OnReturn(wParam, lParam);
        return TRUE;

    case TT_MSG_CMO_LOAD:
        OnLoadCMO(wParam, lParam);
        return TRUE;

    case TT_MSG_EXIT_TO_SYS:
        OnExitToSystem(wParam, lParam);
        return TRUE;

    case TT_MSG_EXIT_TO_TITLE:
        OnExitToTitle(wParam, lParam);
        return TRUE;

    case TT_MSG_SCREEN_MODE_CHG:
        OnScreenModeChanged(wParam, lParam);
        return TRUE;

    case TT_MSG_GO_FULLSCREEN:
        OnGoFullscreen();
        return TRUE;

    case TT_MSG_STOP_FULLSCREEN:
        OnStopFullscreen();
        return TRUE;

    default:
        break;
    }

    return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void CGamePlayer::Construct()
{
    PlayerOptions options = {
        DEFAULT_WIDTH,
        DEFAULT_HEIGHT,
        DEFAULT_BPP,
        0,
        false,
        true};

    char fullPath[MAX_PATH] = "";
    char drive[4] = "";
    char dir[MAX_PATH] = "";
    char filename[MAX_PATH] = "";
    char rootPath[512] = "";

    ::GetModuleFileNameA(NULL, fullPath, MAX_PATH);
    _splitpath(fullPath, drive, dir, filename, NULL);
    strcpy(m_Path, "..\\");
    sprintf(rootPath, "%s%s%s", drive, dir, m_Path);
    TT_ERROR_OPEN(filename, rootPath, true);
    TT_LOG_OPEN(filename, rootPath, false);

    fillResourceMap(&g_ResMap);
    if (IsNoSettingsInIni())
    {
        // Default settings
        IniSetFullscreen(options.fullscreen);
        IniSetBPPAndDriver(options.bpp, options.driver);
        IniSetResolution(options.width, options.height);
    }
    else
    {
        options.fullscreen = IniGetFullScreen();
        IniGetBPPAndDriver(&options.bpp, &options.driver);
        options.width = IniGetWidth();
        options.height = IniGetHeight();
    }

    if (IsUsingCommandLine())
    {
        ParseCommandLine(options);
    }

    m_NeMoContext.SetScreen(&m_WinContext, options.fullscreen, options.driver, options.bpp, options.width, options.height);
    m_WinContext.SetResolution(options.width, options.height);

    m_State = eInitialized;
}

bool CGamePlayer::InitEngine()
{
    char drive[4] = "";
    char fullpath[512] = "";
    char buffer[MAX_PATH] = "";
    char dir[MAX_PATH] = "";

    {
        CSplash splash(m_WinContext.GetAppInstance());
        splash.Show();
    }

    if (!m_NeMoContext.StartUp())
    {
        return false;
    }

    ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
    _splitpath(buffer, drive, dir, NULL, NULL);
    sprintf(fullpath, "%s%s%s", drive, dir, m_Path);
    if (!fullpath)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "Unable to set ProgPath");
        return false;
    }
    m_NeMoContext.SetProgPath(fullpath);

    sprintf(m_PluginPath, "%s%s%s%s", drive, dir, m_Path, "Plugins");
    sprintf(m_RenderPath, "%s%s%s%s", drive, dir, m_Path, "RenderEngines");
    sprintf(m_ManagerPath, "%s%s%s%s", drive, dir, m_Path, "Managers");
    sprintf(m_BehaviorPath, "%s%s%s%s", drive, dir, m_Path, "BuildingBlocks");

    if (!LoadEngineDLL() || !LoadStdDLL())
    {
        return false;
    }

    if (!m_NeMoContext.Init())
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "Failed to init NemoContext");
        return false;
    }

    {
        CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
        if (!im)
        {
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "No InterfaceManager");
            return false;
        }

        im->SetDriverIndex(m_NeMoContext.GetDriverIndex());
        im->SetScreenModeIndex(m_NeMoContext.GetScreenMode());

        if (strlen(g_ResMap.pathSetting) > 128)
        {
            im->SetIniName("Ballance.ini");
        }
        else
        {
            im->SetIniName(g_ResMap.pathSetting);
        }

        im->SetRookie(m_IsRookie);
        im->SetTaskSwitchEnabled(m_TaskSwitchEnabled);
    }

    m_WinContext.ShowWindows();
    m_WinContext.UpdateWindows();

    return true;
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
    {
        return false;
    }

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

bool CGamePlayer::RegisterGameInfoToInterfaceManager()
{
    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im)
    {
        return false;
    }

    im->SetGameInfo(m_Game.GetGameInfo());
    return true;
}