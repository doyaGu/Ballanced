#include "StdAfx.h"

#include "Game.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorProtocol.h"
#include "NeMoContext.h"

namespace
{
    const char *virtoolsVersionStrings[] =
        {
            "Dev \n (Evaluation version)",
            "Creation \n (Evaluation version)",
            "Dev \n (Not For Resale)",
            "Creation \n (Not For Resale)",
            "Dev \n (Education version)",
            "Creation \n (Education version)",
    };

    class CGameDataArray
    {
    public:
        static CGameDataArray &GetInstance();

        CGameData &operator[](size_t n)
        {
            return m_Data[n];
        }

        const CGameData &operator[](size_t n) const
        {
            return m_Data[n];
        }

    private:
        CGameDataArray();
        CGameDataArray(const CGameDataArray &);
        CGameDataArray &operator=(const CGameDataArray &);

        CGameData m_Data[100];
    };

    CGameDataArray &CGameDataArray::GetInstance()
    {
        static CGameDataArray array;
        return array;
    }

    CGameDataArray::CGameDataArray()
    {
        m_Data[0] = CGameData("Clock.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[1] = CGameData("WM.cmo", "WM", "Software\\Terratools\\WorldMap");
        m_Data[2] = CGameData("PBC.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[3] = CGameData("PB_L0.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[4] = CGameData("PB_L1.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[5] = CGameData("PB_L2.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[6] = CGameData("PB_L3.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[7] = CGameData("PB_L4.cmo", "Powerball", "Software\\Terratools\\Powerball\\Data");
        m_Data[8] = CGameData("SP_INTRO.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
        m_Data[9] = CGameData("SPC.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
        m_Data[10] = CGameData("SP_L1.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
        m_Data[11] = CGameData("SP_L2.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
        m_Data[12] = CGameData("SP_L3.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
        m_Data[13] = CGameData("SP_L4.cmo", "Superpuck", "Software\\Terratools\\Superpuck\\Data");
    }

    CGameDataArray &gameData = CGameDataArray::GetInstance();
}

void CGameException::ShowMessageBox() const
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
    if (GetGameInfo() != NULL)
    {
        delete m_GameInfo;
    }
    m_GameInfo = NULL;
    SetNeMoContext(NULL);
}

void CGame::Load()
{
    char cmoPath[256];
    char dir[260];

    if (!m_GameInfo)
    {
        TT_ERROR("Game.cpp", "Load", "gameInfo is NULL: CMO is not loaded");
        throw CGameException(3);
    }

    if (m_NeMoContext->GetMadeWithSprite())
    {
        m_NeMoContext->DestroyMadeWithSprite();
    }
    if (m_NeMoContext->GetFrameRateSprite())
    {
        m_NeMoContext->DestroyFrameRateSprite();
    }

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
        m_NeMoContext->SetStartTime(::GetTickCount() + 3000);
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
    if (!m_NeMoContext->GetFileInfo(cmoPath, &m_CKFileInfo))
    {
        char buffer[128];
        strcpy(buffer, "Made with Virtools ");
        memset(&buffer[20], 0, 108);
        switch (m_CKFileInfo.ProductVersion)
        {
        case 2:
        case 8:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[0], strlen(virtoolsVersionStrings[0]));
            break;
        case 3:
        case 9:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[1], strlen(virtoolsVersionStrings[1]));
            break;
        case 4:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[2], strlen(virtoolsVersionStrings[2]));
            break;
        case 5:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[3], strlen(virtoolsVersionStrings[3]));
            break;
        case 6:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[4], strlen(virtoolsVersionStrings[4]));
            break;
        case 7:
            memcpy(&buffer[strlen(buffer)], virtoolsVersionStrings[5], strlen(virtoolsVersionStrings[5]));
            break;
        }

        m_NeMoContext->SetStartTime(0);
        if (m_NeMoContext->GetStartTime() > 0)
        {
            try
            {
                m_NeMoContext->SetMadeWithSpriteText(buffer);
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
        m_NeMoContext->MoveFrameRateSpriteToLeftTop();
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

    try
    {
        CKLevel *level = m_NeMoContext->GetCurrentLevel();
        if (!level)
        {
            throw CGameException();
        }

        int i;
        int sceneCount = level->GetSceneCount();
        for (i = 0; i < sceneCount; ++i)
        {
            level->GetScene(i)->SetBackgroundColor(0);
        }

        level->AddRenderContext(m_NeMoContext->GetRenderContext(), TRUE);
        array->Clear();

        CK_ID *cameras = m_NeMoContext->GetObjectsListByClassID(CKCID_CAMERA);
        if (cameras || (cameras = m_NeMoContext->GetObjectsListByClassID(CKCID_TARGETCAMERA)))
        {
            CKCamera *camera = (CKCamera *)m_NeMoContext->GetObject(cameras[0]);
            if (camera)
            {
                m_NeMoContext->GetRenderContext()->AttachViewpointToCamera(camera);
            }
        }

        m_NeMoContext->GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);
        m_NeMoContext->GetRenderContext()->SetClearBackground(TRUE);
        m_NeMoContext->GetRenderContext()->BackToFront(CK_RENDER_USECURRENTSETTINGS);
        m_NeMoContext->GetRenderContext()->SetClearBackground(TRUE);
        m_NeMoContext->GetRenderContext()->Clear(CK_RENDER_USECURRENTSETTINGS, 0);

        int curveCount = m_NeMoContext->GetObjectsCountByClassID(CKCID_CURVE);
        CK_ID *curve_ids = m_NeMoContext->GetObjectsListByClassID(CKCID_CURVE);
        for (i = 0; i < curveCount; ++i)
        {
            CKMesh *mesh = ((CKCurve *)m_NeMoContext->GetObject(curve_ids[i]))->GetCurrentMesh();
            if (mesh)
            {
                mesh->Show(CKHIDE);
            }
        }

        // we launch the default scene
        level->LaunchScene(NULL);

        for (i = 0; i < sceneCount; ++i)
        {
            level->GetScene(i)->SetBackgroundColor(0);
        }
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
        strcpy(gameData[m_Count].fileName, gameInfo->fileName);
        strcpy(gameData[m_Count].path, gameInfo->path);
        gameData[m_Count].hkRoot = gameInfo->hkRoot;
        gameData[m_Count].type = gameInfo->type;
        strcpy(gameData[m_Count].regSubkey, gameInfo->regSubkey);
    }
    ++m_Count;
}

void CGameDataManager::Load(CGameInfo *gameInfo, const char *filename)
{
    int i;

    if (m_Count > 0)
    {
        for (i = 0; strcmp(gameData[i].fileName, filename); ++i)
        {
            if (++i >= m_Count)
            {
                return;
            }
        }

        strcpy(gameInfo->fileName, gameData[i].fileName);
        strcpy(gameInfo->path, gameData[i].path);
        gameInfo->hkRoot = gameData[i].hkRoot;
        gameInfo->type = gameData[i].type;
        strcpy(gameInfo->regSubkey, gameData[i].regSubkey);
    }
}
