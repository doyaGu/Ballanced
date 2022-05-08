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

bool CGame::Load()
{
    char cmoPath[MAX_PATH];
    char dir[MAX_PATH];

    if (!m_GameInfo)
    {
        TT_ERROR("Game.cpp", "Load", "gameInfo is NULL: CMO is not loaded");
        return false;
    }

    if (m_NeMoContext->GetMadeWithSprite())
    {
        m_NeMoContext->DestroyMadeWithSprite();
    }
    if (m_NeMoContext->GetFrameRateSprite())
    {
        m_NeMoContext->DestroyFrameRateSprite();
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

    m_NeMoContext->CreateInterfaceSprite();
    m_NeMoContext->SetStartTime(::GetTickCount() + 3000);

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
            m_NeMoContext->SetStartTime(0);
            break;
        }

        if (m_NeMoContext->GetStartTime() > 0)
        {
            m_NeMoContext->SetMadeWithSpriteText(text);
            m_NeMoContext->ShowMadeWithSprite();
        }
    }

    m_NeMoContext->AdjustFrameRateSpritePosition();
    m_NeMoContext->ShowFrameRateSprite();

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
    {
        level->GetScene(i)->SetBackgroundColor(0);
    }

    CKRenderContext* renderContext = m_NeMoContext->GetRenderContext();
    level->AddRenderContext(renderContext, TRUE);
    array->Clear();

    CK_ID *cameras = m_NeMoContext->GetObjectsListByClassID(CKCID_CAMERA);
    if (cameras || (cameras = m_NeMoContext->GetObjectsListByClassID(CKCID_TARGETCAMERA)))
    {
        CKCamera *camera = (CKCamera *)m_NeMoContext->GetObject(cameras[0]);
        if (camera)
        {
            renderContext->AttachViewpointToCamera(camera);
        }
    }

    renderContext->Clear();
    renderContext->SetClearBackground();
    renderContext->BackToFront();
    renderContext->SetClearBackground();
    renderContext->Clear();

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

    // launch the default scene
    level->LaunchScene(NULL);

    for (i = 0; i < sceneCount; ++i)
    {
        level->GetScene(i)->SetBackgroundColor(0);
    }
  
    DeleteCKObjectArray(array);

    // ReRegister OnClick Message in case it changed
    m_NeMoContext->AddMessageType("OnClick");
    m_NeMoContext->AddMessageType("OnDblClick");

    m_NeMoContext->HideMadeWithSprite();
    m_NeMoContext->HideFrameRateSprite();

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
