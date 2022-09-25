#include "NeMoContext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WinContext.h"
#include "Logger.h"
#include "InterfaceManager.h"
#include "config.h"

CNeMoContext *CNeMoContext::s_Instance = NULL;

CNeMoContext::CNeMoContext()
    : m_CKContext(NULL),
      m_RenderManager(NULL),
      m_AttributeManager(NULL),
      m_PathManager(NULL),
      m_MessageManager(NULL),
      m_TimeManager(NULL),
      m_PluginManager(NULL),
      m_InputManager(NULL),
      m_InterfaceManager(NULL),
      m_RenderContext(NULL),
      m_WinContext(NULL),
      m_RenderEngine("CK2_3D"),
      m_Width(PLAYER_DEFAULT_WIDTH),
      m_Height(PLAYER_DEFAULT_HEIGHT),
      m_Bpp(PLAYER_DEFAULT_BPP),
      m_RefreshRate(0),
      m_Fullscreen(false),
      m_Driver(0),
      m_ScreenMode(-1),
      m_MsgWindowClose(0) {}

CNeMoContext::~CNeMoContext() {}

CKERROR CNeMoContext::CreateContext()
{
    int iRenderEngine = GetRenderEnginePluginIdx();
    if (iRenderEngine == -1)
    {
        CLogger::Get().Error("Found no render-engine! (Driver? Critical!!!)");
        return CKERR_NORENDERENGINE;
    }

    CKERROR res = CKCreateContext(&m_CKContext, m_WinContext->GetMainWindow(), iRenderEngine, 0);
    if (res != CK_OK)
    {
        if (res == CKERR_NODLLFOUND)
        {
            CLogger::Get().Error("Render engine is not found");
            return CKERR_NODLLFOUND;
        }

        CLogger::Get().Error("Unable to load render engine");
        return CKERR_NORENDERENGINE;
    }

    m_CKContext->SetVirtoolsVersion(CK_VIRTOOLS_DEV, 0x2000043);
    m_RenderManager = m_CKContext->GetRenderManager();
    m_AttributeManager = m_CKContext->GetAttributeManager();
    m_PathManager = m_CKContext->GetPathManager();
    m_MessageManager = m_CKContext->GetMessageManager();
    m_TimeManager = m_CKContext->GetTimeManager();
    m_InputManager = (CKInputManager *)m_CKContext->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!m_RenderManager || !m_AttributeManager || !m_PathManager || !m_MessageManager || !m_TimeManager || !m_InputManager)
    {
        CLogger::Get().Error("Unable to initialize Managers");
        return CKERR_NODLLFOUND;
    }

    m_InterfaceManager = (InterfaceManager *)m_CKContext->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (!m_InterfaceManager)
    {
        CLogger::Get().Error("No InterfaceManager found");
        return CKERR_NODLLFOUND;
    }

    return CK_OK;
}

bool CNeMoContext::StartUp()
{
    if (CKStartUp() != CK_OK)
    {
        CLogger::Get().Error("Virtools Engine cannot startup!");
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

void CNeMoContext::Process()
{
    if (m_CKContext->IsPlaying())
    {
        float beforeRender = 0.0f;
        float beforeProcess = 0.0f;
        m_TimeManager->GetTimeToWaitForLimits(beforeRender, beforeProcess);
        if (beforeProcess <= 0)
        {
            m_TimeManager->ResetChronos(FALSE, TRUE);
            m_CKContext->Process();
        }
        if (beforeRender <= 0)
        {
            m_TimeManager->ResetChronos(TRUE, FALSE);
            m_RenderContext->Render();
        }
    }
}

CKERROR CNeMoContext::Render(CK_RENDER_FLAGS flags)
{
    return m_RenderContext->Render(flags);
}

void CNeMoContext::ClearScreen()
{
    if (m_RenderContext)
    {
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
    }
}

void CNeMoContext::RefreshScreen()
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

void CNeMoContext::SetScreen(CWinContext *wincontext, bool fullscreen, int driver, int bpp, int width, int height)
{
    m_WinContext = wincontext;
    m_Fullscreen = fullscreen;
    m_Bpp = bpp;
    m_Width = width;
    m_Height = height;
    m_ScreenMode = driver;
}

void CNeMoContext::SetWindow(CWinContext *wincontext, bool fullscreen, int bpp, int width, int height)
{
    m_WinContext = wincontext;
    m_Fullscreen = fullscreen;
    m_Bpp = bpp;
    m_Width = width;
    m_Height = height;
}

void CNeMoContext::GetResolution(int &width, int &height)
{
    width = m_Width;
    height = m_Height;
}

void CNeMoContext::SetResolution(int width, int height)
{
    m_Width = width;
    m_Height = height;
}

int CNeMoContext::GetWidth() const
{
    return m_Width;
}

void CNeMoContext::SetWidth(int width)
{
    m_Width = width;
}

int CNeMoContext::GetHeight() const
{
    return m_Height;
}

void CNeMoContext::SetHeight(int height)
{
    m_Height = height;
}

int CNeMoContext::GetBPP() const
{
    return m_Bpp;
}

void CNeMoContext::SetBPP(int bpp)
{
    m_Bpp = bpp;
}

int CNeMoContext::GetRefreshRate() const
{
    return m_RefreshRate;
}

void CNeMoContext::SetRefreshRate(int fps)
{
    m_RefreshRate = fps;
}

int CNeMoContext::GetDriver() const
{
    return m_Driver;
}

void CNeMoContext::SetDriver(int driver)
{
    m_Driver = driver;
}

int CNeMoContext::GetScreenMode() const
{
    return m_ScreenMode;
}

void CNeMoContext::SetScreenMode(int screenMode)
{
    m_ScreenMode = screenMode;
}

bool CNeMoContext::FindScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
    {
        if (m_CKContext)
            CKCloseContext(m_CKContext);
        m_CKContext = NULL;
        CLogger::Get().Error("Unable to find the specified ScreenMode");
        return false;
    }

    VxDisplayMode *dm = drDesc->DisplayModes;
    const int dmCount = drDesc->DisplayModeCount;

    m_RefreshRate = 0;
    for (int i = 0; i < dmCount; ++i)
    {
        if (dm[i].Width == m_Width &&
            dm[i].Height == m_Height &&
            dm[i].Bpp == m_Bpp)
        {
            if (dm[i].RefreshRate > m_RefreshRate)
                m_RefreshRate = dm[i].RefreshRate;
        }
    }

    m_ScreenMode = -1;
    for (int j = 0; j < dmCount; ++j)
    {
        if (dm[j].Width == m_Width &&
            dm[j].Height == m_Height &&
            dm[j].Bpp == m_Bpp &&
            dm[j].RefreshRate == m_RefreshRate)
        {
            m_ScreenMode = j;
            break;
        }
    }

    return m_ScreenMode >= 0;
}

bool CNeMoContext::ApplyScreenMode()
{
    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
        return false;

    const int dmCount = drDesc->DisplayModeCount;
    if (m_ScreenMode >= dmCount)
        return false;

    VxDisplayMode *dm = &drDesc->DisplayModes[m_ScreenMode];
    m_Width = dm->Width;
    m_Height = dm->Height;
    m_Bpp = dm->Bpp;
    return true;
}

bool CNeMoContext::ChangeScreenMode(int driver, int screenMode)
{
    if (!m_RenderContext)
        return false;

    if (driver == m_Driver && screenMode == m_ScreenMode)
        return true;

    int driverBefore = m_Driver;
    int screenModeBefore = m_ScreenMode;
    m_Driver = driver;
    m_ScreenMode = screenMode;
    if (!ApplyScreenMode())
    {
        m_Driver = driverBefore;
        m_ScreenMode = screenModeBefore;
        ApplyScreenMode();
        return false;
    }

    bool fullscreenBefore = IsRenderFullscreen();
    if (fullscreenBefore)
    {
        StopFullscreen();
        GoFullscreen();
    }
    else
    {
        m_WinContext->SetMainSize(m_Width, m_Height);
        m_WinContext->SetRenderSize(m_Width, m_Height);
        m_RenderContext->Resize();
    }

    return true;
}

bool CNeMoContext::GoFullscreen()
{
    if (!m_RenderContext || IsRenderFullscreen())
        return false;

    VxDriverDesc *drDesc = m_RenderManager->GetRenderDriverDescription(m_Driver);
    if (!drDesc)
        return false;

    VxDisplayMode *dm = &drDesc->DisplayModes[m_ScreenMode];
    m_RenderContext->GoFullScreen(dm->Width, dm->Height, dm->Bpp, m_Driver);

    if (drDesc->Caps2D.Family == CKRST_DIRECTX && m_RenderContext->IsFullScreen())
    {
        m_WinContext->AdjustMainStyle(true);
        m_WinContext->SetPosition(0, 0);
        m_WinContext->SetMainSize(m_Width, m_Height);
        m_WinContext->UpdateWindows();
    }

    m_Fullscreen = true;
    return true;
}

bool CNeMoContext::StopFullscreen()
{
    if (!m_RenderContext || !IsRenderFullscreen())
        return false;

    m_RenderContext->StopFullScreen();
    m_Fullscreen = false;
    return true;
}

bool CNeMoContext::IsRenderFullscreen() const
{
    return m_RenderContext->IsFullScreen() == TRUE;
}

bool CNeMoContext::IsFullscreen() const
{
    return m_Fullscreen;
}

void CNeMoContext::SetFullscreen(bool fullscreen)
{
    m_Fullscreen = fullscreen;
}

void CNeMoContext::ResizeWindow()
{
    int w, h;
    m_WinContext->GetMainSize(w, h);
    m_WinContext->SetRenderSize(w, h);
    m_RenderContext->Resize();
}

bool CNeMoContext::RestoreWindow()
{
    if (!StopFullscreen())
        return false;

    m_WinContext->AdjustMainStyle(false);
    m_WinContext->SetMainSize(m_Width, m_Height);
    m_WinContext->ShowMainWindow();
    m_WinContext->FocusMainWindow();
    m_WinContext->SetRenderSize(m_Width, m_Height);
    m_RenderContext->Resize();
    m_WinContext->FocusRenderWindow();
    m_WinContext->UpdateWindows();
    m_Fullscreen = false;
    return true;
}

void CNeMoContext::MinimizeWindow()
{
    if (!StopFullscreen())
        return;
    m_WinContext->MinimizeMainWindow();
}

void CNeMoContext::SetRenderContext(CKRenderContext *renderContext)
{
    m_RenderContext = renderContext;
}

void CNeMoContext::SetWinContext(CWinContext *winContext)
{
    m_WinContext = winContext;
}

CKContext *CNeMoContext::GetCKContext()
{
    return m_CKContext;
}

CKRenderContext *CNeMoContext::GetRenderContext() const
{
    return m_RenderContext;
}

CKRenderManager *CNeMoContext::GetRenderManager() const
{
    return m_RenderManager;
}

CKInputManager *CNeMoContext::GetInputManager() const
{
    return m_InputManager;
}

InterfaceManager *CNeMoContext::GetInterfaceManager() const
{
    return m_InterfaceManager;
}

bool CNeMoContext::CreateRenderContext()
{
    CKRenderManager *rm = m_CKContext->GetRenderManager();
    CKRECT rect = {0, 0, m_Width, m_Height};
    m_RenderContext = rm->CreateRenderContext(m_WinContext->GetRenderWindow(), m_Driver, &rect, FALSE, m_Bpp, -1, -1, 0);
    if (!m_RenderContext)
        return false;

    if (m_Fullscreen && m_RenderContext->GoFullScreen(m_Width, m_Height, m_Bpp, m_Driver, 0) != CK_OK)
    {
        if (!RestoreWindow())
        {
            CLogger::Get().Error("Cannot switch to windowed mode");
            return false;
        }
        return false;
    }

    if (!m_Fullscreen)
        RestoreWindow();

    m_WinContext->FocusMainWindow();

    ClearScreen();
    RefreshScreen();

    return true;
}

int CNeMoContext::GetRenderEnginePluginIdx()
{
    if (!m_RenderEngine)
        return -1;

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

        char filename[512];
        _splitpath(dllname, NULL, NULL, filename, NULL);
        if (!_strnicmp(m_RenderEngine, filename, strlen(filename)))
            return i;
    }

    return -1;
}

bool CNeMoContext::ParsePlugins(const char *dir)
{
    if (!m_PluginManager)
        return false;
    return m_PluginManager->ParsePlugins((CKSTRING)dir) != 0;
}

void CNeMoContext::AddSoundPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(SOUND_PATH_IDX, str);
}

void CNeMoContext::AddBitmapPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(BITMAP_PATH_IDX, str);
}

void CNeMoContext::AddDataPath(const char *path)
{
    if (!path)
        return;
    XString str = path;
    m_CKContext->GetPathManager()->AddPath(DATA_PATH_IDX, str);
}

CKERROR CNeMoContext::GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo)
{
    return m_CKContext->GetFileInfo(filename, fileinfo);
}

CKERROR CNeMoContext::LoadFile(char *filename, CKObjectArray *liste, CK_LOAD_FLAGS loadFlags, CKGUID *readerGuid)
{
    return m_CKContext->Load(filename, liste, loadFlags, readerGuid);
}

CKObject *CNeMoContext::CreateObject(CK_CLASSID cid, CKSTRING name, CK_OBJECTCREATION_OPTIONS options, CK_CREATIONMODE *res)
{
    return m_CKContext->CreateObject(cid, name, options, res);
}

CKERROR CNeMoContext::DestroyObject(CKObject *obj, CKDWORD flags, CKDependencies *depOptions)
{
    return m_CKContext->DestroyObject(obj, flags, depOptions);
}

CKERROR CNeMoContext::DestroyObject(CK_ID id, CKDWORD flags, CKDependencies *depOptions)
{
    return m_CKContext->DestroyObject(id, flags, depOptions);
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

CKBehavior *CNeMoContext::CreateBB(CKBehavior *beh, CKGUID guid, CKBeObject *target)
{
    CKBehavior *behavior = (CKBehavior *) beh->GetCKContext()->CreateObject(CKCID_BEHAVIOR);
    behavior->InitFromGuid(guid);
    if (target)
    {
        behavior->UseTarget();
        CKBeObject **pt = &target;
        behavior->GetTargetParameter()->GetDirectSource()->SetValue(pt);
    }
    beh->AddSubBehavior(behavior);
    return behavior;
}

CKBehavior *CNeMoContext::GetBehavior(const XObjectPointerArray &array, const char *name, CKBehavior *previous)
{
    CKBehavior *behavior = NULL;
    CKObject **it = array.Begin();

    if (previous)
    {
        for (; it != array.End(); ++it)
        {
            if (*it == previous)
            {
                ++it;
                break;
            }
        }
    }

    for (; it != array.End(); ++it)
    {
        CKObject *obj = *it;
        if (obj->GetClassID() == CKCID_BEHAVIOR && strcmp(obj->GetName(), name) == 0)
        {
            behavior = (CKBehavior *)obj;
            break;
        }
    }

    return behavior;
}

CKBehavior *CNeMoContext::GetBehavior(CKBehavior *beh, const char *name, CKBehavior *previous)
{
    CKBehavior *behavior = NULL;
    const int count = beh->GetSubBehaviorCount();
    int i = 0;

    if (previous)
    {
        for (; i < count; ++i)
        {
            CKBehavior *b = beh->GetSubBehavior(i);
            if (b == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < count; ++i)
    {
        CKBehavior *b = beh->GetSubBehavior(i);
        if (strcmp(b->GetName(), name) == 0)
        {
            behavior = b;
            break;
        }
    }

    return behavior;
}

CKBehavior *CNeMoContext::GetBehavior(CKBehavior *beh, const char *name, const char *targetName, CKBehavior *previous)
{
    CKBehavior *behavior = NULL;
    const int count = beh->GetSubBehaviorCount();
    int i = 0;

    if (previous)
    {
        for (; i < count; ++i)
        {
            CKBehavior *b = beh->GetSubBehavior(i);
            if (b == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < count; ++i)
    {
        CKBehavior *b = beh->GetSubBehavior(i);
        if (strcmp(b->GetName(), name) == 0)
        {
            CKObject *target = b->GetTarget();
            if (target && strcmp(target->GetName(), targetName) == 0)
            {
                behavior = b;
                break;
            }
        }
    }

    return behavior;
}

CKBehaviorLink *CNeMoContext::CreateBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos, int outPos, int delay)
{
    return CreateBehaviorLink(beh, inBeh->GetOutput(inPos), outBeh->GetInput(outPos), delay);
}

CKBehaviorLink *CNeMoContext::CreateBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out, int delay)
{
    CKBehaviorLink *link = (CKBehaviorLink *)m_CKContext->CreateObject(CKCID_BEHAVIORLINK);
    link->SetInBehaviorIO(in);
    link->SetOutBehaviorIO(out);
    link->SetInitialActivationDelay(delay);
    link->ResetActivationDelay();
    beh->AddSubBehaviorLink(link);
    return link;
}

CKBehaviorLink *CNeMoContext::GetBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos, int outPos, CKBehaviorLink *previous)
{
    const int linkCount = beh->GetSubBehaviorLinkCount();
    int i = 0;

    if (previous)
    {
        for (; i < linkCount; ++i)
        {
            CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
            if (link == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < linkCount; ++i)
    {
        CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
        CKBehaviorIO *inIO = link->GetInBehaviorIO();
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        if (inIO->GetOwner() == inBeh && inBeh->GetOutput(inPos) == inIO &&
            outIO->GetOwner() == outBeh && outBeh->GetInput(outPos) == outIO)
            return link;
    }

    return NULL;
}

CKBehaviorLink *CNeMoContext::GetBehaviorLink(CKBehavior *beh, const char *inBehName, CKBehavior *outBeh, int inPos, int outPos, CKBehaviorLink *previous)
{
    const int linkCount = beh->GetSubBehaviorLinkCount();
    int i = 0;

    if (previous)
    {
        for (; i < linkCount; ++i)
        {
            CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
            if (link == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < linkCount; ++i)
    {
        CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
        CKBehaviorIO *inIO = link->GetInBehaviorIO();
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        CKBehavior *inBeh = inIO->GetOwner();
        if (strcmp(inBeh->GetName(), inBehName) == 0 && inBeh->GetOutput(inPos) == inIO &&
            outIO->GetOwner() == outBeh && outBeh->GetInput(outPos) == outIO)
            return link;
    }

    return NULL;
}

CKBehaviorLink *CNeMoContext::GetBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, const char *outBehName, int inPos, int outPos, CKBehaviorLink *previous)
{
    const int linkCount = beh->GetSubBehaviorLinkCount();
    int i = 0;

    if (previous)
    {
        for (; i < linkCount; ++i)
        {
            CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
            if (link == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < linkCount; ++i)
    {
        CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
        CKBehaviorIO *inIO = link->GetInBehaviorIO();
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        CKBehavior *outBeh = outIO->GetOwner();
        if (inIO->GetOwner() == inBeh && inBeh->GetOutput(inPos) == inIO &&
            strcmp(outBeh->GetName(), outBehName) == 0 && outBeh->GetInput(outPos) == outIO)
            return link;
    }

    return NULL;
}

CKBehaviorLink *CNeMoContext::GetBehaviorLink(CKBehavior *beh, const char *inBehName, const char *outBehName, int inPos, int outPos, CKBehaviorLink *previous)
{
    const int linkCount = beh->GetSubBehaviorLinkCount();
    int i = 0;

    if (previous)
    {
        for (; i < linkCount; ++i)
        {
            CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
            if (link == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < linkCount; ++i)
    {
        CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
        CKBehaviorIO *inIO = link->GetInBehaviorIO();
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        CKBehavior *inBeh = inIO->GetOwner();
        CKBehavior *outBeh = outIO->GetOwner();
        if (strcmp(inBeh->GetName(), inBehName) == 0 && inBeh->GetOutput(inPos) == inIO &&
            strcmp(outBeh->GetName(), outBehName) == 0 && outBeh->GetInput(outPos) == outIO)
            return link;
    }

    return NULL;
}

CKBehaviorLink *CNeMoContext::GetBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out, CKBehaviorLink *previous)
{
    const int linkCount = beh->GetSubBehaviorLinkCount();
    int i = 0;

    if (previous)
    {
        for (; i < linkCount; ++i)
        {
            CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
            if (link == previous)
            {
                ++i;
                break;
            }
        }
    }

    for (; i < linkCount; ++i)
    {
        CKBehaviorLink *link = beh->GetSubBehaviorLink(i);
        CKBehaviorIO *inIO = link->GetInBehaviorIO();
        CKBehaviorIO *outIO = link->GetOutBehaviorIO();
        if (inIO == in && outIO == out)
            return link;
    }

    return NULL;
}

CKBehaviorLink *CNeMoContext::RemoveBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos, int outPos, bool destroy)
{
    CKBehaviorLink *link = GetBehaviorLink(beh, inBeh, outBeh, inPos, outPos);
    if (!link)
        return NULL;

    beh->RemoveSubBehaviorLink(link);
    if (destroy)
    {
        DestroyObject(link);
        return NULL;
    }
    return link;
}

CKBehaviorLink *CNeMoContext::RemoveBehaviorLink(CKBehavior *beh, const char *inBehName, CKBehavior *outBeh, int inPos, int outPos, bool destroy)
{
    CKBehaviorLink *link = GetBehaviorLink(beh, inBehName, outBeh, inPos, outPos);
    if (!link)
        return NULL;

    beh->RemoveSubBehaviorLink(link);
    if (destroy)
    {
        DestroyObject(link);
        return NULL;
    }
    return link;
}

CKBehaviorLink *CNeMoContext::RemoveBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, const char *outBehName, int inPos, int outPos, bool destroy)
{
    CKBehaviorLink *link = GetBehaviorLink(beh, inBeh, outBehName, inPos, outPos);
    if (!link)
        return NULL;

    beh->RemoveSubBehaviorLink(link);
    if (destroy)
    {
        DestroyObject(link);
        return NULL;
    }
    return link;
}

CKBehaviorLink *CNeMoContext::RemoveBehaviorLink(CKBehavior *beh, const char *inBehName, const char *outBehName, int inPos, int outPos, bool destroy)
{
    CKBehaviorLink *link = GetBehaviorLink(beh, inBehName, outBehName, inPos, outPos);
    if (!link)
        return NULL;

    beh->RemoveSubBehaviorLink(link);
    if (destroy)
    {
        DestroyObject(link);
        return NULL;
    }
    return link;
}

CKBehaviorLink *CNeMoContext::RemoveBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out, bool destroy)
{
    CKBehaviorLink *link = GetBehaviorLink(beh, in, out);
    if (!link)
        return NULL;

    beh->RemoveSubBehaviorLink(link);
    if (destroy)
    {
        DestroyObject(link);
        return NULL;
    }
    return link;
}

CKMessageType CNeMoContext::AddMessageType(CKSTRING msg)
{
    return m_MessageManager->AddMessageType(msg);
}

CKMessageType CNeMoContext::GetMessageByString(const char *msg)
{
    for (int i = 0; i < m_MessageManager->GetMessageTypeCount(); i++)
        if (!strcmp(m_MessageManager->GetMessageTypeName(i), msg))
            return i;
    return -1;
}

CKMessage *CNeMoContext::SendMessageSingle(int msg, CKBeObject *dest, CKBeObject *sender)
{
    return m_MessageManager->SendMessageSingle(msg, dest, sender);
}

#undef SendMessage

int CNeMoContext::SendMessage(char *targetObject, char *message, int id0, int id1, int id2, int value)
{
    CKLevel *level = GetCurrentLevel();
    CKBeObject *obj = (CKBeObject *)m_CKContext->GetObjectByName(targetObject);
    CKMessageType msgType = GetMessageByString(message);

    if (level && msgType)
    {
        CKMessage *msg = NULL;

        // no target object specified, we send a broadcast message :
        if (!obj)
        {
            msg = m_MessageManager->SendMessageBroadcast(msgType, CKCID_BEOBJECT, level);
            if (!msg)
                return -1;
        } // target object ok, we send  a single message  :
        else
        {
            msg = m_MessageManager->SendMessageSingle(msgType, obj, level);
            if (!msg)
                return -1;
        }

        //////////////////////////////////////////////////////////////////////////
        // we attach our arguments as parameters :

        // id0
        CKParameter *parameter0 = m_CKContext->CreateCKParameterLocal("msg0", CKPGUID_INT, FALSE);
        parameter0->SetValue(&id0);
        msg->AddParameter(parameter0, true);
        // id1
        CKParameter *parameter1 = m_CKContext->CreateCKParameterLocal("msg1", CKPGUID_INT, FALSE);
        parameter1->SetValue(&id1);
        msg->AddParameter(parameter1, true);
        // id2
        CKParameter *parameter2 = m_CKContext->CreateCKParameterLocal("msg2", CKPGUID_INT, FALSE);
        parameter2->SetValue(&id2);
        msg->AddParameter(parameter2, true);
        // the value :
        CKParameter *valuex = m_CKContext->CreateCKParameterLocal("msgValue", CKPGUID_INT, FALSE);
        valuex->SetValue(&value);
        msg->AddParameter(valuex, true);
        return 1;
    }
    return -1;
}

int CNeMoContext::SendMessage(char *targetObject, char *message, int id0, int id1, int id2, float value)
{
    CKLevel *level = GetCurrentLevel();
    CKBeObject *obj = (CKBeObject *)m_CKContext->GetObjectByName(targetObject);
    CKMessageType msgType = GetMessageByString(message);

    if (level && msgType)
    {
        CKMessage *msg = NULL;

        // no target object specified, we send a broadcast message :
        if (!obj)
        {
            msg = m_MessageManager->SendMessageBroadcast(msgType, CKCID_BEOBJECT, level);
            if (!msg)
                return -1;
        } // target object ok, we send  a single message  :
        else
        {
            msg = m_MessageManager->SendMessageSingle(msgType, obj, level);
            if (!msg)
                return -1;
        }

        //////////////////////////////////////////////////////////////////////////
        // we attach our arguments as parameters :

        // id0
        CKParameter *parameter0 = m_CKContext->CreateCKParameterLocal("msg0", CKPGUID_INT, FALSE);
        parameter0->SetValue(&id0);
        msg->AddParameter(parameter0, true);
        // id1
        CKParameter *parameter1 = m_CKContext->CreateCKParameterLocal("msg1", CKPGUID_INT, FALSE);
        parameter1->SetValue(&id1);
        msg->AddParameter(parameter1, true);
        // id2
        CKParameter *parameter2 = m_CKContext->CreateCKParameterLocal("msg2", CKPGUID_INT, FALSE);
        parameter2->SetValue(&id2);
        msg->AddParameter(parameter2, true);
        // the value :
        CKParameter *valuex = m_CKContext->CreateCKParameterLocal("msgValue", CKPGUID_FLOAT, FALSE);
        valuex->SetValue(&value);
        msg->AddParameter(valuex, true);
        return 1;
    }
    return -1;
}

int CNeMoContext::SendMessage(char *targetObject, char *message, int id0, int id1, int id2, char *value)
{
    CKLevel *level = GetCurrentLevel();
    CKBeObject *obj = (CKBeObject *)m_CKContext->GetObjectByName(targetObject);
    CKMessageType msgType = GetMessageByString(message);

    if (level && msgType)
    {
        CKMessage *msg = NULL;

        // no target object specified, we send a broadcast message :
        if (!obj)
        {
            msg = m_MessageManager->SendMessageBroadcast(msgType, CKCID_BEOBJECT, level);
            if (!msg)
                return -1;
        } // target object ok, we send  a single message  :
        else
        {
            msg = m_MessageManager->SendMessageSingle(msgType, obj, level);
            if (!msg)
                return -1;
        }

        //////////////////////////////////////////////////////////////////////////
        // we attach our arguments as parameters :

        // id0
        CKParameter *parameter0 = m_CKContext->CreateCKParameterLocal("msg0", CKPGUID_INT, FALSE);
        parameter0->SetValue(&id0);
        msg->AddParameter(parameter0, true);
        // id1
        CKParameter *parameter1 = m_CKContext->CreateCKParameterLocal("msg1", CKPGUID_INT, FALSE);
        parameter1->SetValue(&id1);
        msg->AddParameter(parameter1, true);
        // id2
        CKParameter *parameter2 = m_CKContext->CreateCKParameterLocal("msg2", CKPGUID_INT, FALSE);
        parameter2->SetValue(&id2);
        msg->AddParameter(parameter2, true);
        // the value :
        CKParameter *valuex = m_CKContext->CreateCKParameterLocal("msgValue", CKPGUID_STRING, FALSE);
        valuex->SetStringValue(value);
        msg->AddParameter(valuex, true);
        return 1;
    }
    return -1;
}

void CNeMoContext::AddCloseMessage()
{
    m_MsgWindowClose = m_MessageManager->AddMessageType("WM_CLOSE");
}

bool CNeMoContext::BroadcastCloseMessage()
{
    return m_MessageManager->SendMessageBroadcast(m_MsgWindowClose, CKCID_BEOBJECT) != NULL;
}

CNeMoContext *CNeMoContext::GetInstance()
{
    return s_Instance;
}

void CNeMoContext::RegisterInstance(CNeMoContext *nemoContext)
{
    s_Instance = nemoContext;
}