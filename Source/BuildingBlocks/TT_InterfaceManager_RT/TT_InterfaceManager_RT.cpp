#include "StdAfx.h"

#include <stdio.h>

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

#define TT_INTERFACE_MANAGER_BEHAVIOR CKGUID(0x1B5171D8, 0x41B60E5F)
#define INTERFACE_MANAGER_BEHAVIOR CKGUID(0x30833801, 0x6DEE620D)

#ifdef CK_LIB
#define RegisterBehaviorDeclarations Register_TT_Interface_Manager_BehaviorDeclarations
#define InitInstance _TT_Interface_Manager_InitInstance
#define ExitInstance _TT_Interface_Manager_ExitInstance
#define CKGetPluginInfoCount CKGet_TT_Interface_Manager_PluginInfoCount
#define CKGetPluginInfo CKGet_TT_Interface_Manager_PluginInfo
#define g_PluginInfo g_TT_Interface_Manager_PluginInfo
#else
#define RegisterBehaviorDeclarations RegisterBehaviorDeclarations
#define InitInstance InitInstance
#define ExitInstance ExitInstance
#define CKGetPluginInfoCount CKGetPluginInfoCount
#define CKGetPluginInfo CKGetPluginInfo
#define g_PluginInfo g_PluginInfo
#endif

CKERROR InitInstance(CKContext *context)
{
    new CTTInterfaceManager(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CTTInterfaceManager *man = (CTTInterfaceManager *)context->GetManagerByGuid(TT_INTERFACE_MANAGER_GUID);
    if (man)
        delete man;

    return CK_OK;
}

CKPluginInfo g_Interface_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_Interface_PluginInfo[0].m_Author = "Terratools - Andre Menzel";
    g_Interface_PluginInfo[0].m_Description = "Routine for Player - Behavior - Interface";
    g_Interface_PluginInfo[0].m_Extension = "";
    g_Interface_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_Interface_PluginInfo[0].m_Version = 0x000001;
    g_Interface_PluginInfo[0].m_InitInstanceFct = NULL;
    g_Interface_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_Interface_PluginInfo[0].m_GUID = TT_INTERFACE_MANAGER_BEHAVIOR;
    g_Interface_PluginInfo[0].m_Summary = "Behavior for Interface Manager";

    g_Interface_PluginInfo[1].m_Author = "Terratools - Andre Menzel";
    g_Interface_PluginInfo[1].m_Description = "Terratools - Interface Manager";
    g_Interface_PluginInfo[1].m_Extension = "";
    g_Interface_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_Interface_PluginInfo[1].m_Version = 0x000001;
    g_Interface_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_Interface_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_Interface_PluginInfo[1].m_GUID = INTERFACE_MANAGER_BEHAVIOR;
    g_Interface_PluginInfo[1].m_Summary = "Interface Manager";

    return &g_Interface_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    char fullpath[260];
    char drive[4];
    char dir[260];
    char filename[256];
    char ext[256];
    char buffer[260];

    ::GetModuleFileNameA(NULL, fullpath, 260);
    _splitpath(fullpath, drive, dir, filename, ext);
    sprintf(buffer, "%s%s", drive, dir);
    TT_ERROR_OPEN("TT_Player_CMO_Interface", buffer, true);

    // Message
    RegisterBehavior(reg, FillBehaviorSendCMORestartDecl);
    RegisterBehavior(reg, FillBehaviorSendCMOLoadDecl);
    RegisterBehavior(reg, FillBehaviorSendExitToSystemDecl);

    // DataArray
    RegisterBehavior(reg, FillBehaviorInstallArrayDecl);
    RegisterBehavior(reg, FillBehaviorWriteArrayDecl);
    RegisterBehavior(reg, FillBehaviorReadArrayDecl);
    RegisterBehavior(reg, FillBehaviorUninstallArrayDecl);

    // GameInfo
    RegisterBehavior(reg, FillBehaviorCreateGameInfoDecl);
    RegisterBehavior(reg, FillBehaviorExistsGameInfoDecl);
    RegisterBehavior(reg, FillBehaviorDeleteGameInfoDecl);

    // Display
    RegisterBehavior(reg, FillBehaviorLimitFramerateDecl);
    RegisterBehavior(reg, FillBehaviorChangeScreenModeDecl);
    RegisterBehavior(reg, FillBehaviorListScreenModesDecl);
    RegisterBehavior(reg, FillBehaviorListDriversDecl);
    RegisterBehavior(reg, FillBehaviorWindowActivatedDecl);

    // General
    RegisterBehavior(reg, FillBehaviorPlayerActiveDecl);
    RegisterBehavior(reg, FillBehaviorSetTaskSwitchHandlerDecl);

    // GameInfo
    RegisterBehavior(reg, FillBehaviorSetGameDataDecl);
    RegisterBehavior(reg, FillBehaviorSetGameScoreDecl);
    RegisterBehavior(reg, FillBehaviorSetGameBonusDecl);
    RegisterBehavior(reg, FillBehaviorSetGameNameDecl);
    RegisterBehavior(reg, FillBehaviorSetGameIDDecl);

    RegisterBehavior(reg, FillBehaviorGetGameDataDecl);
    RegisterBehavior(reg, FillBehaviorGetGameScoreDecl);
    RegisterBehavior(reg, FillBehaviorGetGameBonusDecl);
    RegisterBehavior(reg, FillBehaviorGetGameNameDecl);
    RegisterBehavior(reg, FillBehaviorGetGameIDDecl);

    // LevelInfo
    RegisterBehavior(reg, FillBehaviorSetLevelDataDecl);
    RegisterBehavior(reg, FillBehaviorSetLevelScoreDecl);
    RegisterBehavior(reg, FillBehaviorSetLevelReachedDecl);
    RegisterBehavior(reg, FillBehaviorSetLevelBonusDecl);
    RegisterBehavior(reg, FillBehaviorSetLevelNameDecl);
    RegisterBehavior(reg, FillBehaviorSetLevelIDDecl);

    RegisterBehavior(reg, FillBehaviorGetLevelDataDecl);
    RegisterBehavior(reg, FillBehaviorGetLevelScoreDecl);
    RegisterBehavior(reg, FillBehaviorGetLevelReachedDecl);
    RegisterBehavior(reg, FillBehaviorGetLevelBonusDecl);
    RegisterBehavior(reg, FillBehaviorGetLevelNameDecl);
    RegisterBehavior(reg, FillBehaviorGetLevelIDDecl);

    // Registry
    RegisterBehavior(reg, FillBehaviorSetStringValueToRegistryDecl);
    RegisterBehavior(reg, FillBehaviorSetIntegerValueToRegistryDecl);
    RegisterBehavior(reg, FillBehaviorSetFloatValueToRegistryDecl);
    RegisterBehavior(reg, FillBehaviorSetBooleanValueToRegistryDecl);
    
    RegisterBehavior(reg, FillBehaviorGetStringValueFromRegistryDecl);
    RegisterBehavior(reg, FillBehaviorGetIntegerValueFromRegistryDecl);
    RegisterBehavior(reg, FillBehaviorGetFloatValueFromRegistryDecl);
    RegisterBehavior(reg, FillBehaviorGetBooleanValueFromRegistryDecl);
    
    RegisterBehavior(reg, FillBehaviorWriteRegistryDecl);
    RegisterBehavior(reg, FillBehaviorReadRegistryDecl);

    // Rookie
    RegisterBehavior(reg, FillBehaviorSetRookieDecl);
    RegisterBehavior(reg, FillBehaviorIsRookieDecl);
}