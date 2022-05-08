#include "StdAfx.h"

#include "GamePlayer.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "ResDll.h"
#include "Splash.h"
#include "TT_InterfaceManager_RT/InterfaceManager.h"
#include "TerraTools.h"

extern RESOURCEMAP g_ResMap;

static inline bool IsNoSettingsInIni()
{
    return (g_ResMap.fKey & 2) == 2;
}

static bool IniSetBPP(int bpp)
{
    if (bpp != 32 && bpp != 16)
    {
        return false;
    }

    DWORD dwVideoDriver = ::GetPrivateProfileIntA("Settings", g_ResMap.videoDriver, -1, g_ResMap.pathSetting);
    if (dwVideoDriver == -1)
    {
        return false;
    }

    if (bpp == 16)
    {
        dwVideoDriver &= 0xFFFE;
    }
    else if ((dwVideoDriver & 1) != 1)
    {
        dwVideoDriver |= 1;
    }

    g_ResMap.dwVideoDriverSetting = dwVideoDriver;

    char buffer[64];
    sprintf(buffer, "%d", g_ResMap.dwVideoDriverSetting);
    return ::WritePrivateProfileStringA("Settings", g_ResMap.videoDriver, buffer, g_ResMap.pathSetting);
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
                if (i == CNeMoContext::GetInstance()->GetScreenModeIndex())
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
                    if (i == CNeMoContext::GetInstance()->GetScreenModeIndex())
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
            im->SetScreenModeIndex(CNeMoContext::GetInstance()->GetScreenModeIndex());

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
      m_IsRookie(rookie)
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

void CGamePlayer::OnExitToTitle(WPARAM wParam, LPARAM lParam)
{
}

void CGamePlayer::OnExitToSystem(WPARAM wParam, LPARAM lParam)
{
    Done();
}

bool CGamePlayer::OnLoadCMO(WPARAM wParam, LPARAM lParam)
{
    return LoadCMO((const char *)wParam);
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

void CGamePlayer::OnSized()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext && !renderContext->IsFullScreen())
    {
        RECT rect;
        ::GetClientRect(m_WinContext.GetMainWindow(), &rect);
        ::SetWindowPos(m_WinContext.GetRenderWindow(), NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER);
        renderContext->Resize();
        m_NeMoContext.AdjustFrameRateSpritePosition();
    }
}

void CGamePlayer::OnClose()
{
    Done();
}

void CGamePlayer::OnPaint()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext && !renderContext->IsFullScreen())
    {
        renderContext->Render();
    }
}

void CGamePlayer::OnSetCursor()
{
    if (IsInitialized() && !m_NeMoContext.IsPlaying())
    {
        ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
    }
}

void CGamePlayer::OnMouseDoubleClick()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (!renderContext)
    {
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    if (!renderContext->IsFullScreen())
    {
        ::ScreenToClient(m_WinContext.GetRenderWindow(), &pt);
    }

    int msg = m_NeMoContext.GetMsgClick();

    CKPOINT ckpt = {pt.x, pt.y};
    CKPICKRESULT res;
    CKObject *obj = renderContext->Pick(ckpt, &res, FALSE);
    if (obj && CKIsChildClassOf(obj, CKCID_BEOBJECT))
    {
        m_NeMoContext.SendMessageSingle(msg, (CKBeObject *)obj, NULL);
    }
    if (res.Sprite)
    {
        CKObject *sprite = m_NeMoContext.GetObject(res.Sprite);
        if (sprite)
        {
            m_NeMoContext.SendMessageSingle(msg, (CKBeObject *)sprite, NULL);
        }
    }
}

void CGamePlayer::OnMouseClick()
{
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

LRESULT CGamePlayer::OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool isPlaying;
    static bool activated, displayChanged;

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im || !m_NeMoContext.GetInterfaceManager()->IsTaskSwitchEnabled())
    {
        return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }

    if (wParam == WA_ACTIVE)
    {
        if (isPlaying)
        {
            m_NeMoContext.Play();
        }

        if (displayChanged)
        {
            m_NeMoContext.GoFullscreen();
            if (isPlaying)
            {
                m_NeMoContext.Play();
            }
        }

        activated = true;
        return ::DefWindowProcA(hWnd, WM_ACTIVATEAPP, wParam, lParam);
    }
    else
    {
        if (m_NeMoContext.GetCKContext())
        {
            if (activated)
            {
                isPlaying = m_NeMoContext.IsPlaying();
            }

            if (m_NeMoContext.GetRenderContext() && m_NeMoContext.IsRenderFullScreen())
            {
                if (activated)
                {
                    displayChanged = true;
                }

                m_NeMoContext.RestoreWindow();
                m_NeMoContext.MinimizeWindow();

                activated = false;
                return ::DefWindowProcA(hWnd, WM_ACTIVATEAPP, 0, lParam);
            }
            else if (activated)
            {
                displayChanged = false;
            }
        }

        activated = false;
        return ::DefWindowProcA(hWnd, WM_ACTIVATEAPP, 0, lParam);
    }
}

LRESULT CGamePlayer::OnScreenModeChanged(WPARAM wParam, LPARAM lParam)
{
    int screenMode = m_NeMoContext.GetScreenModeIndex();
    int driver = m_NeMoContext.GetDriverIndex();

    if (m_NeMoContext.ChangeScreenMode(lParam, wParam))
    {
        screenMode = m_NeMoContext.GetScreenModeIndex();
        driver = m_NeMoContext.GetDriverIndex();
    }

    CTTInterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    if (!im)
    {
        TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
    }
    else
    {
        im->SetDriverIndex(driver);
        im->SetScreenModeIndex(screenMode);
    }

    IniSetBPPAndDriver(m_NeMoContext.GetBPP(), m_NeMoContext.GetDriverIndex());
    IniSetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());

    ::SetFocus(m_WinContext.GetMainWindow());
    return 1;
}

void CGamePlayer::OnCommand(UINT id, UINT code)
{
}

CGamePlayer::~CGamePlayer()
{
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
        if (m_NeMoContext.IsPlaying())
        {
            m_NeMoContext.Process();
        }
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

    case WM_QUIT:
        break;

    case WM_PAINT:
        OnPaint();
        break;

    case WM_CLOSE:
        m_NeMoContext.SendMessageWindowCloseToAll();
        return 0;

    case WM_ACTIVATEAPP:
        return OnActivateApp(hWnd, uMsg, wParam, lParam);

    case WM_SETCURSOR:
        OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpmmi = (LPMINMAXINFO)lParam;
        if (lpmmi)
        {
            lpmmi->ptMinTrackSize.x = 400;
            lpmmi->ptMinTrackSize.y = 200;
        }
    }
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
        OnMouseClick();
        break;

    case WM_LBUTTONDBLCLK:
        OnMouseDoubleClick();
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
        return OnScreenModeChanged(wParam, lParam);

    case TT_MSG_GO_FULLSCREEN:
        if (!m_NeMoContext.IsRenderFullScreen())
        {
            m_NeMoContext.GoFullscreen();
            ::SetFocus(m_WinContext.GetMainWindow());
            m_NeMoContext.Play();
            ::SetFocus(m_WinContext.GetMainWindow());
        }
        break;

    case TT_MSG_STOP_FULLSCREEN:
        if (m_NeMoContext.IsFullscreen() && m_NeMoContext.GetRenderContext()->IsFullScreen())
        {
            m_NeMoContext.GetRenderContext()->StopFullScreen();
            ::ShowWindow(m_WinContext.GetMainWindow(), SW_MINIMIZE);
        }
        return 1;

    default:
        break;
    }

    return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
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
            IniSetBPPAndDriver(m_NeMoContext.GetBPP(), m_NeMoContext.GetScreenModeIndex());
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
            m_NeMoContext.GetRenderContext()->Clear();
            m_NeMoContext.GetRenderContext()->SetClearBackground();
            m_NeMoContext.GetRenderContext()->BackToFront();
            m_NeMoContext.GetRenderContext()->SetClearBackground();
            m_NeMoContext.GetRenderContext()->Clear();
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
    exit(EXIT_SUCCESS);
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

void CGamePlayer::Construct()
{
    strcpy(m_Path, "..\\");

    char fullPath[MAX_PATH];
    char drive[4];
    char dir[MAX_PATH];
    char filename[MAX_PATH];
    char rootPath[512];

    ::GetModuleFileNameA(NULL, fullPath, MAX_PATH);
    _splitpath(fullPath, drive, dir, filename, NULL);
    sprintf(rootPath, "%s%s%s", drive, dir, m_Path);
    TT_ERROR_OPEN(filename, rootPath, true);
    TT_LOG_OPEN(filename, rootPath, false);
    fillResourceMap(&g_ResMap);

    if (IsNoSettingsInIni())
    {
        // Default settings
        m_NeMoContext.SetScreen(&m_WinContext, false, 0, DEFAULT_BPP, DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_WinContext.SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        IniSetBPP(DEFAULT_BPP);
        IniSetBPPAndDriver(DEFAULT_BPP, 0);
        IniSetFullscreen(false);
        IniSetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    }
    else
    {
        int bpp, driver;
        IniGetBPPAndDriver(&bpp, &driver);
        int width = IniGetWidth();
        int height = IniGetHeight();
        m_NeMoContext.SetScreen(&m_WinContext, g_ResMap.fullScreenSetting == TRUE, driver, bpp, width, height);
        m_WinContext.SetResolution(width, height);
    }
    m_State = eInitialized;
}

bool CGamePlayer::InitEngine()
{
    char drive[4];
    char fullpath[512];
    char buffer[MAX_PATH];
    char dir[MAX_PATH];

    CSplash *splash = new CSplash(m_WinContext.GetAppInstance());
    splash->Show();
    delete splash;

    if (!m_NeMoContext.DoStartUp())
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
        im->SetScreenModeIndex(m_NeMoContext.GetScreenModeIndex());

        if (strlen(g_ResMap.pathSetting) > 128)
        {
            im->SetIniName("Ballance.ini");
        }
        else
        {
            im->SetIniName(g_ResMap.pathSetting);
        }

        im->SetRookie(m_IsRookie);
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