#ifndef PLAYER_NEMOCONTEXT_H
#define PLAYER_NEMOCONTEXT_H

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "CKAll.h"

#include "config.h"

class CWinContext;
class CTTInterfaceManager;

class PLAYER_EXPORT CNeMoContext
{
public:
    CNeMoContext();
    ~CNeMoContext();

    CKERROR Init();
    bool ReInit();

    bool StartUp();
    void Shutdown();

    void Play();
    void Pause();
    void Reset();
    void Cleanup();

    bool IsPlaying() const;
    bool IsReseted() const;

    void Update();
    CKERROR Process();
    CKERROR Render(CK_RENDER_FLAGS flags = CK_RENDER_USECURRENTSETTINGS);
    void Refresh();

    void SetScreen(CWinContext *wincontext,
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
        m_ScreenMode = driver;
    }

    void SetWindow(CWinContext *wincontext,
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

    void SetResolution(int width, int height)
    {
        m_Width = width;
        m_Height = height;
    }

    void GetResolution(int &width, int &height)
    {
        width = m_Width;
        height = m_Height;
    }

    void SetWidth(int width)
    {
        m_Width = width;
    }

    int GetWidth() const
    {
        return m_Width;
    }

    void SetHeight(int height)
    {
        m_Height = height;
    }

    int GetHeight() const
    {
        return m_Height;
    }

    void SetBPP(int bpp)
    {
        m_Bpp = bpp;
    }

    int GetBPP() const
    {
        return m_Bpp;
    }

    void SetRefreshRate(int fps)
    {
        m_RefreshRate = fps;
    }

    int GetRefreshRate() const
    {
        return m_RefreshRate;
    }

    void SetDriver(int driver)
    {
        m_Driver = driver;
    }

    int GetDriver() const
    {
        return m_Driver;
    }

    bool FindScreenMode();
    bool ApplyScreenMode(int screenMode);
    bool ChangeScreenMode(int driver, int screenMode);
    int GetScreenMode() const
    {
        return m_ScreenMode;
    }

    void GoFullscreen();
    void SwitchFullscreen();
    bool IsRenderFullscreen() const;
    void SetFullscreen(bool fullscreen)
    {
        m_Fullscreen = fullscreen;
    }
    bool IsFullscreen() const
    {
        return m_Fullscreen;
    }

    void ResizeWindow();
    bool RestoreWindow();
    void MinimizeWindow();

    void SetRenderContext(CKRenderContext *renderContext)
    {
        m_RenderContext = renderContext;
    }

    void SetWinContext(CWinContext *winContext)
    {
        m_WinContext = winContext;
    }

    CKContext *GetCKContext()
    {
        return m_CKContext;
    }

    CKRenderContext *GetRenderContext() const
    {
        return m_RenderContext;
    }

    CKRenderManager *GetRenderManager()
    {
        return m_RenderManager;
    }

    CTTInterfaceManager *GetInterfaceManager()
    {
        return m_InterfaceManager;
    }

    bool CreateRenderContext();
    int GetRenderEnginePluginIdx();
    bool ParsePlugins(CKSTRING dir);

    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);

    void SetProgPath(const char *path);
    char *GetProgPath() const;

    CKERROR GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo);
    CKERROR LoadFile(
        char *filename,
        CKObjectArray *liste,
        CK_LOAD_FLAGS loadFlags = CK_LOAD_DEFAULT,
        CKGUID *readerGuid = (CKGUID *)0);

    CKObject *GetObject(CK_ID objID);
    CKObject *GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, CKObject *previous = NULL);
    CK_ID *GetObjectsListByClassID(CK_CLASSID cid);
    int GetObjectsCountByClassID(CK_CLASSID cid);
    CKDataArray *GetArrayByName(CKSTRING name);
    CKGroup *GetGroupByName(CKSTRING name);
    CKMaterial *GetMaterialByName(CKSTRING name);
    CKMesh *GetMeshByName(CKSTRING name);
    CK2dEntity *Get2dEntityByName(CKSTRING name);
    CK3dEntity *Get3dEntityByName(CKSTRING name);
    CK3dObject *Get3dObjectByName(CKSTRING name);
    CKCamera *GetCameraByName(CKSTRING name);
    CKTargetCamera *GetTargetCameraByName(CKSTRING name);
    CKLight *GetLightByName(CKSTRING name);
    CKTargetLight *GetTargetLightByName(CKSTRING name);
    CKSound *GetSoundByName(CKSTRING name);
    CKTexture *GetTextureByName(CKSTRING name);
    CKBehavior *GetScriptByName(CKSTRING name);

    CKLevel *GetCurrentLevel();
    CKScene *GetCurrentScene();

    void SetIC(CKBeObject *obj, bool hierarchy);
    void RestoreIC(CKBeObject *obj, bool hierarchy);

    CKMessageType AddMessageType(CKSTRING msg);
    CKMessage *SendMessageSingle(
        int msg,
        CKBeObject *dest,
        CKBeObject *sender = NULL);

    void AddCloseMessage();
    bool BroadcastCloseMessage();

    static CNeMoContext *GetInstance()
    {
        return instance;
    }

    static void RegisterInstance(CNeMoContext *nemoContext)
    {
        instance = nemoContext;
    }

private:
    CNeMoContext(const CNeMoContext &);
    CNeMoContext &operator=(const CNeMoContext &);

    CKContext *m_CKContext;
    CKRenderManager *m_RenderManager;
    CKBehaviorManager *m_BehaviorManager;
    CKParameterManager *m_ParameterManager;
    CKAttributeManager *m_AttributeManager;
    CKPathManager *m_PathManager;
    CKMessageManager *m_MessageManager;
    CKTimeManager *m_TimeManager;
    CKPluginManager *m_PluginManager;
    CKSoundManager *m_SoundManager;
    CKInputManager *m_InputManager;
    CKCollisionManager *m_CollisionManager;
    CTTInterfaceManager *m_InterfaceManager;
    CKRenderContext *m_RenderContext;
    CKDebugContext *m_DebugContext;
    CWinContext *m_WinContext;
    CKSTRING m_RenderEngine;
    int m_Width;
    int m_Height;
    int m_Bpp;
    int m_RefreshRate;
    bool m_Fullscreen;
    bool m_DisplayChanged;
    int m_Driver;
    int m_ScreenMode;
    char m_ProgPath[512];
    CKMessageType m_MsgWindowClose;
    BOOL m_DebugMode;
    BOOL m_Debugging;

    static CNeMoContext *instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */