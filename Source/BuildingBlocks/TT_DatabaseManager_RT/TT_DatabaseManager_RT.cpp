#include "DatabaseManager.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_TT_Database_Manager_BehaviorDeclarations
#define InitInstance                    _TT_Database_Manager_InitInstance
#define ExitInstance                    _TT_Database_Manager_ExitInstance
#define CKGetPluginInfoCount            CKGet_TT_Database_Manager_PluginInfoCount
#define CKGetPluginInfo                 CKGet_TT_Database_Manager_PluginInfo
#define g_PluginInfo                    g_TT_Database_Manager_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

CKERROR InitInstance(CKContext *context)
{
    new DatabaseManager(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    DatabaseManager *man = DatabaseManager::GetManager(context);
    if (man) delete man;

    return CK_OK;
}

CKPluginInfo g_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo[0].m_Author = "TERRATOOLS";
    g_PluginInfo[0].m_Description = "Manages databases";
    g_PluginInfo[0].m_Extension = "";
    g_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo[0].m_Version = 0x000001;
    g_PluginInfo[0].m_InitInstanceFct = NULL;
    g_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_PluginInfo[0].m_GUID = CKGUID(0, 0);
    g_PluginInfo[0].m_Summary = "TT Database Building Blocks";

    g_PluginInfo[1].m_Author = "TERRATOOLS";
    g_PluginInfo[1].m_Description = "Manages databases";
    g_PluginInfo[1].m_Extension = "";
    g_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_PluginInfo[1].m_Version = 0x000001;
    g_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_PluginInfo[1].m_GUID = TT_DATABASE_MANAGER_GUID;
    g_PluginInfo[1].m_Summary = "TT Database-Manager";

    return &g_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorRegisterArrayDecl);
    RegisterBehavior(reg, FillBehaviorLoadDatabaseDecl);
    RegisterBehavior(reg, FillBehaviorSaveDatabaseDecl);
    RegisterBehavior(reg, FillBehaviorSetDatabasePropertiesDecl);
}