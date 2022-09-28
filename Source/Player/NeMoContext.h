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

    void SetScreen(CWinContext *wincontext, bool fullscreen, int driver, int bpp, int width, int height);

    void SetWindow(CWinContext *wincontext, bool fullscreen, int bpp, int width, int height);

    void GetResolution(int &width, int &height);

    void SetResolution(int width, int height);

    int GetWidth() const;

    void SetWidth(int width);

    int GetHeight() const;

    void SetHeight(int height);

    int GetBPP() const;

    void SetBPP(int bpp);

    int GetRefreshRate() const;

    void SetRefreshRate(int fps);

    int GetDriver() const;

    void SetDriver(int driver);

    int GetScreenMode() const;

    void SetScreenMode(int screenMode);

    bool FindScreenMode();
    bool ApplyScreenMode();
    bool ChangeScreenMode(int driver, int screenMode);

    bool GoFullscreen();
    bool StopFullscreen();
    bool IsRenderFullscreen() const;

    bool IsFullscreen() const;

    void SetFullscreen(bool fullscreen);

    void ResizeWindow();
    bool RestoreWindow();
    void MinimizeWindow();

    void SetRenderContext(CKRenderContext *renderContext);

    void SetWinContext(CWinContext *winContext);

    CKContext *GetCKContext();

    CKRenderContext *GetRenderContext() const;
    CKRenderManager *GetRenderManager() const;

    CKTimeManager *GetTimeManager() const;
    CKInputManager *GetInputManager() const;
    InterfaceManager *GetInterfaceManager() const;

    CKERROR CreateContext();
    bool CreateRenderContext();
    int GetRenderEnginePluginIdx();
    bool ParsePlugins(const char *dir);

    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);

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

    CKBehavior *CreateBehavior(CKBehavior *script, CKGUID guid, CKBeObject *target = NULL);

    void LinkBehavior(CKBehavior *script, CKBehaviorLink *link, CKBehavior *beh, int inPos, int outPos);

    void RemoveBehavior(CKBehavior *script, CKBehavior *beh, bool destroy = false);

    CKBehavior *GetBehavior(const XObjectPointerArray &array, const char *name, CKBehavior *previous = NULL);
    CKBehavior *GetBehavior(CKBehavior *script, const char *name, CKBehavior *previous = NULL);
    CKBehavior *GetBehavior(CKBehavior *script, const char *name, const char *targetName, CKBehavior *previous = NULL);

    CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name, CKGUID type);

    template<typename T>
    CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name, CKGUID guid, T value)
    {
        CKParameterLocal *param = CreateLocalParameter(script, name, guid);
        param->SetValue(&value, sizeof(T));
        return param;
    }

    template<>
    CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name, CKGUID guid, CKObject *value)
    {
        return CreateLocalParameter(script, name, guid, CKOBJID(value));
    }

    CKParameterLocal *CreateLocalParameter(CKBehavior *script, const char *name, const char *value);

    CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, int delay = 0);
    CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, int delay = 0);
    CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehaviorIO *out, int inPos, int delay = 0);
    CKBehaviorLink *CreateBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehavior *outBeh, int outPos, int delay = 0);

    CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, CKBehaviorLink *previous = NULL);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *script, const char *inBehName, CKBehavior *outBeh, int inPos = 0, int outPos = 0, CKBehaviorLink *previous = NULL);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehavior *inBeh, const char *outBehName, int inPos = 0, int outPos = 0, CKBehaviorLink *previous = NULL);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *script, const char *inBehName, const char *outBehName, int inPos = 0, int outPos = 0, CKBehaviorLink *previous = NULL);
    CKBehaviorLink *GetBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, CKBehaviorLink *previous = NULL);

    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehavior *inBeh, CKBehavior *outBeh, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, const char *inBehName, CKBehavior *outBeh, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehavior *inBeh, const char *outBehName, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, const char *inBehName, const char *outBehName, int inPos = 0, int outPos = 0, bool destroy = false);
    CKBehaviorLink *RemoveBehaviorLink(CKBehavior *script, CKBehaviorIO *in, CKBehaviorIO *out, bool destroy = false);

    CKMessageType AddMessageType(CKSTRING msg);
    CKMessageType GetMessageByString(const char *msg);

    CKMessage *SendMessageSingle(int msg, CKBeObject *dest, CKBeObject *sender = NULL);

    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, int value);
    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, float value);
    int SendMessage(char *targetObject, char *message, int id0, int id1, int id2, char *value);

    void AddCloseMessage();
    bool BroadcastCloseMessage();

    static CNeMoContext *GetInstance();

    static void RegisterInstance(CNeMoContext *nemoContext);

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
    CKMessageType m_MsgWindowClose;

    static CNeMoContext *s_Instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */