//
// SoundBehavior.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Sounds_BehaviorDeclarations
    #define InitInstance					_Sounds_InitInstance
    #define ExitInstance					_Sounds_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Sounds_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Sounds_PluginInfo
    #define g_PluginInfo					g_Sounds_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

/**********************************************************************************/
/**********************************************************************************/

CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->RegisterNewEnum(CKGUID(0x413648dd, 0x47515700), "Sound Type", "Background=1,Point=2");

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->UnRegisterParameterType(CKGUID(0x413648dd, 0x47515700));

    return CK_OK;
}

/**********************************************************************************/
/**********************************************************************************/

#define SOUNDS_BEHAVIOR CKGUID(0x63e37796, 0x586f0b92)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }
PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Sound building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = SOUNDS_BEHAVIOR;
    g_PluginInfo.m_Summary = "Sounds";
    return &g_PluginInfo;
}

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
/**********************************************************************************/
/**********************************************************************************/
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
#ifdef WIN32
    RegisterBehavior(reg, FillBehaviorCDPlayer2Decl);
#endif

    RegisterBehavior(reg, FillBehaviorWavePlayDecl);
    RegisterBehavior(reg, FillBehaviorPlaySoundInstanceDecl);
    // Control

    // 3D
    RegisterBehavior(reg, FillBehaviorSoundManagerSetupDecl);

    RegisterBehavior(reg, FillBehaviorPositionSoundDecl);

    RegisterBehavior(reg, FillBehaviorSetPriorityDecl);
    RegisterBehavior(reg, FillBehaviorSetRangeDecl);
    RegisterBehavior(reg, FillBehaviorSetConeDecl);
    RegisterBehavior(reg, FillBehaviorSetRenderModeDecl);

    RegisterBehavior(reg, FillBehaviorPanningControlDecl);
    RegisterBehavior(reg, FillBehaviorVolumeControlDecl);
    RegisterBehavior(reg, FillBehaviorFrequencyControlDecl);

    RegisterBehavior(reg, FillBehaviorFadeInDecl);
    RegisterBehavior(reg, FillBehaviorFadeOutDecl);
    RegisterBehavior(reg, FillBehaviorSetListenerDecl);
}
