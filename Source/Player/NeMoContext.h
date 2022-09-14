#ifndef PLAYER_NEMOCONTEXT_H
#define PLAYER_NEMOCONTEXT_H

#include "CKAll.h"

class CWinContext;
class InterfaceManager;

class CNeMoContext
{
public:
    CNeMoContext();
    ~CNeMoContext();

    bool StartUp();
    void Shutdown();

    void Play();
    void Pause();
    void Reset();
    void Cleanup();

    bool IsPlaying() const;
    bool IsReseted() const;

    void Process();
    CKERROR Render(CK_RENDER_FLAGS flags = CK_RENDER_USECURRENTSETTINGS);

    void ClearScreen();
    void RefreshScreen();

    void SetScreen(CWinContext *wincontext, bool fullscreen, int driver, int bpp, int width, int height)
    {
        m_WinContext = wincontext;
        m_Fullscreen = fullscreen;
        m_Bpp = bpp;
        m_Width = width;
        m_Height = height;
        m_ScreenMode = driver;
    }

    void SetWindow(CWinContext *wincontext, bool fullscreen, int bpp, int width, int height)
    {
        m_WinContext = wincontext;
        m_Fullscreen = fullscreen;
        m_Bpp = bpp;
        m_Width = width;
        m_Height = height;
    }

    void GetResolution(int &width, int &height)
    {
        width = m_Width;
        height = m_Height;
    }

    void SetResolution(int width, int height)
    {
        m_Width = width;
        m_Height = height;
    }

    int GetWidth() const
    {
        return m_Width;
    }

    void SetWidth(int width)
    {
        m_Width = width;
    }

    int GetHeight() const
    {
        return m_Height;
    }

    void SetHeight(int height)
    {
        m_Height = height;
    }

    int GetBPP() const
    {
        return m_Bpp;
    }

    void SetBPP(int bpp)
    {
        m_Bpp = bpp;
    }

    int GetRefreshRate() const
    {
        return m_RefreshRate;
    }

    void SetRefreshRate(int fps)
    {
        m_RefreshRate = fps;
    }

    int GetDriver() const
    {
        return m_Driver;
    }

    void SetDriver(int driver)
    {
        m_Driver = driver;
    }

    int GetScreenMode() const
    {
        return m_ScreenMode;
    }

    void SetScreenMode(int screenMode)
    {
        m_ScreenMode = screenMode;
    }

    bool FindScreenMode();
    bool ApplyScreenMode();
    bool ChangeScreenMode(int driver, int screenMode);

    bool GoFullscreen();
    bool StopFullscreen();
    bool IsRenderFullscreen() const;

    bool IsFullscreen() const
    {
        return m_Fullscreen;
    }

    void SetFullscreen(bool fullscreen)
    {
        m_Fullscreen = fullscreen;
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

    CKRenderManager *GetRenderManager() const
    {
        return m_RenderManager;
    }

    CKInputManager *GetInputManager() const
    {
        return m_InputManager;
    }

    InterfaceManager *GetInterfaceManager() const
    {
        return m_InterfaceManager;
    }

    CKERROR CreateContext();
    bool CreateRenderContext();
    int GetRenderEnginePluginIdx();
    bool ParsePlugins(const char *dir);

    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);

    char *GetProgPath();
    void SetProgPath(const char *path);

    CKERROR GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo);
    CKERROR LoadFile(char *filename, CKObjectArray *liste, CK_LOAD_FLAGS loadFlags = CK_LOAD_DEFAULT, CKGUID *readerGuid = NULL);

    CKObject *CreateObject(CK_CLASSID cid, CKSTRING name = NULL, CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_NONAMECHECK, CK_CREATIONMODE *res = NULL);
    CKERROR DestroyObject(CKObject *obj, CKDWORD flags = 0, CKDependencies *depOptions = NULL);
    CKERROR DestroyObject(CK_ID id, CKDWORD flags = 0, CKDependencies *depOptions = NULL);

    CKObject *GetObject(CK_ID objID);
    CKObject *GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, CKObject *previous = NULL);
    CK_ID *GetObjectsListByClassID(CK_CLASSID cid);
    int GetObjectsCountByClassID(CK_CLASSID cid);

    CKLevel *GetCurrentLevel();
    CKScene *GetCurrentScene();

    CKBehavior *GetBehavior(const XObjectPointerArray &array, const char *name);
    CKBehavior *GetBehavior(CKBehavior *beh, const char *name);
    CKBehavior *GetBehavior(CKBehavior *beh, const char *name, const char *targetName);

    CKBehaviorLink *CreateBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, int delay = 0);
    CKBehaviorLink *CreateBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out, int delay = 0);

    CKBehaviorLink *GetBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *beh, const char *inBehName, CKBehavior *outBeh, int inPos = 0, int outPos = 0);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, const char *outBehName, int inPos = 0, int outPos = 0);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out);

    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *beh, const char *inBehName, CKBehavior *outBeh, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *beh, CKBehavior *inBeh, const char *outBehName, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *beh, CKBehaviorIO *in, CKBehaviorIO *out, bool destroy = false);

    CKMessageType AddMessageType(CKSTRING msg);
    CKMessageType GetMessageByString(const char *msg);

    CKMessage *SendMessageSingle(int msg, CKBeObject *dest, CKBeObject *sender = NULL);

    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, int value);
    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, float value);
    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, char *value);

    void AddCloseMessage();
    bool BroadcastCloseMessage();

    static CNeMoContext *GetInstance()
    {
        return s_Instance;
    }

    static void RegisterInstance(CNeMoContext *nemoContext)
    {
        s_Instance = nemoContext;
    }

private:
    CNeMoContext(const CNeMoContext &);
    CNeMoContext &operator=(const CNeMoContext &);

    CKContext *m_CKContext;
    CKRenderManager *m_RenderManager;
    CKAttributeManager *m_AttributeManager;
    CKPathManager *m_PathManager;
    CKMessageManager *m_MessageManager;
    CKTimeManager *m_TimeManager;
    CKPluginManager *m_PluginManager;
    CKInputManager *m_InputManager;
    InterfaceManager *m_InterfaceManager;
    CKRenderContext *m_RenderContext;
    CWinContext *m_WinContext;
    const char *m_RenderEngine;
    int m_Width;
    int m_Height;
    int m_Bpp;
    int m_RefreshRate;
    bool m_Fullscreen;
    int m_Driver;
    int m_ScreenMode;
    char m_ProgPath[512];
    CKMessageType m_MsgWindowClose;

    static CNeMoContext *s_Instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */