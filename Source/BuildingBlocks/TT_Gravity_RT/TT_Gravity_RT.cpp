#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_TT_Gravity_BehaviorDeclarations
#define InitInstance                    _TT_Gravity_InitInstance
#define ExitInstance                    _TT_Gravity_ExitInstance
#define CKGetPluginInfoCount            CKGet_TT_Gravity_PluginInfoCount
#define CKGetPluginInfo                 CKGet_TT_Gravity_PluginInfo
#define g_PluginInfo                    g_TT_Gravity_PluginInfo
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
    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    return CK_OK;
}

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Terratools";
    g_PluginInfo.m_Description = "Gravity Buildingblocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = NULL;
    g_PluginInfo.m_ExitInstanceFct = NULL;
    g_PluginInfo.m_GUID = CKGUID(0, 0);
    g_PluginInfo.m_Summary = "Gravity Buildingblocks";

    return &g_PluginInfo;
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorExtraDecl);
    RegisterBehavior(reg, FillBehaviorProximityVolumeControlDecl);
    RegisterBehavior(reg, FillBehaviorSpeedOMeterDecl);
    RegisterBehavior(reg, FillBehaviorSimpleShadowDecl);
    RegisterBehavior(reg, FillBehaviorRealShadowMappingDecl);
    RegisterBehavior(reg, FillBehaviorSkyDecl);
    RegisterBehavior(reg, FillBehaviorTextureSineDecl);
    RegisterBehavior(reg, FillBehaviorGetSoundPropertiesDecl);
}