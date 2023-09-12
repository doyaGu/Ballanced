#include "Game.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "NeMoContext.h"

#include "GameInfo.h"

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

CGameException::~CGameException()
{
    switch (error)
    {
    case 1:
        ::MessageBoxA(NULL, "Error", "Error occurred in the engine (ClearAll). Abort!", MB_OK);
        break;
    case 5:
        ::MessageBoxA(NULL, "Error", "Error occurred in the engine (Render). Abort!", MB_OK);
        break;
    case 2:
        ::MessageBoxA(NULL, "Error", "Textsprites could not be created", MB_OK);
        break;
    case -1:
        ::MessageBoxA(NULL, "Error", "Error occurred in the engine (Memory out). Abort!", MB_OK);
        break;
    case 4:
        ::MessageBoxA(NULL, "Error", "Level (or game) could not be loaded (No open file) => Possibly not available. Start again!", MB_OK);
        break;
    case 3:
        ::MessageBoxA(NULL, "Error", "Level (or game) could not be loaded (No gameInfo) => Possibly not available. Start again!", MB_OK);
        break;
    case 6:
        ::MessageBoxA(NULL, "Error", "Sounds directory does not exist.", MB_OK);
        break;
    case 7:
        ::MessageBoxA(NULL, "Error", "Texture directory does not exist.", MB_OK);
        break;
    default:
        ::MessageBoxA(NULL, "Error", "Unhandled exception occurred during loading process. Abort!", MB_OK);
        break;
    }
}

CGame::CGame() : m_NeMoContext(NULL), m_GameInfo(NULL) {}

CGame::~CGame()
{
    if (!GetGameInfo())
        delete m_GameInfo;
    m_GameInfo = NULL;
    SetNeMoContext(NULL);
}

void CGame::Load()
{
    char cmoPath[MAX_PATH];
    char dir[MAX_PATH];

    if (!m_GameInfo)
    {
        TT_ERROR("Game.cpp", "Load", "gameInfo is NULL: CMO is not loaded");
        throw CGameException(3);
    }

    if (m_NeMoContext->GetMadeWithSprite())
        m_NeMoContext->DestroyMadeWithSprite();

    if (m_NeMoContext->GetFrameRateSprite())
        m_NeMoContext->DestroyFrameRateSprite();

    try
    {
        strcpy(m_FileName, m_GameInfo->fileName);
        sprintf(m_ProgPath, "%s%s\\", m_NeMoContext->GetProgPath(), m_GameInfo->path);
        sprintf(cmoPath, "%s%s", m_ProgPath, m_GameInfo->fileName);
        FILE *fcmo = fopen(cmoPath, "r");
        if (!fcmo)
        {
            TT_ERROR("Game.cpp", "Load", "fopen/fclose threw exception");
            throw CGameException(4);
        }
        fclose(fcmo);
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "fopen/fclose threw exception");
        throw CGameException(4);
    }

    try
    {
        m_NeMoContext->CreateInterfaceSprite();
        m_NeMoContext->SetTimeToHideSprite(::GetTickCount() + 3000);
    }
    catch (const CNeMoContextException &nmce)
    {
        if (nmce.error == 2)
        {
            TT_ERROR("Game.cpp", "Load", "Exception at sprite display due to missing RenderContext");
            throw CGameException(5);
        }
        else
        {
            TT_ERROR("Game.cpp", "Load", "Exception on sprite display / No abort");
            return;
        }
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Exception at sprite display due to missing RenderContext");
        return;
    }

    memset(&m_CKFileInfo, 0, sizeof(CKFileInfo));
    if (m_NeMoContext->GetFileInfo(cmoPath, &m_CKFileInfo) == CK_OK)
    {
        char text[128] = "Made with Virtools ";
        switch (m_CKFileInfo.ProductVersion)
        {
        case CK_VIRTOOLS_DEV:
        case CK_VIRTOOLS_CREATION:
            break;
        case CK_VIRTOOLS_DEV_NFR:
            strcat(text, "Dev \n (Not For Resale)");
            break;
        case CK_VIRTOOLS_CREA_NFR:
            strcat(text, "Creation \n (Not For Resale)");
            break;
        case CK_VIRTOOLS_DEV_EDU:
            strcat(text, "Dev \n (Education version)");
            break;
        case CK_VIRTOOLS_CREA_EDU:
            strcat(text, "Creation \n (Education version)");
            break;
        case CK_VIRTOOLS_DEV_TB:
        case CK_VIRTOOLS_DEV_EVAL:
            strcat(text, "Dev \n (Evaluation version)");
            break;
        case CK_VIRTOOLS_CREA_TB:
        case CK_VIRTOOLS_CREA_EVAL:
            strcat(text, "Creation \n (Evaluation version)");
            break;
        default:
            m_NeMoContext->SetTimeToHideSprite(0);
            break;
        }

        if (m_NeMoContext->GetTimeToHideSprite() > 0)
        {
            try
            {
                m_NeMoContext->SetMadeWithSpriteText(text);
                m_NeMoContext->ShowMadeWithSprite();
            }
            catch (...)
            {
                TT_ERROR("Game.cpp", "Load", "Exception with TextSprites / no abort");
                return;
            }
        }
    }

    try
    {
        m_NeMoContext->AdjustFrameRateSpritePosition();
        m_NeMoContext->ShowFrameRateSprite();
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Exception with TextSprites / no abort");
        return;
    }

    try
    {
        m_NeMoContext->Render(CK_RENDER_BACKGROUNDSPRITES);
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Exception on Render() before CKLoad()");
        throw CGameException(5);
    }

    try
    {
        sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), "Sounds");
        if (_access(dir, 0) == -1)
        {
            TT_ERROR("Game.cpp", "Load", "No Sounds directory");
            throw CGameException(6);
        }
        m_NeMoContext->AddSoundPath(dir);

        sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), "Textures");
        if (_access(dir, 0) == -1)
        {
            TT_ERROR("Game.cpp", "Load", "No Textures directory");
            throw CGameException(7);
        }
        m_NeMoContext->AddBitmapPath(dir);

        sprintf(dir, "%s%s\\", m_NeMoContext->GetProgPath(), m_GameInfo->path);
        m_NeMoContext->AddDataPath(dir);
    }
    catch (...)
    {
        throw CGameException();
    }

    CKObjectArray *array = NULL;
    try
    {
        array = CreateCKObjectArray();
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Exception on CreateCKObjectArray()");
        throw CGameException(-1);
    }

    if (m_NeMoContext->LoadFile(cmoPath, array, CK_LOAD_DEFAULT, NULL) != CK_OK)
    {
        TT_ERROR("Game.cpp", "Load", "CKLoad");
        throw CGameException(4);
    }

    // Loads the file and fills the array with loaded objects
    if (m_NeMoContext->LoadFile(cmoPath, array, CK_LOAD_DEFAULT, NULL) != CK_OK)
    {
        TT_ERROR("Game.cpp", "Load", "CKLoad");
        throw CGameException(4);
    }

    try
    {
        CKLevel *level = m_NeMoContext->GetCurrentLevel();
        if (!level)
            throw CGameException();

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

        m_NeMoContext->GetRenderContext()->Clear();
        m_NeMoContext->GetRenderContext()->SetClearBackground();
        m_NeMoContext->GetRenderContext()->BackToFront();
        m_NeMoContext->GetRenderContext()->SetClearBackground();
        m_NeMoContext->GetRenderContext()->Clear();

        // Hide curves ?
        int curveCount = m_NeMoContext->GetObjectsCountByClassID(CKCID_CURVE);
        CK_ID *curveIds = m_NeMoContext->GetObjectsListByClassID(CKCID_CURVE);
        for (i = 0; i < curveCount; ++i)
        {
            CKMesh *mesh = ((CKCurve *)m_NeMoContext->GetObject(curveIds[i]))->GetCurrentMesh();
            if (mesh)
                mesh->Show(CKHIDE);
        }

        // Set the initial conditions for the level
        level->LaunchScene(NULL);

        for (i = 0; i < sceneCount; ++i)
            level->GetScene(i)->SetBackgroundColor(0);
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Embed charged CMO");
        throw CGameException(4);
    }

    try
    {
        DeleteCKObjectArray(array);
    }
    catch (...)
    {
        TT_ERROR("Game.cpp", "Load", "Delete, Render");
        throw CGameException();
    }

    // ReRegister OnClick Message in case it changed
    m_NeMoContext->AddMessageType("OnClick");
    m_NeMoContext->AddMessageType("OnDblClick");

    m_NeMoContext->HideMadeWithSprite();
    m_NeMoContext->HideFrameRateSprite();
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
    if (m_Count < 100)
    {
        strcpy(g_GameData[m_Count].fileName, gameInfo->fileName);
        strcpy(g_GameData[m_Count].path, gameInfo->path);
        g_GameData[m_Count].hkRoot = gameInfo->hkRoot;
        g_GameData[m_Count].type = gameInfo->type;
        strcpy(g_GameData[m_Count].regSubkey, gameInfo->regSubkey);
    }
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
