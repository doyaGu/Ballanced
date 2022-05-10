#ifndef PLAYER_NEMOCONTEXT_H
#define PLAYER_NEMOCONTEXT_H

class CWinContext;
class CTTInterfaceManager;

class CNeMoContext
{
public:
    CNeMoContext();
    ~CNeMoContext();

    void SetDriverIndex(int idx);
    bool ApplyScreenMode(int idx);
    void SetFullscreen(bool fullscreen);
    void SetBPP(int bpp);
    void SetRefreshRate(int fps);
    void SetProgPath(const char *path);
    void SetMsgClick(int msg);
    void SetRenderContext(CKRenderContext *renderContext);
    void SetFrameRateSprite(CKSpriteText *sprite);
    void SetMadeWithSprite(CKSpriteText *sprite);
    void SetWinContext(CWinContext *winContext);
    void SetResolution(int width, int height);
    void SetWidth(int width);
    void SetHeight(int height);
    int GetDriverIndex() const;
    int GetScreenModeIndex() const;
    bool IsFullscreen() const;
    void GetResolution(int &width, int &height);
    int GetWidth() const;
    int GetHeight() const;
    int GetBPP() const;
    int GetRefreshRate() const;
    char *GetProgPath() const;
    int GetMsgClick() const;
    bool IsRenderFullScreen() const;
    bool DoStartUp();
    void Pause();
    void Play();
    void MinimizeWindow();
    CKERROR Reset();
    CKERROR Render(CK_RENDER_FLAGS flags = CK_RENDER_USECURRENTSETTINGS);
    void Cleanup();
    void Shutdown();
    void GoFullscreen();
    CKERROR Process();
    void Update();
    void SwitchFullscreen();
    bool RestoreWindow();
    bool Init();
    bool CreateRenderContext();
    int GetRenderEnginePluginIdx();
    bool FindScreenMode();
    bool ReInit();
    void SetScreen(CWinContext *wincontext,
                   bool fullscreen,
                   int driver,
                   int bpp,
                   int width,
                   int height);
    void SetWindow(CWinContext *wincontext,
                   bool fullscreen,
                   int bpp,
                   int width,
                   int height);

    CKRenderContext *GetRenderContext() const;
    CKSpriteText *GetFrameRateSprite() const;
    CKSpriteText *GetMadeWithSprite() const;
    bool IsReseted() const;
    CKERROR GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo);
    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);
    CKERROR CNeMoContext::LoadFile(
        char *filename,
        CKObjectArray *liste,
        CK_LOAD_FLAGS loadFlags = CK_LOAD_DEFAULT,
        CKGUID *readerGuid = (CKGUID *)0);
    CKLevel *GetCurrentLevel();
    CK_ID *GetObjectsListByClassID(CK_CLASSID cid);
    CKObject *GetObject(CK_ID objID);
    CKRenderManager *GetRenderManager();
    int GetObjectsCountByClassID(CK_CLASSID cid);
    CKMessageType AddMessageType(CKSTRING msg);
    bool ParsePlugins(CKSTRING dir);
    CTTInterfaceManager *GetInterfaceManager();
    CKMessage *SendMessageSingle(
        int msg,
        CKBeObject *dest,
        CKBeObject *sender = NULL);
    bool IsPlaying() const;
    bool ChangeScreenMode(int driver, int screenMode);
    bool SendMessageWindowCloseToAll();
    void AddMessageWindowClose();
    CKContext *GetCKContext();
    void Refresh();
    void ResizeWindow();

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

    CKMessageType m_MsgWindowClose;
    CKContext *m_CKContext;
    CKRenderManager *m_RenderManager;
    CKTimeManager *m_TimeManager;
    CKPluginManager *m_PluginManager;
    CKMessageManager *m_MessageManager;
    CKRenderContext *m_RenderContext;
    CWinContext *m_WinContext;
    CKSTRING m_RenderEngine;
    int m_Width;
    int m_Height;
    int m_Bpp;
    int m_RefreshRate;
    bool m_Fullscreen;
    bool m_DisplayChanged;
    int m_DriverIndex;
    int m_ScreenModeIndex;
    char m_ProgPath[512];
    int m_MsgClick;

    static CNeMoContext *instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */