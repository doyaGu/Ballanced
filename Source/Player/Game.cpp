#include "Game.h"

#include <io.h>
#include <stdio.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "GameConfig.h"
#include "NeMoContext.h"

#include "TT_InterfaceManager_RT/GameInfo.h"

#define MAX_GAMEDATA 100

static CGameData g_GameData[MAX_GAMEDATA] = {
    CGameData("Clock.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("WM.cmo", "WM", "Software\\Terratools\\WorldMap"),
    CGameData("PBC.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("PB_L0.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("PB_L1.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("PB_L2.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("PB_L3.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("PB_L4.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data"),
    CGameData("SP_INTRO.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
    CGameData("SPC.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
    CGameData("SP_L1.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
    CGameData("SP_L2.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
    CGameData("SP_L3.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
    CGameData("SP_L4.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data"),
};

CGame::CGame() : m_NeMoContext(NULL), m_GameInfo(NULL)
{
    memset(&m_CKFileInfo, 0, sizeof(CKFileInfo));
    memset(m_ProgPath, 0, sizeof(m_ProgPath));
    memset(m_FileName, 0, sizeof(m_FileName));
}

CGame::~CGame()
{
    if (GetGameInfo() != NULL)
    {
        delete m_GameInfo;
    }
    m_GameInfo = NULL;
    SetNeMoContext(NULL);
}

bool CGame::Load(const CGameConfig &config)
{
    char cmoPath[MAX_PATH] = "";
    char dir[MAX_PATH] = "";

    if (!m_GameInfo)
    {
        TT_ERROR("Game.cpp", "Load", "gameInfo is NULL: CMO is not loaded");
        return false;
    }

    strcpy(m_FileName, m_GameInfo->fileName);
    sprintf(m_ProgPath, "%s%s\\", m_NeMoContext->GetProgPath(), m_GameInfo->path);
    sprintf(cmoPath, "%s%s", m_ProgPath, m_GameInfo->fileName);
    FILE *fp = fopen(cmoPath, "r");
    if (!fp)
    {
        TT_ERROR("Game.cpp", "Load", "Failed to open the cmo file");
        return false;
    }
    fclose(fp);

    m_NeMoContext->Cleanup();

    if (m_NeMoContext->Render(CK_RENDER_BACKGROUNDSPRITES) != CK_OK)
    {
        TT_ERROR("Game.cpp", "Load", "Exception on Render() before CKLoad()");
        return false;
    }

    sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), "Sounds");
    if (_access(dir, 0) == -1)
    {
        TT_ERROR("Game.cpp", "Load", "No Sounds directory");
        return false;
    }
    m_NeMoContext->AddSoundPath(dir);

    sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), "Textures");
    if (_access(dir, 0) == -1)
    {
        TT_ERROR("Game.cpp", "Load", "No Textures directory");
        return false;
    }
    m_NeMoContext->AddBitmapPath(dir);

    sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), m_GameInfo->path);
    m_NeMoContext->AddDataPath(dir);

    CKObjectArray *array = CreateCKObjectArray();
    if (!array)
    {
        TT_ERROR("Game.cpp", "Load", "CreateCKObjectArray() Failed");
        return false;
    }

    // Loads the file and fills the array with loaded objects
    if (m_NeMoContext->LoadFile(cmoPath, array) != CK_OK)
    {
        TT_ERROR("Game.cpp", "Load", "LoadFile() Failed");
        return false;
    }

    CKLevel *level = m_NeMoContext->GetCurrentLevel();
    if (!level)
    {
        TT_ERROR("Game.cpp", "Load", "GetCurrentLevel() Failed");
        return false;
    }

    int i;
    int sceneCount = level->GetSceneCount();
    for (i = 0; i < sceneCount; ++i)
        level->GetScene(i)->SetBackgroundColor(0);

    CKRenderContext *renderContext = m_NeMoContext->GetRenderContext();
    level->AddRenderContext(renderContext, TRUE);
    array->Clear();

    // Take the first camera we found and attach the viewpoint to it
    CK_ID *cameraIds = m_NeMoContext->GetObjectsListByClassID(CKCID_CAMERA);
    if (cameraIds || (cameraIds = m_NeMoContext->GetObjectsListByClassID(CKCID_TARGETCAMERA)))
    {
        CKCamera *camera = (CKCamera *)m_NeMoContext->GetObject(cameraIds[0]);
        if (camera)
            renderContext->AttachViewpointToCamera(camera);
    }

    m_NeMoContext->Refresh();

    // Hide curves ?
    int curveCount = m_NeMoContext->GetObjectsCountByClassID(CKCID_CURVE);
    CK_ID *curveIds = m_NeMoContext->GetObjectsListByClassID(CKCID_CURVE);
    for (i = 0; i < curveCount; ++i)
    {
        CKMesh *mesh = ((CKCurve *)m_NeMoContext->GetObject(curveIds[i]))->GetCurrentMesh();
        if (mesh)
            mesh->Show(CKHIDE);
    }

    // Correct the bbp filter
    for (CKBehavior *rri = (CKBehavior *)m_NeMoContext->GetObjectByNameAndClass("Remove Row If", CKCID_BEHAVIOR);
         rri != NULL;
         rri = (CKBehavior *)m_NeMoContext->GetObjectByNameAndClass("Remove Row If", CKCID_BEHAVIOR, rri))
    {
        if (rri->GetTarget() && strcmp(rri->GetTarget()->GetName(), "ScreenModes") == 0)
        {
            int column = -1;
            rri->GetInputParameterValue(0, &column);
            int *value = (int *)rri->GetInputParameterReadDataPtr(2);

            if (column == 3) // ScreenModes.Bpp
            {
                *value = m_NeMoContext->GetBPP();
                break;
            }
        }
    }

    // Unlock frame rate limitation
    if (config.unlockFramerate)
    {
        for (CKBehavior *timeSettings = (CKBehavior *)m_NeMoContext->GetObjectByNameAndClass("Time Settings", CKCID_BEHAVIOR);
             timeSettings != NULL;
             timeSettings = (CKBehavior *)m_NeMoContext->GetObjectByNameAndClass("Time Settings", CKCID_BEHAVIOR, timeSettings))
        {
            DWORD *frameRate = (DWORD *)timeSettings->GetInputParameterReadDataPtr(0);
            if (*frameRate == 3) // Frame Rate == Limit
            {
                *frameRate = 1; // Frame Rate = Free
                break;
            }
        }
    }

    // Modify game settings according to initialization configurations
    CKDataArray *gameSettings = (CKDataArray *)m_NeMoContext->GetObjectByNameAndClass("GameSettings", CKCID_DATAARRAY);
    if (config.playerActive)
        gameSettings->SetElementStringValue(0, 5, "TRUE");
    if (config.godmode)
        gameSettings->SetElementStringValue(0, 6, "TRUE");
    if (config.debug)
        gameSettings->SetElementStringValue(0, 7, "TRUE");

    // Set the initial conditions for the level
    level->LaunchScene(NULL);

    for (i = 0; i < sceneCount; ++i)
        level->GetScene(i)->SetBackgroundColor(0);

    DeleteCKObjectArray(array);

    return true;
}

void CGame::Play()
{
    m_NeMoContext->Play();
}

CGameInfo *CGame::NewGameInfo()
{
    m_GameInfo = new CGameInfo();
    return m_GameInfo;
}

CGameInfo *CGame::GetGameInfo()
{
    return m_GameInfo;
}

void CGame::SetGameInfo(CGameInfo *gameInfo)
{
    m_GameInfo = gameInfo;
}

CNeMoContext *CGame::GetNeMoContext() const
{
    return m_NeMoContext;
}

void CGame::SetNeMoContext(CNeMoContext *nemoContext)
{
    m_NeMoContext = nemoContext;
}

CGameData::CGameData() : type(0), hkRoot(HKEY_CURRENT_USER)
{
    memset(fileName, 0, sizeof(fileName));
    memset(path, 0, sizeof(path));
    memset(regSubkey, 0, sizeof(regSubkey));
}

CGameData::CGameData(const char *filename, const char *path, const char *subkey, HKEY root) : type(0), hkRoot(root)
{
    strcpy(this->fileName, filename);
    strcpy(this->path, path);
    strcpy(this->regSubkey, subkey);
}

CGameData::CGameData(const CGameData &rhs) : type(0), hkRoot(rhs.hkRoot)
{
    strcpy(fileName, rhs.fileName);
    strcpy(path, rhs.path);
    strcpy(regSubkey, rhs.regSubkey);
}

CGameData &CGameData::operator=(const CGameData &rhs)
{
    strcpy(fileName, rhs.fileName);
    strcpy(path, rhs.path);
    type = rhs.type;
    hkRoot = rhs.hkRoot;
    strcpy(regSubkey, rhs.regSubkey);

    return *this;
}

CGameDataManager::CGameDataManager() : m_Count(14) {}

void CGameDataManager::Save(CGameInfo *gameInfo)
{
    if (m_Count >= 100)
        return;

    strcpy(g_GameData[m_Count].fileName, gameInfo->fileName);
    strcpy(g_GameData[m_Count].path, gameInfo->path);
    g_GameData[m_Count].hkRoot = gameInfo->hkRoot;
    g_GameData[m_Count].type = gameInfo->type;
    strcpy(g_GameData[m_Count].regSubkey, gameInfo->regSubkey);
    ++m_Count;
}

void CGameDataManager::Load(CGameInfo *gameInfo, const char *filename)
{
    if (m_Count <= 0)
        return;

    int i;
    for (i = 0; strcmp(g_GameData[i].fileName, filename) != 0; ++i)
        if (++i >= m_Count)
            return;

    strcpy(gameInfo->fileName, g_GameData[i].fileName);
    strcpy(gameInfo->path, g_GameData[i].path);
    gameInfo->hkRoot = g_GameData[i].hkRoot;
    gameInfo->type = g_GameData[i].type;
    strcpy(gameInfo->regSubkey, g_GameData[i].regSubkey);
}
