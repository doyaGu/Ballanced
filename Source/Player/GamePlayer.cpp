#include "GamePlayer.h"

#include <io.h>
#include <stdio.h>
#include <string.h>

#include "GameConfig.h"
#include "Splash.h"
#include "Logger.h"
#include "InterfaceManager.h"
#include "resource.h"

CGamePlayer *CGamePlayer::s_Instance = NULL;

static CKERROR LogRedirect(CKUICallbackStruct &cbStruct, void *)
{
    if (cbStruct.Reason == CKUIM_OUTTOCONSOLE ||
        cbStruct.Reason == CKUIM_OUTTOINFOBAR ||
        cbStruct.Reason == CKUIM_DEBUGMESSAGESEND)
    {
        static XString text = "";
        if (text.Compare(cbStruct.ConsoleString))
        {
            CLogger::Get().Info(cbStruct.ConsoleString);
            text = cbStruct.ConsoleString;
        }
    }
    return CK_OK;
}

static bool ClipMouse(bool enable)
{
    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderContext *rc = context->GetRenderContext();
    if (!rc)
        return false;

    if (!enable)
        return ClipCursor(NULL) == TRUE; // Disable the clipping

    // Retrieve the render window rectangle
    VxRect r;
    rc->GetWindowRect(r, TRUE);

    RECT rect;
    rect.top = (LONG)r.top;
    rect.left = (LONG)r.left;
    rect.bottom = (LONG)r.bottom;
    rect.right = (LONG)r.right;

    // To clip the mouse in it.
    return ClipCursor(&rect) == TRUE;
}

static BOOL FillScreenModeList(HWND hWnd)
{
    char buffer[256];

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();
    if (!rm)
        return FALSE;

    VxDriverDesc *drDesc = rm->GetRenderDriverDescription(context->GetDriver());
    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;
    int i = 0;
    while (i < dmCount)
    {
        int width = dm[i].Width;
        int height = dm[i].Height;
        while (dm[i].Width == width && dm[i].Height == height && i < dmCount)
        {
            if (dm[i].Bpp > 8)
            {
                sprintf(buffer, "%d x %d x %d x %dHz", dm[i].Width, dm[i].Height, dm[i].Bpp, dm[i].RefreshRate);
                int index = SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_ADDSTRING, 0, (LPARAM)buffer);
                SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETITEMDATA, index, i);
                if (i == context->GetScreenMode())
                {
                    SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETCURSEL, index, 0);
                    SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_SETTOPINDEX, index, 0);
                }
            }
            ++i;
        }
    }

    return TRUE;
}

static void OnInitDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!FillScreenModeList(hWnd))
        EndDialog(hWnd, IDCANCEL);

    CNeMoContext *context = CNeMoContext::GetInstance();
    CKRenderManager *rm = context->GetRenderManager();
    const int drCount = rm->GetRenderDriverCount();
    for (int i = 0; i < drCount; ++i)
    {
        VxDriverDesc *drDesc = rm->GetRenderDriverDescription(i);
        int index = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_ADDSTRING, 0, (LPARAM)drDesc->DriverName);
        ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETITEMDATA, index, i);
        if (i == context->GetDriver())
            ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_SETCURSEL, index, 0);
    }
}

static BOOL CALLBACK FullscreenSetupProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WORD wNotifyCode = HIWORD(wParam);
    int wID = LOWORD(wParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
        OnInitDialog(hWnd, uMsg, wParam, lParam);
        return TRUE;
    case WM_COMMAND:
    {
        CNeMoContext *context = CNeMoContext::GetInstance();
        if (wNotifyCode == LBN_SELCHANGE && wID == IDC_LB_DRIVER)
        {
            int index = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETCURSEL, 0, 0);
            int driver = ::SendDlgItemMessageA(hWnd, IDC_LB_DRIVER, LB_GETITEMDATA, index, 0);
            context->SetDriver(driver);

            ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_RESETCONTENT, 0, 0);
            if (!FillScreenModeList(hWnd))
                EndDialog(hWnd, FALSE);
            return TRUE;
        }
        else if (wID == IDOK || wID == IDCANCEL)
        {
            int index = ::SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETCURSEL, 0, 0);
            int screenMode = SendDlgItemMessageA(hWnd, IDC_LB_SCREEN_MODE, LB_GETITEMDATA, index, 0);
            context->SetScreenMode(screenMode);

            EndDialog(hWnd, wID);
            return TRUE;
        }
    }
    break;
    }
    return FALSE;
}

static BOOL CALLBACK AboutProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hWnd, LOWORD(wParam));
            return TRUE;
        }
        break;
    default:
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CGamePlayer *player = CGamePlayer::GetInstance();
    assert(player != NULL);

    switch (uMsg)
    {
    case WM_DESTROY:
        player->OnDestroy();
        break;

    case WM_MOVE:
        player->OnMove();
        break;

    case WM_SIZE:
        player->OnSized();
        break;

    case WM_PAINT:
        player->OnPaint();
        break;

    case WM_CLOSE:
        player->OnClose();
        return 0;

    case WM_ACTIVATEAPP:
        player->OnActivateApp(hWnd, uMsg, wParam, lParam);
        break;

    case WM_SETCURSOR:
        player->OnSetCursor();
        break;

    case WM_GETMINMAXINFO:
        player->OnGetMinMaxInfo((LPMINMAXINFO)lParam);
        break;

    case WM_KEYDOWN:
        return player->OnKeyDown(wParam);

    case WM_SYSKEYDOWN:
        return player->OnSysKeyDown(wParam);

    case WM_COMMAND:
        return player->OnCommand(LOWORD(wParam), HIWORD(wParam));

    case TT_MSG_NO_GAMEINFO:
        player->OnExceptionCMO(wParam, lParam);
        break;

    case TT_MSG_CMO_RESTART:
        player->OnReturn(wParam, lParam);
        break;

    case TT_MSG_CMO_LOAD:
        player->OnLoadCMO(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_SYS:
        player->OnExitToSystem(wParam, lParam);
        break;

    case TT_MSG_EXIT_TO_TITLE:
        player->OnExitToTitle(wParam, lParam);
        break;

    case TT_MSG_SCREEN_MODE_CHG:
        return player->OnChangeScreenMode(wParam, lParam);

    case TT_MSG_GO_FULLSCREEN:
        player->OnGoFullscreen();
        break;

    case TT_MSG_STOP_FULLSCREEN:
        player->OnStopFullscreen();
        return 1;

    default:
        break;
    }

    return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

CGamePlayer::CGamePlayer()
    : m_State(eInitial),
      m_NeMoContext(),
      m_WinContext(),
      m_Game(),
      m_hMutex(NULL) {}

CGamePlayer::~CGamePlayer()
{
    Terminate();
}

bool CGamePlayer::Init(HINSTANCE hInstance, HANDLE hMutex)
{
    if (m_State != eInitial)
        return true;

    Register(this);
    m_hMutex = hMutex;

    CGameConfig &config = CGameConfig::Get();

    if (!config.HasPath(eRootPath))
    {
        CLogger::Get().Error("Root path is not specified");
        return false;
    }

    {
        const char *dirs[] = {
            "Plugins",
            "RenderEngines",
            "Managers",
            "BuildingBlocks",
            "Sounds",
            "Textures",
            ""};
        const char *rootPath = config.GetPath(eRootPath);
        char szPath[MAX_PATH];
        for (int p = ePluginPath; p < ePathCategoryCount; ++p)
        {
            if (!config.HasPath((PathCategory)p))
            {
                _snprintf(szPath, MAX_PATH, "%s%s", rootPath, dirs[p - ePluginPath]);
                config.SetPath((PathCategory)p, szPath);
            }
        }
    }

    // Save the last position firstly.
    int x = config.posX;
    int y = config.posY;

    if (!m_WinContext.Init(hInstance, WndProc))
    {
        CLogger::Get().Error("WinContext Initialization Failed");
        return false;
    }

    CNeMoContext::RegisterInstance(&m_NeMoContext);

    switch (InitEngine())
    {
    case CK_OK:
        RedirectLog();
        break;
    case CKERR_NODLLFOUND:
        ::MessageBoxA(NULL, "Necessary dll is not found", "Error", MB_OK);
        return false;
    case CKERR_NORENDERENGINE:
        ::MessageBoxA(NULL, "No RenderEngine", "Error", MB_OK);
        return false;
    case CKERR_INVALIDPARAMETER:
    {
        if (::DialogBoxParamA(m_WinContext.GetAppInstance(), MAKEINTRESOURCE(IDD_FULLSCREEN_SETUP), NULL, FullscreenSetupProc, 0) != IDOK)
            return false;
        if (!m_NeMoContext.ApplyScreenMode())
            return false;

        config.bpp = m_NeMoContext.GetBPP();
        config.driver = m_NeMoContext.GetDriver();
        config.width = m_NeMoContext.GetWidth();
        config.height = m_NeMoContext.GetHeight();
        m_WinContext.SetMainSize(config.width, config.height);
        m_WinContext.SetRenderSize(config.width, config.height);

        if (!ReInitEngine())
            return false;
    }
    break;
    default:
        return false;
    }

    InterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetDriver(m_NeMoContext.GetDriver());
    im->SetScreenMode(m_NeMoContext.GetScreenMode());
    im->SetRookie(config.rookie);
    im->SetTaskSwitchEnabled(true);

    if (!config.fullscreen && x != 2147483647 && y != 2147483647)
        m_WinContext.SetPosition(x, y);

    m_WinContext.UpdateWindows();
    m_WinContext.ShowMainWindow();
    m_WinContext.ShowRenderWindow();

    m_NeMoContext.RefreshScreen();

    ::SendMessageA(m_WinContext.GetMainWindow(), TT_MSG_EXIT_TO_TITLE, NULL, NULL);
    m_WinContext.FocusMainWindow();

    m_State = eReady;
    return true;
}

void CGamePlayer::Run()
{
    while (Process())
        continue;
}

bool CGamePlayer::Process()
{
    if (!m_WinContext.Process())
        return false;

    m_NeMoContext.Process();

    return true;
}

void CGamePlayer::Terminate()
{
    if (m_State == eInitial)
        return;

    Pause();
    m_NeMoContext.Cleanup();
    if (m_NeMoContext.RestoreWindow())
        m_NeMoContext.RefreshScreen();
    m_NeMoContext.Shutdown();

    if (m_hMutex)
        ::CloseHandle(m_hMutex);
    m_hMutex = NULL;

    m_State = eInitial;
}

bool CGamePlayer::Load(const char *filename)
{
    if (m_State == eInitial)
        return false;

    CGameConfig &config = CGameConfig::Get();
    if (_access(config.GetPath(eSoundPath), 0) == -1)
    {
        CLogger::Get().Error("No Sounds directory");
        return false;
    }
    m_NeMoContext.AddSoundPath(config.GetPath(eSoundPath));

    if (_access(config.GetPath(eBitmapPath), 0) == -1)
    {
        CLogger::Get().Error("No Textures directory");
        return false;
    }
    m_NeMoContext.AddBitmapPath(config.GetPath(eBitmapPath));

    m_NeMoContext.AddDataPath(config.GetPath(eDataPath));

    CGameInfo *gameInfo = m_Game.NewGameInfo();
    strcpy(gameInfo->path, ".");
    strcpy(gameInfo->fileName, filename);

    InterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetGameInfo(m_Game.GetGameInfo());

    Pause();
    if (!m_Game.Load())
        return false;
    Play();

    return true;
}

void CGamePlayer::Play()
{
    m_State = ePlaying;
    m_NeMoContext.Play();
}

void CGamePlayer::Pause()
{
    m_State = ePaused;
    m_NeMoContext.Pause();
}

void CGamePlayer::Reset()
{
    m_State = ePlaying;
    m_NeMoContext.Reset();
    m_NeMoContext.Play();
}

void CGamePlayer::OnDestroy()
{
    m_NeMoContext.BroadcastCloseMessage();
    ::PostQuitMessage(0);
}

void CGamePlayer::OnMove()
{
    CGameConfig &config = CGameConfig::Get();
    if (!config.fullscreen)
        m_WinContext.GetPosition(config.posX, config.posY);
}

void CGamePlayer::OnSized()
{
    CKRenderContext *renderContext = m_NeMoContext.GetRenderContext();
    if (renderContext)
        m_NeMoContext.ResizeWindow();
}

void CGamePlayer::OnPaint()
{
    if (m_NeMoContext.GetRenderContext() && !m_NeMoContext.IsRenderFullscreen())
        m_NeMoContext.Render();
}

void CGamePlayer::OnClose()
{
    ::PostMessageA(m_WinContext.GetMainWindow(), TT_MSG_EXIT_TO_SYS, 0, 0);
}

void CGamePlayer::OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static bool wasPlaying = false;
    static bool wasFullscreen = false;
    static bool firstDeActivate = true;

    if (m_State == eInitial)
        return;

    CGameConfig &config = CGameConfig::Get();

    if (wParam != WA_ACTIVE)
    {
        if (m_NeMoContext.GetCKContext())
        {
            if (firstDeActivate)
                wasPlaying = m_NeMoContext.IsPlaying();

            if (config.pauseOnDeactivated)
                Pause();
            else if (!config.alwaysHandleInput)
                m_NeMoContext.GetInputManager()->Pause(TRUE);

            if (config.clipMouse)
                ClipMouse(false);

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
        m_State = eFocusLost;
    }
    else
    {
        if (wasFullscreen && !firstDeActivate)
            m_NeMoContext.GoFullscreen();

        if (config.clipMouse)
            ClipMouse(true);

        if (!config.alwaysHandleInput)
            m_NeMoContext.GetInputManager()->Pause(FALSE);

        if (wasPlaying)
            Play();

        firstDeActivate = true;
        m_State = ePlaying;
    }
}

void CGamePlayer::OnSetCursor()
{
    if (m_State == ePaused)
        ::SetCursor(::LoadCursorA(NULL, IDC_ARROW));
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
        OnSwitchFullscreen();
        break;

    case VK_F4:
        // ALT + F4 -> Quit the application
        OnClose();
        return 1;

    default:
        break;
    }
    return 0;
}

int CGamePlayer::OnCommand(UINT id, UINT code)
{
    switch (id)
    {
    case IDM_APP_ABOUT:
        Pause();
        ::DialogBoxParamA(m_WinContext.GetAppInstance(), MAKEINTRESOURCE(IDD_ABOUT), NULL, AboutProc, 0);
        Play();
        break;

    default:
        break;
    }
    return 0;
}

void CGamePlayer::OnExceptionCMO(WPARAM wParam, LPARAM lParam)
{
    m_NeMoContext.RestoreWindow();
    m_NeMoContext.Cleanup();
    CLogger::Get().Error("Exception in the CMO - Abort");
    OnDestroy();
}

void CGamePlayer::OnReturn(WPARAM wParam, LPARAM lParam)
{
    InterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetGameInfo(m_Game.GetGameInfo());

    if (!m_Game.Load())
        OnDestroy();
    Play();
}

bool CGamePlayer::OnLoadCMO(WPARAM wParam, LPARAM lParam)
{
    return Load((const char *)wParam);
}

void CGamePlayer::OnExitToSystem(WPARAM wParam, LPARAM lParam)
{
    OnDestroy();
}

void CGamePlayer::OnExitToTitle(WPARAM wParam, LPARAM lParam)
{
}

int CGamePlayer::OnChangeScreenMode(WPARAM wParam, LPARAM lParam)
{
    if (!m_NeMoContext.ChangeScreenMode(lParam, wParam))
    {
        CLogger::Get().Error("Failed to change screen mode");
        return 0;
    }

    m_Game.SyncCamerasWithScreen();

    InterfaceManager *im = m_NeMoContext.GetInterfaceManager();
    im->SetDriver(m_NeMoContext.GetDriver());
    im->SetScreenMode(m_NeMoContext.GetScreenMode());

    CGameConfig &config = CGameConfig::Get();
    config.bpp = m_NeMoContext.GetBPP();
    config.driver = m_NeMoContext.GetDriver();
    config.width = m_NeMoContext.GetWidth();
    config.height = m_NeMoContext.GetHeight();

    if (config.clipMouse)
        ClipMouse(true);

    return 1;
}

void CGamePlayer::OnGoFullscreen()
{
    Pause();
    m_NeMoContext.GoFullscreen();
    Play();
    CGameConfig::Get().fullscreen = true;
}

void CGamePlayer::OnStopFullscreen()
{
    Pause();
    m_NeMoContext.StopFullscreen();
    Play();
    CGameConfig::Get().fullscreen = false;
}

void CGamePlayer::OnSwitchFullscreen()
{
    if (m_State == eInitial)
        return;

    Pause();

    CGameConfig &config = CGameConfig::Get();
    static int x = config.posX;
    static int y = config.posY;
    if (!m_NeMoContext.IsRenderFullscreen())
    {
        m_NeMoContext.GoFullscreen();
        config.fullscreen = true;
        if (config.clipMouse)
            ClipMouse(false);
    }
    else
    {
        m_NeMoContext.RestoreWindow();
        config.fullscreen = false;
        m_WinContext.SetPosition(x, y);
        if (config.clipMouse)
            ClipMouse(true);
    }

    Play();
}

void CGamePlayer::Register(CGamePlayer *player)
{
    s_Instance = player;
}

CGamePlayer *CGamePlayer::GetInstance()
{
    return s_Instance;
}

int CGamePlayer::InitEngine()
{
    {
        CSplash splash(m_WinContext.GetAppInstance());
        splash.Show();
    }

    CGameConfig &config = CGameConfig::Get();

    m_WinContext.SetMainSize(config.width, config.height);
    m_WinContext.SetRenderSize(config.width, config.height);

    m_NeMoContext.SetScreen(&m_WinContext, config.fullscreen, config.driver, config.bpp, config.width, config.height);

    if (!m_NeMoContext.StartUp())
        return CKERR_INVALIDPARAMETER;

    if (!LoadRenderEngine())
        return CKERR_INVALIDPARAMETER;

    if (!LoadPlugins())
        return CKERR_INVALIDPARAMETER;

    CKERROR err = m_NeMoContext.CreateContext();
    if (err != CK_OK)
        return err;

    CKRenderManager *rm = m_NeMoContext.GetRenderManager();
    rm->SetRenderOptions("DisableFilter", config.disableFilter);
    rm->SetRenderOptions("DisableDithering", config.disableDithering);
    rm->SetRenderOptions("Antialias", config.antialias);
    rm->SetRenderOptions("DisableMipmap", config.disableMipmap);
    rm->SetRenderOptions("DisableSpecular", config.disableSpecular);

    if (!m_NeMoContext.FindScreenMode())
    {
        CLogger::Get().Error("Found no capable screen mode");
        return CKERR_INVALIDPARAMETER;
    }

    if (!m_NeMoContext.CreateRenderContext())
    {
        CLogger::Get().Error("Failed to create render context");
        return CKERR_INVALIDPARAMETER;
    }

    m_NeMoContext.AddCloseMessage();

    return CK_OK;
}

bool CGamePlayer::ReInitEngine()
{
    if (!m_NeMoContext.CreateRenderContext())
    {
        CLogger::Get().Error("Cannot recreate render context");
        return false;
    }
    m_WinContext.UpdateWindows();
    m_WinContext.ShowMainWindow();
    m_WinContext.ShowRenderWindow();
    return true;
}

bool CGamePlayer::LoadRenderEngine()
{
    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(eRenderEnginePath);
    if (_access(path, 0) == -1 || !m_NeMoContext.ParsePlugins(path))
    {
        CLogger::Get().Error("Render engine parse error");
        return false;
    }
    return true;
}

bool CGamePlayer::LoadPlugins()
{
    CGameConfig &config = CGameConfig::Get();
    const char *path = config.GetPath(eManagerPath);
    if (_access(path, 0) == -1 || !m_NeMoContext.ParsePlugins(path))
    {
        CLogger::Get().Error("Managers parse error");
        return false;
    }

    path = config.GetPath(eBuildingBlockPath);
    if (_access(path, 0) == -1 || !m_NeMoContext.ParsePlugins(path))
    {
        CLogger::Get().Error("Behaviors parse error");
        return false;
    }

    path = config.GetPath(ePluginPath);
    if (_access(path, 0) == -1 || !m_NeMoContext.ParsePlugins(path))
    {
        CLogger::Get().Error("Plugins parse error");
        return false;
    }

    return true;
}

void CGamePlayer::RedirectLog()
{
    m_NeMoContext.GetCKContext()->SetInterfaceMode(FALSE, LogRedirect, NULL);
}