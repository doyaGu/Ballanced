//
// WorldEnvironment.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_WorldEnvironment_BehaviorDeclarations
    #define InitInstance					_WorldEnvironment_InitInstance
    #define ExitInstance					_WorldEnvironment_ExitInstance
    #define CKGetPluginInfoCount			CKGet_WorldEnvironment_PluginInfoCount
    #define CKGetPluginInfo					CKGet_WorldEnvironment_PluginInfo
    #define g_PluginInfo					g_WorldEnvironment_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

#define WORLDENVIRONMENTS_BEHAVIOR CKGUID(0x45567f5b, 0x74cd0cc3)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "World Environment building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = NULL;
    g_PluginInfo.m_ExitInstanceFct = NULL;
    g_PluginInfo.m_GUID = WORLDENVIRONMENTS_BEHAVIOR;
    g_PluginInfo.m_Summary = "World Environments";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorSetBackgroundImageDecl);
    RegisterBehavior(reg, FillBehaviorSetBackgroundMaterialDecl);
    RegisterBehavior(reg, FillBehaviorSkyAroundDecl);
    RegisterBehavior(reg, FillBehaviorCloudsAroundDecl);
    RegisterBehavior(reg, FillBehaviorSetFogDecl);
    RegisterBehavior(reg, FillBehaviorSetBackgroundColorDecl);
    RegisterBehavior(reg, FillBehaviorSetGlobalAmbientDecl);
    RegisterBehavior(reg, FillBehaviorSkyAroundCubeMapDecl);
}
