#include "CKAll.h"

#include "CKIpionManager.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations Register_TT_Physics_BehaviorDeclarations
#define InitInstance _TT_Physics_InitInstance
#define ExitInstance _TT_Physics_ExitInstance
#define CKGetPluginInfoCount CKGet_TT_Physics_PluginInfoCount
#define CKGetPluginInfo CKGet_TT_Physics_PluginInfo
#define g_PluginInfo g_TT_Physics_PluginInfo
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
    new CKIpionManager(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (man)
        delete man;

    return CK_OK;
}

CKPluginInfo g_Physics_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_Physics_PluginInfo[0].m_Author = "TERRATOOLS";
    g_Physics_PluginInfo[0].m_Description = "Physics Building Blocks";
    g_Physics_PluginInfo[0].m_Extension = "";
    g_Physics_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_Physics_PluginInfo[0].m_Version = 0x000001;
    g_Physics_PluginInfo[0].m_InitInstanceFct = NULL;
    g_Physics_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_Physics_PluginInfo[0].m_GUID = CKGUID(0x2FA687D, 0x22814521);
    g_Physics_PluginInfo[0].m_Summary = "Physics Building Blocks";

    g_Physics_PluginInfo[1].m_Author = "TERRATOOLS";
    g_Physics_PluginInfo[1].m_Description = "Physics Manager";
    g_Physics_PluginInfo[1].m_Extension = "";
    g_Physics_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_Physics_PluginInfo[1].m_Version = 0x000001;
    g_Physics_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_Physics_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_Physics_PluginInfo[1].m_GUID = TT_PHYSICS_MANAGER_GUID;
    g_Physics_PluginInfo[1].m_Summary = "Physics Manager";

    return &g_Physics_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorPhysicalizeDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsImpulseDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsForceDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsWakeUpDecl);
    RegisterBehavior(reg, FillBehaviorSetPhysicsGlobalsDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsHingeDecl);
    RegisterBehavior(reg, FillBehaviorSetPhysicsSpringDecl);
    RegisterBehavior(reg, FillBehaviorSetPhysicsSliderDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsBallJointDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsCollDetectionDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsContinuousContactDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsBuoyancyDecl);
    RegisterBehavior(reg, FillBehaviorPhysicsResetDecl);
    RegisterBehavior(reg, FillBehaviorGetProfilerValuesDecl);
    RegisterBehavior(reg, FillBehaviorDeleteCollisionSurfacesDecl);
}