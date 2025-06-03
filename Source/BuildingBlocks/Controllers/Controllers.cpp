//
// Controllers.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Controllers_BehaviorDeclarations
    #define InitInstance					_Controllers_InitInstance
    #define ExitInstance					_Controllers_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Controllers_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Controllers_PluginInfo
    #define g_PluginInfo					g_Controllers_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

#define CONTROLLER_BEHAVIOR CKGUID(0x344366e3, 0x35f23b0a)

CKERROR InitInstance(CKContext *context);
CKERROR ExitInstance(CKContext *context);
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Controller Building Blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = CONTROLLER_BEHAVIOR;
    g_PluginInfo.m_Summary = "Controller Building Blocks";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Controllers/Joystick
    RegisterBehavior(reg, FillBehaviorJoystickControllerDecl);
    RegisterBehavior(reg, FillBehaviorDigitalJoystickMapperDecl);
    RegisterBehavior(reg, FillBehaviorJoystickWaiterDecl);

    // Controllers/Keyboard
    RegisterBehavior(reg, FillBehaviorKeyEventDecl);
    RegisterBehavior(reg, FillBehaviorKeyWaiterDecl);
    RegisterBehavior(reg, FillBehaviorKeyboardControllerDecl);
    RegisterBehavior(reg, FillBehaviorKeyboardMapperDecl);
    RegisterBehavior(reg, FillBehaviorSwitchOnKeyDecl);
    RegisterBehavior(reg, FillBehaviorEditStringDecl);

    // Controllers/Mouse
    RegisterBehavior(reg, FillBehaviorGetMousePositionDecl);
    RegisterBehavior(reg, FillBehaviorGetMouseRelativePositionDecl);
    RegisterBehavior(reg, FillBehaviorMouseWaiterDecl);
}

#define CKPGUID_JOYAXIS CKGUID(0x1ed64a6a, 0x5a24059a)
#define CKPGUID_MOUSEEVENT CKGUID(0x7abe0373, 0x1c366a1b)
#define CKPGUID_KEYBOARDPART CKGUID(0x7a4347dd, 0x59a82cce)

CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    // Evolution Flags
    pm->RegisterNewFlags(CKPGUID_MOUSEEVENT, "Mouse Event", "Move=1,Left Button Down=2,Left Button Up=4,Right Button Down=8,Right Button Up=16,Middle Button Down=32,Middle Button Up=64,Roll Received=128");
    pm->RegisterNewFlags(CKPGUID_JOYAXIS, "Joy Axis", "Axis X=1,Axis Y=2,Axis Z=4,Rotation X=8,Rotation Y=16,Rotation Z=32,Slider 1=64,Slider 2=128,Pov=256");
    pm->RegisterNewFlags(CKPGUID_KEYBOARDPART, "Keyboard Section", "NumPad=1,Arrows=2");

    // We hide the parameters (structures and flags)
    CKParameterTypeDesc *param_type;
    param_type = pm->GetParameterTypeDescription(CKPGUID_MOUSEEVENT);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_JOYAXIS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(CKPGUID_MOUSEEVENT);
    pm->UnRegisterParameterType(CKPGUID_JOYAXIS);

    return CK_OK;
}
