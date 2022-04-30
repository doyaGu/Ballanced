#ifndef PLAYER_NEMOCONTEXT_H
#define PLAYER_NEMOCONTEXT_H

#include <exception>

#include "FixedString.h"

class CWinContext;
class CTTInterfaceManager;

class CNeMoContextException : public std::exception
{
public:
    CNeMoContextException() : std::exception(), error(0) {}
    CNeMoContextException(int err) : std::exception(), error(err) {}
    int error;
};

class CNeMoContext
{
public:
    CNeMoContext();
    virtual ~CNeMoContext() {}

    virtual float GetField0x4C() const;
    virtual void SetField0x4C(float val);
    virtual void SetFrameRateSpritePosition(int x, int y);
    virtual void SetMadeWithSpritePosition(int x, int y);
    virtual void SetFrameRateSpriteText(CKSTRING text);
    virtual void SetMadeWithSpriteText(CKSTRING text);
    virtual bool MoveFrameRateSpriteToLeftTop();
    virtual void SetStartTime(int time);
    virtual void SetField0x25C(bool val);
    virtual void SetCleared(bool clear);
    virtual int GetStartTime() const;
    virtual int GetField0x25C() const;
    virtual bool IsCleared() const;
    virtual void SetDriverIndex(int idx);
    virtual bool SetScreenMode(int idx);
    virtual void SetFullscreen(bool fullscreen);
    virtual void SetColorBPP(int bpp);
    virtual void SetField0x30(int val);
    virtual void SetProgPath(const char *path);
    virtual void SetField0x254(int val);
    virtual void SetMsgClick(int msg);
    virtual void SetRenderContext(CKRenderContext *renderContext);
    virtual void SetFrameRateSprite(CKSpriteText *sprite);
    virtual void SetMadeWithSprite(CKSpriteText *sprite);
    virtual void SetWinContext(CWinContext *winContext);
    virtual void SetResolution(int width, int height);
    virtual void SetWidth(int width);
    virtual void SetHeight(int height);
    virtual int GetDriverIndex() const;
    virtual int GetScreenModeIndex() const;
    virtual bool IsFullscreen() const;
    virtual void GetResolution(int &width, int &height);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetColorBPP() const;
    virtual int GetField0x30() const;
    virtual char *GetProgPath() const;
    virtual int GetField0x254() const;
    virtual int GetMsgClick() const;
    virtual bool IsRenderFullScreen() const;
    virtual void DoStartUp();
    virtual void Pause();
    virtual void Play();
    virtual void ShowWindow();
    virtual CKERROR Reset();
    virtual CKERROR Render(CK_RENDER_FLAGS flags);
    virtual void HideFrameRateSprite();
    virtual void ShowFrameRateSprite();
    virtual void HideMadeWithSprite();
    virtual void ShowMadeWithSprite();
    virtual void Cleanup();
    virtual void Shutdown();
    virtual void GoFullscreen();
    virtual void Process();
    virtual void Update(int state);
    virtual void SwitchFullscreen();
    virtual bool RestoreWindow();
    virtual bool Init();
    virtual void CreateInterfaceSprite();
    virtual bool CreateRenderContext();
    virtual int GetRenderEnginePluginIdx();
    virtual bool FindScreenMode();
    virtual bool ReInit();
    virtual void SetScreen(CWinContext *wincontext,
                           bool fullscreen,
                           int driver,
                           int bpp,
                           int width,
                           int height);
    virtual void SetWindow(CWinContext *wincontext,
                           bool fullscreen,
                           int bpp,
                           int width,
                           int height);

    CKRenderContext *GetRenderContext() const;
    CKSpriteText *GetFrameRateSprite() const;
    CKSpriteText *GetMadeWithSprite() const;
    bool IsReseted() const;
    void DestroyMadeWithSprite();
    void DestroyFrameRateSprite();
    CKERROR GetFileInfo(CKSTRING filename, CKFileInfo *fileinfo);
    void AddSoundPath(const char *path);
    void AddBitmapPath(const char *path);
    void AddDataPath(const char *path);
    CKERROR CNeMoContext::LoadFile(
        char *filename,
        CKObjectArray *liste,
        CK_LOAD_FLAGS flags,
        CKGUID *readerGuid);
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
    bool SwitchScreenMode(int driver, int screenMode);
    bool SendMessageWindowCloseToAll();
    void AddMessageWindowClose();
    CKContext *GetCKContext();

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
    CKSpriteText *m_FrameRateSprite;
    CKSpriteText *m_MadeWithSprite;
    CWinContext *m_WinContext;
    int m_ColorBPP;
    int field_30;
    CFixedString m_RenderEnginePath;
    int m_Width;
    int m_Height;
    bool m_Fullscreen;
    bool m_DisplayChanged;
    int m_DriverIndex;
    int m_ScreenModeIndex;
    float field_4C;
    DWORD m_StartTime;
    char m_ProgPath[512];
    int field_254;
    int m_MsgClick;
    bool field_25C;
    bool m_IsCleared;

    static CNeMoContext *instance;
};

#endif /* PLAYER_NEMOCONTEXT_H */