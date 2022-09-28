#include "Game.h"

#include <stdio.h>
#include <string.h>

#include "GameConfig.h"
#include "NeMoContext.h"
#include "Logger.h"
#include "InterfaceManager.h"

static void EditScriptDefaultLevel()
{
    CNeMoContext *nc = CNeMoContext::GetInstance();
    assert(nc != NULL);

    const CGameConfig &config = CGameConfig::Get();

    CKLevel *level = nc->GetCurrentLevel();
    if (!level)
    {
        CLogger::Get().Error("No level!");
        return;
    }

    CKBehavior *defaultLevel = nc->GetBehavior(level->ComputeObjectList(CKCID_BEHAVIOR), "Default Level");
    if (!defaultLevel)
    {
        CLogger::Get().Error("Unable to find script Default Level");
        return;
    }

    bool success = false;

    // Set debug mode
    if (config.debug)
    {
        CKBehavior *sd = nc->GetBehavior(defaultLevel, "set DebugMode");
        if (sd)
        {
            CKBehavior *bs = nc->GetBehavior(sd, "Binary Switch");
            if (bs)
            {
                CKParameterLocal *local = sd->CreateLocalParameter("Condition", CKPGUID_BOOL);
                CKBOOL debug = TRUE;
                local->SetValue(&debug);
                bs->GetInputParameter(0)->SetDirectSource(local);

                success = true;
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to set debug mode");
        success = false;
    }

    // Bypass "Set Language" script and set our language id
    {
        CKBehavior *sl = nc->GetBehavior(defaultLevel, "Set Language");
        if (sl)
        {
            CKBehavior *rr = nc->GetBehavior(sl, "TT_ReadRegistry");
            if (rr)
            {
                CKBehaviorLink *linkOpRr = nc->RemoveBehaviorLink(sl, "Op", rr, 0, 0);
                if (linkOpRr)
                {
                    CKBehavior *op1 = linkOpRr->GetInBehaviorIO()->GetOwner();
                    nc->DestroyObject(linkOpRr);

                    CKBehaviorLink *linkRrOp = nc->RemoveBehaviorLink(sl, rr, "Op", 0, 0);
                    if (linkRrOp)
                    {
                        CKBehavior *op2 = linkRrOp->GetOutBehaviorIO()->GetOwner();
                        *(int *)op2->GetInputParameterReadDataPtr(0) = config.langId;
                        nc->DestroyObject(linkRrOp);

                        nc->CreateBehaviorLink(sl, op1, op2, 0, 0);

                        success = true;
                    }
                }
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to set language id");
        success = false;
    }

    int i;

    CKBehavior *sm = nc->GetBehavior(defaultLevel, "Screen Modes");
    if (!sm)
    {
        CLogger::Get().Error("Unable to find script Screen Modes");
        return;
    }

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

    // Correct the bbp filter
    if (filters[2])
    {
        int *bpp = (int *)filters[2]->GetInputParameterReadDataPtr(2);
        *bpp = nc->GetBPP();
    }
    else
    {
        CLogger::Get().Error("Failed to correct the bbp filter");
    }

    // Unlock widescreen (Not 4:3)
    if (config.unlockWidescreen)
    {
        CKBehavior *ic = nc->GetBehavior(sm, "Insert Column", "ScreenModes");
        if (ic)
        {
            CKBehaviorLink *linkScIc = nc->RemoveBehaviorLink(sm, "Set Cell", ic, 0, 0);
            if (linkScIc)
            {
                CKBehavior *sc = linkScIc->GetInBehaviorIO()->GetOwner();
                sm->RemoveSubBehaviorLink(linkScIc);
                nc->DestroyObject(linkScIc);

                if (filters[0])
                {
                    nc->RemoveBehaviorLink(sm, "Remove Column", filters[0], 0, 0, true);

                    nc->CreateBehaviorLink(sm, sc, filters[0], 0, 0);

                    success = true;
                }
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to unlock widescreen");
        success = false;
    }

    // Unlock high resolution
    if (config.unlockHighResolution)
    {
        const char *inBehName = (config.unlockWidescreen) ? "Set Cell" : "Remove Column";
        CKBehaviorLink *linkRri = nc->RemoveBehaviorLink(sm, inBehName, filters[0], 0, 0);
        if (linkRri)
        {
            CKBehavior *inBeh = linkRri->GetInBehaviorIO()->GetOwner();
            nc->DestroyObject(linkRri);

            if (filters[1] && filters[2])
            {
                nc->RemoveBehaviorLink(sm, filters[1], filters[2], 0, 0, true);

                nc->CreateBehaviorLink(sm, inBeh, filters[2], 0, 0);

                success = true;
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to unlock high resolution");
        success = false;
    }

    CKBehavior *sts = nc->GetBehavior(defaultLevel, "Synch to Screen");
    if (!sts)
    {
        CLogger::Get().Error("Unable to find script Synch to Screen");
        return;
    }

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

                    success = true;
                    break;
                }
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to unlock frame rate limitation");
        success = false;
    }

    // Make it not to test 640x480 resolution
    {
        CKBehavior *ii = nc->GetBehavior(sts, "Iterator If");
        CKBehavior *delayer = nc->GetBehavior(sts, "Delayer");
        if (ii && delayer)
        {
            CKBehaviorLink *linkTsIi1 = nc->GetBehaviorLink(sts, "Time Settings", ii, 0, 0);
            CKBehaviorLink *linkTsIi2 = nc->GetBehaviorLink(sts, "Time Settings", ii, 0, 0, linkTsIi1);
            if (linkTsIi1 && linkTsIi2)
            {
                CKBehavior *ts1 = linkTsIi1->GetInBehaviorIO()->GetOwner();
                CKBehavior *ts2 = linkTsIi2->GetInBehaviorIO()->GetOwner();

                sts->RemoveSubBehaviorLink(linkTsIi1);
                sts->RemoveSubBehaviorLink(linkTsIi2);

                nc->DestroyObject(linkTsIi1);
                nc->DestroyObject(linkTsIi2);

                CKBehaviorLink *linkDelayerCsm = nc->GetBehaviorLink(sts, delayer, "TT Change ScreenMode", 0, 0);
                if (linkDelayerCsm)
                {
                    CKBehavior *csm2 = linkDelayerCsm->GetOutBehaviorIO()->GetOwner();
                    CKBehaviorLink *linkCsmSmc = nc->RemoveBehaviorLink(sts, csm2, "Show Mouse Cursor", 0, 0);
                    if (linkCsmSmc)
                    {
                        CKBehavior *smc2 = linkCsmSmc->GetOutBehaviorIO()->GetOwner();
                        nc->DestroyObject(linkCsmSmc);

                        // TT Change ScreenMode
                        CKBehavior *csm = nc->CreateBehavior(sts, CKGUID(0x38b84d97, 0x13932f28));
                        CKParameterLocal *pin0 = nc->CreateLocalParameter(sts, "Driver ID", CKPGUID_INT, nc->GetDriver());
                        CKParameterLocal *pin1 = nc->CreateLocalParameter(sts, "ScreenMode ID", CKPGUID_INT, nc->GetScreenMode());
                        csm->GetInputParameter(0)->SetDirectSource(pin0);
                        csm->GetInputParameter(1)->SetDirectSource(pin1);

                        nc->CreateBehaviorLink(sts, ts1, csm, 0, 0);
                        nc->CreateBehaviorLink(sts, ts2, csm, 0, 0);
                        nc->CreateBehaviorLink(sts, csm, smc2, 0, 0);

                        success = true;
                    }
                }
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to bypass 640x480 resolution test");
        success = false;
    }

    // Skip Opening Animation
    if (config.skipOpening)
    {
        CKBehavior *is = nc->GetBehavior(defaultLevel, "Intro Start");
        CKBehavior *ie = nc->GetBehavior(defaultLevel, "Intro Ende");
        CKBehavior *ml = nc->GetBehavior(defaultLevel, "Main Loading");
        CKBehavior *ps = nc->GetBehavior(defaultLevel, "Preload Sound");
        if (is && ie && ml && ps)
        {
            CKBehaviorLink *linkStsIs = nc->GetBehaviorLink(defaultLevel, sts, is, 0, 0);
            if (linkStsIs)
            {
                linkStsIs->SetOutBehaviorIO(ml->GetInput(0));

                CKBehaviorLink *linkIeAs = nc->GetBehaviorLink(defaultLevel, ie, "Activate Script", 0, 0);
                if (linkIeAs)
                {
                    CKBehaviorLink *linkPsWfa = nc->GetBehaviorLink(defaultLevel, ps, "Wait For All", 0, 0);
                    if (linkPsWfa)
                    {
                        linkPsWfa->SetOutBehaviorIO(linkIeAs->GetOutBehaviorIO());

                        success = true;
                    }
                }
            }
        }

        if (!success)
            CLogger::Get().Error("Failed to skip opening animation");
        success = false;
    }
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