//
// Cameras.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Cameras_BehaviorDeclarations
    #define InitInstance					_Cameras_InitInstance
    #define ExitInstance					_Cameras_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Cameras_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Cameras_PluginInfo
    #define g_PluginInfo					g_Cameras_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

CKERROR InitInstance(CKContext *context);
CKERROR ExitInstance(CKContext *context);
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

#define CAMERA_BEHAVIOR CKGUID(0x12d94eba, 0x47057415)
#define CKPGUID_PROJECTIONTYPE CKDEFINEGUID(0x1ee22148, 0x602c1ca1)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Camera building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = CAMERA_BEHAVIOR;
    g_PluginInfo.m_Summary = "Cameras";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/

CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->RegisterNewEnum(CKPGUID_PROJECTIONTYPE, "Projection Mode", "Perspective=1,Orthographic=2");

// Mouse Camera Orbit
#define CKPGUID_MOUSEBUTTON CKGUID(0x1ff24d5a, 0x122f2c1f)
    pm->RegisterNewEnum(CKPGUID_MOUSEBUTTON, "Mouse Button", "Left=0,Middle=2,Right=1");

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->UnRegisterParameterType(CKPGUID_PROJECTIONTYPE);
    pm->UnRegisterParameterType(CKPGUID_MOUSEBUTTON);
    return CK_OK;
}

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{

    // Cameras/Basic
    RegisterBehavior(reg, FillBehaviorDollyDecl);
    RegisterBehavior(reg, FillBehaviorSetOrthographicZoomDecl);
    RegisterBehavior(reg, FillBehaviorSetCameraTargetDecl);
    RegisterBehavior(reg, FillBehaviorSetClippingDecl);
    RegisterBehavior(reg, FillBehaviorSetFOVDecl);
    RegisterBehavior(reg, FillBehaviorSetProjectionDecl);
    RegisterBehavior(reg, FillBehaviorSetZoomDecl);

    // Cameras/FX
    RegisterBehavior(reg, FillBehaviorCameraColorFilterDecl);
    RegisterBehavior(reg, FillBehaviorVertigoDecl);

    // Cameras/Montage
    RegisterBehavior(reg, FillBehaviorGetCurrentCameraDecl);
    RegisterBehavior(reg, FillBehaviorSetAsActiveCameraDecl);

    // Cameras/Movement
    RegisterBehavior(reg, FillBehaviorOrbitDecl);
    RegisterBehavior(reg, FillBehaviorKeyboardCameraOrbitDecl);
    RegisterBehavior(reg, FillBehaviorMouseCameraOrbitDecl);
    RegisterBehavior(reg, FillBehaviorJoystickCameraOrbitDecl);
    RegisterBehavior(reg, FillBehaviorGenericCameraOrbitDecl);
}
