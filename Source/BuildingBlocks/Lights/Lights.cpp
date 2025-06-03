//
// Lights.cpp : Defines the initialization routines for the DLL.
//

#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Lights_BehaviorDeclarations
    #define InitInstance					_Lights_InitInstance
    #define ExitInstance					_Lights_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Lights_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Lights_PluginInfo
    #define g_PluginInfo					g_Lights_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

#define LIGHTS_BEHAVIOR CKGUID(0x3cec69ec, 0x194e1b23)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Light building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = NULL;
    g_PluginInfo.m_ExitInstanceFct = NULL;
    g_PluginInfo.m_GUID = LIGHTS_BEHAVIOR;
    g_PluginInfo.m_Summary = "Lights";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{

    // Lights/Basic
    RegisterBehavior(reg, FillBehaviorSetConstantAttenuationDecl);
    RegisterBehavior(reg, FillBehaviorSetFalloffDecl);
    RegisterBehavior(reg, FillBehaviorSetColorLightDecl);
    RegisterBehavior(reg, FillBehaviorSetRangeLightDecl);
    RegisterBehavior(reg, FillBehaviorSetLightTargetDecl);
    RegisterBehavior(reg, FillBehaviorSetTypeLightDecl);
    RegisterBehavior(reg, FillBehaviorSetLinearAttenuationDecl);
    RegisterBehavior(reg, FillBehaviorSetQuadraticAttenuationDecl);
    RegisterBehavior(reg, FillBehaviorSetSpecularFlagDecl);

    // Lights/FX
    RegisterBehavior(reg, FillBehaviorDisplayOmniLightDecl);
    RegisterBehavior(reg, FillBehaviorFlashColorDecl);
}
