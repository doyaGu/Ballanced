#include "StdAfx.h"

#include "GamePlayer.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "dsetup.h"

#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "ResDll.h"
#include "Splash.h"
#include "TT_InterfaceManager_RT/InterfaceManager.h"
#include "TerraTools.h"

extern RESOURCEMAP g_ResMap;

static inline bool IsNoSettingsInRegistry()
{
    return (g_ResMap.noKey & 2) == 2;
}

static bool RegSetBPP(int bpp)
{
    HKEY hkResult;
    DWORD dwDisposition;

    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(DWORD);

    if (::RegCreateKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS ||
        (bpp != 32 && bpp != 16))
    {
        ::RegCloseKey(hkResult);
        return false;
    }

    if (::RegQueryValueExA(hkResult, g_ResMap.videoDriver, 0, &dwType, (LPBYTE)&g_ResMap.dwVideoDriverSetting, &cbData) != ERROR_SUCCESS)
        g_ResMap.dwVideoDriverSetting = 0;

    if (bpp == 16)
    {
        g_ResMap.dwVideoDriverSetting &= 0xFFFE;
    }
    else if ((g_ResMap.dwVideoDriverSetting & 1) != 1)
    {
        g_ResMap.dwVideoDriverSetting |= 1;
    }

    if (::RegSetValueExA(hkResult, g_ResMap.videoDriver, 0, REG_DWORD, (LPBYTE)&g_ResMap.dwVideoDriverSetting, 4) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        return true;
    }

    return false;
}

static bool RegSetBPPAndDriver(int bpp, int driver)
{
    HKEY hkResult;
    DWORD dwDisposition;

    if (::RegCreateKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        return false;
    }

    if (bpp == 32)
        g_ResMap.dwVideoDriverSetting = (driver << 16) | 1;
    else if (bpp == 16)
        g_ResMap.dwVideoDriverSetting = (driver << 16) | 0;

    if (::RegSetValueExA(hkResult, g_ResMap.videoDriver, 0, REG_DWORD, (LPBYTE)&g_ResMap.dwVideoDriverSetting, 4) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        return true;
    }

    return false;
}

static bool RegSetFullscreen(bool fullscreen)
{
    HKEY hkResult;
    DWORD dwDisposition;
    DWORD dwFullscreen = (DWORD)fullscreen;

    if (::RegCreateKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS ||
        ::RegSetValueExA(hkResult, g_ResMap.fullScreen, 0, REG_DWORD, (LPBYTE)&dwFullscreen, 4) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        return false;
    }

    g_ResMap.fullScreenSetting = fullscreen;
    ::RegCloseKey(hkResult);
    return true;
}

static bool RegSetResolution(int width, int height)
{
    HKEY hkResult;
    DWORD dwDisposition;

    g_ResMap.dwVideoModeSetting = (width << 16) | height;
    if (::RegCreateKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS ||
        ::RegSetValueExA(hkResult, g_ResMap.videoMode, 0, REG_DWORD, (LPBYTE)&g_ResMap.dwVideoModeSetting, 4) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        return false;
    }

    ::RegCloseKey(hkResult);
    return true;
}

static bool RegGetBPPAndDriver(int *bpp, int *driver)
{
    HKEY hkResult;
    DWORD dwDisposition;

    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(DWORD);

    if (::RegCreateKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS ||
        ::RegQueryValueExA(hkResult, g_ResMap.videoDriver, 0, &dwType, (LPBYTE)&g_ResMap.dwVideoDriverSetting, &cbData))
    {
        ::RegCloseKey(hkResult);
        return false;
    }

    *bpp = (g_ResMap.dwVideoDriverSetting & 1) != 0 ? 32 : 16;
    *driver = g_ResMap.dwVideoDriverSetting >> 16;

    ::RegCloseKey(hkResult);
    return true;
}

static int RegGetWidth()
{
    return HIWORD(g_ResMap.dwVideoModeSetting);
}

static int RegGetHeight()
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

    {
        DWORD dwDXVersion;
        DWORD dwDXMinorVersion;
        if (!DirectXSetupGetVersion(&dwDXVersion, &dwDXMinorVersion))
        {
            TT_LOG("Could not detect the DirectX-version!");
            ::MessageBoxA(NULL, g_ResMap.unknownDirectXVersion, "Error", MB_ICONERROR);
            return false;
        }

        sprintf(buffer, "DirectX version is %d.%d.%d.%d ... ",
                HIWORD(dwDXVersion),
                LOWORD(dwDXVersion),
                HIWORD(dwDXMinorVersion),
                LOWORD(dwDXMinorVersion));
        TT_LOG(buffer);

        if (HIWORD(dwDXVersion) != 4 ||
            LOWORD(dwDXVersion) <= HIWORD(g_ResMap.dwDXVersion) &&
                (dwDXVersion != (0x40000 | HIWORD(g_ResMap.dwDXVersion)) ||
                 HIWORD(dwDXMinorVersion) < LOWORD(g_ResMap.dwDXVersion)))
        {
            TT_LOG("DirectX check failed !");
            ::MessageBoxA(NULL, g_ResMap.needDirectX, "Error", MB_ICONERROR);
            return false;
        }
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
                fError = CNeMoContext::GetInstance()->SetScreenMode(SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, curIdx, 0));
            }

            CTTInterfaceManager *interfaceManager = CNeMoContext::GetInstance()->GetInterfaceManager();
            if (!interfaceManager)
            {
                TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
                return TRUE;
            }
            interfaceManager->SetDriverIndex(CNeMoContext::GetInstance()->GetDriverIndex());
            interfaceManager->SetScreenModeIndex(CNeMoContext::GetInstance()->GetScreenModeIndex());

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
      field_E(false),
      m_DataManager(),
      m_NeMoContext(),
      m_WinContext(),
      m_Stack(defaultSetting),
      m_Game(),
      m_IsRookie(rookie)
{
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
    // char path[128];
    // strcpy(path, (const char *)wParam);
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
    try
    {
        RegisterGameInfoToInterfaceManager();
        m_Game.Load();
        m_Game.Play();
        m_WinContext.SetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
    }
    catch (const CGamePlayerException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnReturn()", "Non critical");
        throw CGamePlayerException(10);
    }
    catch (const CNeMoContextException &nmce)
    {
        if (nmce.error == 2)
        {
            m_NeMoContext.RestoreWindow();
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnReturn()", "Non critical");
        }
    }
    catch (const CGameException &ge)
    {
        if (ge.error == 1 || ge.error == 5 || ge.error == -1)
        {
            m_NeMoContext.RestoreWindow();
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::OnReturn()", "SYSTEM_HALT");
            ::MessageBoxA(m_WinContext.GetMainWindow(), "Error occurred in the engine. Abort!", "Error", MB_OK);
            ::PostQuitMessage(-1);
        }

        if (ge.error != 4 && ge.error != 3)
        {
            TT_ERROR_BOX("GamePlayer.cpp", "CGamePlayer::OnReturn()", "CGameException caught");
            return;
        }

        CGameInfo *gameInfoStack = m_Stack.RemoveHead();
        CGameInfo *gameInfo = m_Game.GetGameInfo();
        if (gameInfo)
        {
            delete gameInfo;
        }

        m_Game.SetGameInfo(gameInfoStack);
        if (gameInfoStack)
        {
            m_Game.Play();
        }
    }
}

void CGamePlayer::OnSized()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext && !renderContext->IsFullScreen())
    {
        RECT rect;
        ::GetClientRect(m_WinContext.GetMainWindow(), &rect);
        ::SetWindowPos(m_WinContext.GetRenderWindow(), HWND_TOP, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER);
        renderContext->Resize();
        m_NeMoContext.MoveFrameRateSpriteToLeftTop();
    }
}

void CGamePlayer::OnClose()
{
    Done();
}

void CGamePlayer::OnPaint()
{
    if (m_NeMoContext.GetRenderContext() && !m_NeMoContext.GetRenderContext()->IsFullScreen())
    {
        // In windowed mode call render when WM_PAINT
        m_NeMoContext.GetRenderContext()->Render(CK_RENDER_USECURRENTSETTINGS);
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
        PostQuitMessage(0);
        return 1;
    }
    return 0;
}

LRESULT CGamePlayer::OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool isPlaying;
    static bool activated, displayChanged;

    CTTInterfaceManager *interfaceManager = m_NeMoContext.GetInterfaceManager();
    if (!interfaceManager || !m_NeMoContext.GetInterfaceManager()->IsTaskSwitchEnabled())
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
                ::ShowWindow(m_WinContext.GetMainWindow(), SW_MINIMIZE);

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
    LRESULT res = 0;

    int screenMode = m_NeMoContext.GetScreenModeIndex();
    int driver = m_NeMoContext.GetDriverIndex();

    if (m_NeMoContext.SwitchScreenMode(lParam, wParam))
    {
        res = 1;
    }

    CTTInterfaceManager *interfaceManager = m_NeMoContext.GetInterfaceManager();
    if (!interfaceManager)
    {
        TT_ERROR("GamePlayer.cpp", "WndProc()", "No InterfaceManager");
        return 1;
    }

    interfaceManager->SetDriverIndex(driver);
    interfaceManager->SetScreenModeIndex(screenMode);
    RegSetBPPAndDriver(m_NeMoContext.GetColorBPP(), m_NeMoContext.GetDriverIndex());
    RegSetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
    ::SetFocus(m_WinContext.GetMainWindow());

    return res;
}

void CGamePlayer::OnCommand(UINT id, UINT code)
{
}

CGamePlayer::~CGamePlayer()
{
}

void CGamePlayer::Run()
{
    try
    {
        while (Step())
            continue;
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Run()", "Unhandled Exception - System-Halt");
    }
}

bool CGamePlayer::Step()
{
    MSG msg;
    BOOL bRet;

    try
    {
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
            if (m_NeMoContext.IsPlaying() && !field_E)
            {
                m_NeMoContext.Process();
            }
            return true;
        }

        if (msg.message != WM_QUIT && msg.message != WM_DESTROY)
        {
            HACCEL hAccel = m_WinContext.GetAccelTable();
            if (!::TranslateAcceleratorA(msg.hwnd, hAccel, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessageA(&msg);
            }
            return true;
        }
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "Step()", "Unhandled Exception - System-Halt");
    }

    return false;
}

LRESULT CGamePlayer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    case WM_SIZE:
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
        if (lParam != NULL)
        {
            ((MINMAXINFO *)lParam)->ptMaxPosition.x = 400;
            ((MINMAXINFO *)lParam)->ptMaxPosition.y = 200;
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
            // ::SystemParametersInfoA(SPI_SCREENSAVERRUNNING, 0, NULL, 0);
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
    bool settingChanged = false;
	bool engineReinitialized = false;

    m_State = eInitial;

    try
    {
        if (!CheckPrerequisite())
        {
            return;
        }

        CNeMoContext::RegisterInstance(&m_NeMoContext);
        m_Game.SetNeMoContext(&m_NeMoContext);
        m_WinContext.Init(hInstance, lpfnWndProc, m_NeMoContext.IsFullscreen());

        if (!InitEngine())
        {
            if (m_DefaultSetting)
            {
                m_NeMoContext.SetDriverIndex(0);
                m_NeMoContext.SetColorBPP(16);
                m_NeMoContext.SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
                engineReinitialized = ReInitEngine();
                settingChanged = true;
            }

            if (!m_DefaultSetting || !engineReinitialized)
            {

                if (::DialogBoxParamA(m_WinContext.GetAppInstance(), (LPCSTR)IDD_FULLSCREEN_SETUP, NULL, DialogProc, 0) != 1)
                {
                    m_State = eInitial;
                    return;
                }
                if (ReInitEngine())
                {
                    settingChanged = true;
                }
            }

            if (settingChanged)
            {
                RegSetBPPAndDriver(m_NeMoContext.GetColorBPP(), m_NeMoContext.GetScreenModeIndex());
                RegSetResolution(m_NeMoContext.GetWidth(), m_NeMoContext.GetHeight());
            }
        }

        m_WinContext.ShowWindows();
        m_WinContext.UpdateWindows();
        
        m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
        m_NeMoContext.GetRenderContext()->BackToFront(CK_RENDER_USECURRENTSETTINGS);
        m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);

        ::SendMessageA(m_WinContext.GetMainWindow(), TT_MSG_EXIT_TO_TITLE, NULL, NULL);
        ::SetFocus(m_WinContext.GetMainWindow());
        // ::SystemParametersInfoA(SPI_SCREENSAVERRUNNING, 0, 0, 0);

        m_State = eInitialized;
    }
    catch (const CWinContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "WinContext threw Exception - Aborting");
    }
    catch (const CGamePlayerException &gpe)
    {
        switch (gpe.error)
        {
        case 4:
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "No manager dll is found");
            break;
        case 9:
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "SYSTEM HALTED");
            break;
        default:
            throw CGamePlayerException(11);
            break;
        }
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Init()", "Unhandled Exception - Abort");
    }
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
    try
    {
        if (m_hMutex)
        {
            ::ReleaseMutex(m_hMutex);
            m_hMutex = NULL;
        }

        if (!m_Cleared)
        {
            m_NeMoContext.Cleanup();
            m_NeMoContext.RestoreWindow();
            m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
            m_NeMoContext.GetRenderContext()->SetClearBackground(TRUE);
            m_NeMoContext.GetRenderContext()->BackToFront(CK_RENDER_USECURRENTSETTINGS);
            m_NeMoContext.GetRenderContext()->SetClearBackground(TRUE);
            m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
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
        exit(1);
    }
    catch (const CGameStackException &)
    {
        TT_ERROR("GamePlayer.cpp", "Done()", "Non critical");
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "Done()", "Unhandled Exception");
    }
}

bool CGamePlayer::LoadCMO(const char *filename)
{
    try
    {
        m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
        m_NeMoContext.GetRenderContext()->BackToFront(CK_RENDER_USECURRENTSETTINGS);
        m_NeMoContext.GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);

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

        RegisterGameInfoToInterfaceManager();

        m_Game.Load();
        ::SetCursor(::LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
        m_Game.Play();
        ::SetFocus(m_WinContext.GetMainWindow());
        // ::SystemParametersInfoA(SPI_SCREENSAVERRUNNING, 0, NULL, 0);

        return true;
    }

    catch (const CGamePlayerException &)
    {
        TT_ERROR("GamePlayer.cpp", "LoadCMO()", "Non critical");
    }
    catch (const CNeMoContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "LoadCMO()", "Missing RenderContext - critical - Abort");
    }
    catch (const CGameException &)
    {
        TT_ERROR("GamePlayer.cpp", "LoadCMO()", "CGameException. Abort!");
    }

    return false;
}

void CGamePlayer::Construct()
{
    strcpy(m_Path, "..\\");

    try
    {
        char fullPath[260];
        char drive[4];
        char dir[260];
        char filename[260];
        char rootPath[512];

        ::GetModuleFileNameA(NULL, fullPath, 260);
        _splitpath(fullPath, drive, dir, filename, NULL);
        sprintf(rootPath, "%s%s%s", drive, dir, m_Path);
        TT_ERROR_OPEN(filename, rootPath, true);
        TT_LOG_OPEN(filename, rootPath, false);
        fillResourceMap(&g_ResMap);

        if (IsNoSettingsInRegistry()) // if missing key in Settings Registry
        {
            // Default settings
            m_NeMoContext.SetScreen(&m_WinContext, false, 0, 16, DEFAULT_WIDTH, DEFAULT_HEIGHT);
            m_WinContext.SetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
            RegSetBPP(16);
            RegSetBPPAndDriver(16, 0);
            RegSetFullscreen(false);
            RegSetResolution(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        }
        else
        {
            int bpp, driver;
            RegGetBPPAndDriver(&bpp, &driver);
            int width = RegGetWidth();
            int height = RegGetHeight();
            m_NeMoContext.SetScreen(&m_WinContext, g_ResMap.fullScreenSetting == TRUE, driver, bpp, width, height);
            m_WinContext.SetResolution(width, height);
        }
        m_State = eInitialized;
    }
    catch (const CNeMoContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Construct()", "NemoContext threw Exception - Aborting");
    }
    catch (const CWinContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Construct()", "WinContext threw Exception - Aborting");
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::Construct()", "Unhandled Exception");
    }
}

bool CGamePlayer::InitEngine()
{
    char drive[4];
    char fullpath[512];
    char buffer[260];
    char dir[256];

    CSplash *splash = new CSplash(m_WinContext.GetAppInstance());
    splash->Show();
    delete splash;

    try
    {
        m_NeMoContext.DoStartUp();
    }
    catch (const CNeMoContextException &)
    {
        return false;
    }

    try
    {
        ::GetModuleFileNameA(NULL, buffer, 260);
        _splitpath(buffer, drive, dir, NULL, NULL);
        sprintf(fullpath, "%s%s%s", drive, dir, m_Path);
        if (fullpath)
        {
            m_NeMoContext.SetProgPath(fullpath);
        }
    }
    catch (const CNeMoContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "SetProgPath");
        return false;
    }

    sprintf(m_PluginPath, "%s%s%s%s", drive, dir, m_Path, "Plugins");
    sprintf(m_RenderPath, "%s%s%s%s", drive, dir, m_Path, "RenderEngines");
    sprintf(m_ManagerPath, "%s%s%s%s", drive, dir, m_Path, "Managers");
    sprintf(m_BehaviorPath, "%s%s%s%s", drive, dir, m_Path, "BuildingBlocks");

    m_State = eInitial;

    try
    {
        LoadEngineDLL();
        LoadStdDLL();

        if (!m_NeMoContext.Init())
        {
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "Init NemoContext");
            return false;
        }

        CTTInterfaceManager *interfaceManager = m_NeMoContext.GetInterfaceManager();
        if (!interfaceManager)
        {
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "No InterfaceManager");
            return false;
        }

        interfaceManager->SetDriverIndex(m_NeMoContext.GetDriverIndex());
        interfaceManager->SetScreenModeIndex(m_NeMoContext.GetScreenModeIndex());

        m_WinContext.ShowWindows();
        m_WinContext.UpdateWindows();

        interfaceManager->SetRookie(m_IsRookie);
    }
    catch (const CGamePlayerException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "GamePlayer Exception");
        return false;
    }
    catch (const CNeMoContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "NemoContext Exception");
        return false;
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::InitEngine()", "Unhandled Exception");
        return false;
    }

    m_State = eInitialized;
    return true;
}

bool CGamePlayer::ReInitEngine()
{
    try
    {
        m_State = eInitial;

        if (!m_NeMoContext.ReInit())
        {
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::ReInitEngine()", "ReInit NemoContext");
            return false;
        }

        m_WinContext.ShowWindows();
        m_WinContext.UpdateWindows();

        m_State = eInitialized;
        return true;
    }
    catch (const CGamePlayerException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::ReInitEngine()", "CGamePlayerException");
    }
    catch (const CNeMoContextException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::ReInitEngine()", "NeMoContextException");
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::ReInitEngine()", "Unhandled Exception");
    }

    return false;
}

void CGamePlayer::LoadEngineDLL()
{
    try
    {
        if (!m_NeMoContext.ParsePlugins(m_RenderPath) || _access(m_RenderPath, 0) == -1)
        {
            throw CGamePlayerException(1);
        }
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadEngineDLL()", "Render path not available");
        throw;
    }
}

void CGamePlayer::LoadStdDLL()
{
    if (!m_NeMoContext.ParsePlugins(m_ManagerPath))
    {
        m_NeMoContext.RestoreWindow();
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "manager parse error");
        throw CGamePlayerException(3);
    }

    if (!m_NeMoContext.ParsePlugins(m_BehaviorPath))
    {
        m_NeMoContext.RestoreWindow();
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "behavior parse error");
        throw CGamePlayerException(12);
    }

    m_NeMoContext.ParsePlugins(m_ManagerPath);

    try
    {
        if (!m_ManagerPath || _access(m_PluginPath, 0) == -1)
        {
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "empty or wrong plugin path");
            throw CGamePlayerException(4);
        }

        if (!m_NeMoContext.ParsePlugins(m_PluginPath))
        {
            m_NeMoContext.RestoreWindow();
            TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "plugin parse error");
            throw CGamePlayerException(5);
        }
    }
    catch (const CGamePlayerException &)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "SYSTEM HALTED");
        ::PostQuitMessage(-1);
    }
    catch (...)
    {
        TT_ERROR("GamePlayer.cpp", "CGamePlayer::LoadStdDLL()", "Unhandled Exception");
    }
}

inline void CGamePlayer::RegisterGameInfoToInterfaceManager()
{
    m_NeMoContext.GetInterfaceManager()->SetGameInfo(m_Game.GetGameInfo());
}