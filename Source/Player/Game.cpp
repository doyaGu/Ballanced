#include "Game.h"

#include <stdio.h>
#include <string.h>

#include "GameConfig.h"
#include "NeMoContext.h"
#include "Logger.h"
#include "InterfaceManager.h"

static bool EditScriptDefaultLevel()
{
    CNeMoContext *nc = CNeMoContext::GetInstance();
    assert(nc != NULL);

    const CGameConfig &config = CGameConfig::Get();

    CKLevel *level = nc->GetCurrentLevel();
    if (!level)
        return false;

    CKBehavior *defaultLevel = nc->GetBehavior(level->ComputeObjectList(CKCID_BEHAVIOR), "Default Level");
    if (!defaultLevel)
        return false;

    // Set debug mode
    {
        CKBehavior *sd = nc->GetBehavior(defaultLevel, "set DebugMode");
        if (!sd)
            return false;

        CKBehavior *bs = nc->GetBehavior(sd, "Binary Switch");
        if (!bs)
            return false;

        CKParameterLocal *local = sd->CreateLocalParameter("Condition", CKPGUID_BOOL);
        CKBOOL debug = config.debug ? TRUE : FALSE;
        local->SetValue(&debug);
        bs->GetInputParameter(0)->SetDirectSource(local);
    }

    // Bypass "Set Language" script and set our language id
    {
        CKBehavior *sl = nc->GetBehavior(defaultLevel, "Set Language");
        if (!sl)
            return false;

        CKBehavior *rr = nc->GetBehavior(sl, "TT_ReadRegistry");
        if (!rr)
            return false;

        CKBehaviorLink *linkOpRr = nc->RemoveBehaviorLink(sl, "Op", rr, 0, 0);
        if (!linkOpRr)
            return false;
        CKBehavior *op1 = linkOpRr->GetInBehaviorIO()->GetOwner();
        nc->DestroyObject(linkOpRr);

        CKBehaviorLink *linkRrOp = nc->RemoveBehaviorLink(sl, rr, "Op", 0, 0);
        if (!linkRrOp)
            return false;
        CKBehavior *op2 = linkRrOp->GetOutBehaviorIO()->GetOwner();
        *(int *)op2->GetInputParameterReadDataPtr(0) = config.langId;
        nc->DestroyObject(linkRrOp);

        nc->CreateBehaviorLink(sl, op1, op2, 0, 0);
    }

    int i;

    CKBehavior *sm = nc->GetBehavior(defaultLevel, "Screen Modes");
    if (!sm)
        return false;

    CKBehavior *filters[3] = {NULL, NULL, NULL};
    for (i = 0; i < sm->GetSubBehaviorCount(); ++i)
    {
        CKBehavior *beh = sm->GetSubBehavior(i);
        if (strcmp(beh->GetName(), "Remove Row If") == 0)
        {
            int val = *(int *)beh->GetInputParameterReadDataPtr(2);
            switch (val)
            {
            case 640: // Minimum width
                filters[0] = beh;
                break;
            case 1600: // Maximum width
                filters[1] = beh;
                break;
            case 16: // BBP filter
                filters[2] = beh;
                break;
            default:
                break;
            }
        }
    }
    for (i = 0; i < 3; ++i)
        if (!filters[i])
            return false;

    // Correct the bbp filter
    int *bpp = (int *)filters[2]->GetInputParameterReadDataPtr(2);
    *bpp = nc->GetBPP();

    // Unlock widescreen (Not 4:3)
    if (config.unlockWidescreen)
    {
        CKBehavior *ic = nc->GetBehavior(sm, "Insert Column", "ScreenModes");
        if (!ic)
            return false;

        CKBehaviorLink *linkScIc = nc->RemoveBehaviorLink(sm, "Set Cell", ic, 0, 0);
        if (!linkScIc)
            return false;
        CKBehavior *sc = linkScIc->GetInBehaviorIO()->GetOwner();
        sm->RemoveSubBehaviorLink(linkScIc);
        nc->DestroyObject(linkScIc);

        nc->RemoveBehaviorLink(sm, "Remove Column", filters[0], 0, 0, true);

        nc->CreateBehaviorLink(sm, sc, filters[0], 0, 0);
    }

    // Unlock high resolution
    if (config.unlockHighResolution)
    {
        const char *inBehName = (config.unlockWidescreen) ? "Set Cell" : "Remove Column";
        CKBehaviorLink *linkRri = nc->RemoveBehaviorLink(sm, inBehName, filters[0], 0, 0);
        if (!linkRri)
            return false;
        CKBehavior *inBeh = linkRri->GetInBehaviorIO()->GetOwner();
        nc->DestroyObject(linkRri);

        nc->RemoveBehaviorLink(sm, filters[1], filters[2], 0, 0, true);

        nc->CreateBehaviorLink(sm, inBeh, filters[2], 0, 0);
    }

    CKBehavior *sts = nc->GetBehavior(defaultLevel, "Synch to Screen");
    if (!sts)
        return false;

    // Unlock frame rate limitation
    if (config.unlockFramerate)
    {
        for (i = 0; i < sts->GetSubBehaviorCount(); ++i)
        {
            CKBehavior *beh = sts->GetSubBehavior(i);
            if (strcmp(beh->GetName(), "Time Settings") == 0)
            {
                CKDWORD *frameRate = (CKDWORD *)beh->GetInputParameterReadDataPtr(0);
                if (*frameRate == 3) // Frame Rate == Limit
                {
                    *frameRate = 1; // Frame Rate = Free
                    break;
                }
            }
        }
    }

    // Make it not to test 640x480 resolution
    {
        CKBehavior *ii = nc->GetBehavior(sts, "Iterator If", "ScreenModes");
        CKBehavior *delayer = nc->GetBehavior(sts, "Delayer");
        if (!ii || !delayer)
            return false;

        nc->RemoveBehaviorLink(sts, ii, "TT Change ScreenMode", 1, 0, true);
        nc->RemoveBehaviorLink(sts, "Show Mouse Cursor", delayer, 0, 0, true);
        nc->CreateBehaviorLink(sts, ii, delayer, 1, 0);
    }

    // Skip Opening Animation
    if (config.skipOpening)
    {
        CKBehavior *is = nc->GetBehavior(defaultLevel, "Intro Start");
        CKBehavior *ie = nc->GetBehavior(defaultLevel, "Intro Ende");
        CKBehavior *ml = nc->GetBehavior(defaultLevel, "Main Loading");
        CKBehavior *ps = nc->GetBehavior(defaultLevel, "Preload Sound");
        if (!is || !ie || !ml || !ps)
            return false;

        CKBehaviorLink *linkStsIs = nc->GetBehaviorLink(defaultLevel, sts, is, 0, 0);
        if (!linkStsIs)
            return false;
        linkStsIs->SetOutBehaviorIO(ml->GetInput(0));

        CKBehaviorLink *linkIeAs = nc->GetBehaviorLink(defaultLevel, ie, "Activate Script", 0, 0);
        if (!linkIeAs)
            return false;
        CKBehaviorLink *linkPsWfa = nc->GetBehaviorLink(defaultLevel, ps, "Wait For All", 0, 0);
        if (!linkPsWfa)
            return false;
        linkPsWfa->SetOutBehaviorIO(linkIeAs->GetOutBehaviorIO());
    }

    return true;
}

static int SetAsActiveCamera(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
        return CKBR_OWNERERROR;

    if (strcmp(cam->GetName(), "Cam_MenuLevel") == 0 ||
        strcmp(cam->GetName(), "InGameCam") == 0)
    {
        const int w = behcontext.CurrentRenderContext->GetWidth();
        const int h = behcontext.CurrentRenderContext->GetHeight();
        cam->SetAspectRatio(w, h);
        cam->SetFov((float)(0.75 * w / h));
        behcontext.CurrentScene->SetObjectInitialValue(cam, CKSaveObjectState(cam));
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    if (behcontext.CurrentRenderContext)
        behcontext.CurrentRenderContext->AttachViewpointToCamera(cam);

    return CKBR_OK;
}

CGame::CGame() : m_GameInfo(NULL) {}

CGame::~CGame()
{
    if (m_GameInfo)
        delete m_GameInfo;
    m_GameInfo = NULL;
}

bool CGame::Load()
{
    CGameConfig &config = CGameConfig::Get();
    CNeMoContext *nc = CNeMoContext::GetInstance();
    assert(nc != NULL);

    if (!m_GameInfo)
    {
        CLogger::Get().Error("gameInfo is NULL: CMO is not loaded");
        return false;
    }

    char cmoPath[MAX_PATH];
    _snprintf(cmoPath, MAX_PATH, "%s%s\\%s", config.GetPath(eRootPath), m_GameInfo->path, m_GameInfo->fileName);
    FILE *fp = fopen(cmoPath, "r");
    if (!fp)
    {
        CLogger::Get().Error("Failed to open the cmo file");
        return false;
    }
    fclose(fp);

    nc->Cleanup();

    if (nc->Render(CK_RENDER_BACKGROUNDSPRITES) != CK_OK)
    {
        CLogger::Get().Error("Exception on Render() before CKLoad()");
        return false;
    }

    CKObjectArray *array = CreateCKObjectArray();
    if (!array)
    {
        CLogger::Get().Error("CreateCKObjectArray() Failed");
        return false;
    }

    // Load the file and fills the array with loaded objects
    if (nc->LoadFile(cmoPath, array) != CK_OK)
    {
        CLogger::Get().Error("LoadFile() Failed");
        return false;
    }

    DeleteCKObjectArray(array);

    return FinishLoad();
}

CGameInfo *CGame::NewGameInfo()
{
    m_GameInfo = new CGameInfo();
    return m_GameInfo;
}

CGameInfo *CGame::GetGameInfo() const
{
    return m_GameInfo;
}

void CGame::SetGameInfo(CGameInfo *gameInfo)
{
    m_GameInfo = gameInfo;
}

bool CGame::FinishLoad()
{
    CNeMoContext *nc = CNeMoContext::GetInstance();
    assert(nc != NULL);

    const CGameConfig &config = CGameConfig::Get();

    // Retrieve the level
    CKLevel *level = nc->GetCurrentLevel();
    if (!level)
    {
        CLogger::Get().Error("GetCurrentLevel() Failed");
        return false;
    }

    int i;
    const int sceneCount = level->GetSceneCount();
    for (i = 0; i < sceneCount; ++i)
        level->GetScene(i)->SetBackgroundColor(0);

    CKRenderContext *rc = nc->GetRenderContext();
    level->AddRenderContext(rc, TRUE);

    nc->RefreshScreen();

    EditScriptDefaultLevel();

    if (config.adaptiveCamera)
    {
        CKBehaviorPrototype *setAsActiveCameraProto = CKGetPrototypeFromGuid(CKGUID(0x368f0ab1, 0x2d8957e4));
        setAsActiveCameraProto->SetFunction(&SetAsActiveCamera);
    }

    // Set the initial conditions for the level
    level->LaunchScene(NULL);

    // Render the first frame
    rc->Render();

    return true;
}

void CGame::SyncCamerasWithScreen()
{
    if (CGameConfig::Get().adaptiveCamera)
    {
        CNeMoContext *nc = CNeMoContext::GetInstance();
        assert(nc != NULL);

        CKRenderContext *rc = nc->GetRenderContext();
        if (!rc)
            return;

        CKCamera *cams[2] = {
            (CKCamera *)nc->GetObjectByNameAndClass("Cam_MenuLevel", CKCID_TARGETCAMERA),
            (CKCamera *)nc->GetObjectByNameAndClass("InGameCam", CKCID_TARGETCAMERA)};
        for (int i = 0; i < 2; ++i)
        {
            if (cams[i])
            {
                cams[i]->SetAspectRatio(rc->GetWidth(), rc->GetHeight());
                cams[i]->SetFov((float)(0.75 * rc->GetWidth() / rc->GetHeight()));
                nc->GetCurrentScene()->SetObjectInitialValue(cams[i], CKSaveObjectState(cams[i]));
            }
        }
    }
}