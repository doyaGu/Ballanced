#include "CKAll.h"
#include "ToolboxGuids.h"
#include "DXManager.h"
#include "DebugManager.h"
#include "TimeManager.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations Register_TT_Toolbox_BehaviorDeclarations
#define InitInstance _TT_Toolbox_InitInstance
#define ExitInstance _TT_Toolbox_ExitInstance
#define CKGetPluginInfoCount CKGet_TT_Toolbox_PluginInfoCount
#define CKGetPluginInfo CKGet_TT_Toolbox_PluginInfo
#define g_PluginInfo g_TT_Toolbox_PluginInfo
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
    CKParameterManager *pm = context->GetParameterManager();
    pm->RegisterNewFlags(CKPGUID_PROXIMITY, "Proximity", "InRange=1,OutRange=2,EnterRange=4,ExitRange=8");
    pm->RegisterNewFlags(CKPGUID_CHECKAXIS, "Check Axis?", "X=1,Y=2,Z=4");
    pm->RegisterNewFlags(CKPGUID_2DSPRITEINPUT, "Set", "Position=4,Size=2,UV-Mapping=1");
    pm->RegisterNewFlags(CKPGUID_3DSPRITEINPUT, "Set", "Position=8,Size=4,Offset= 2,UV-Mapping=1");

    pm->RegisterNewEnum(CKPGUID_TTAXIS, "Axis", "X=1,Y=2,Z=4,XY=3,XZ=5,YZ=6,XYZ=7");
    pm->RegisterNewEnum(CKPGUID_MAPPING, "Mapping", "Conic=1,Radial=2");

    pm->RegisterNewStructure(CKPGUID_SSHADOW, "SShadow", "Mesh-ID,Static, Shadow-Length, SelfShadow", CKPGUID_INT, CKPGUID_BOOL, CKPGUID_FLOAT, CKPGUID_BOOL);

    new TimeManager(context);
    new DebugManager(context);
    new DXManager(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->UnRegisterParameterType(CKPGUID_PROXIMITY);
    pm->UnRegisterParameterType(CKPGUID_TTAXIS);
    pm->UnRegisterParameterType(CKPGUID_MAPPING);
    pm->UnRegisterParameterType(CKPGUID_SSHADOW);

    return CK_OK;
}

CKPluginInfo g_Toolbox_PluginInfo[4];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 4; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_Toolbox_PluginInfo[0].m_Author = "Terratools";
    g_Toolbox_PluginInfo[0].m_Description = "Tools for NeMo";
    g_Toolbox_PluginInfo[0].m_Extension = "";
    g_Toolbox_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_Toolbox_PluginInfo[0].m_Version = 0x000001;
    g_Toolbox_PluginInfo[0].m_InitInstanceFct = InitInstance;
    g_Toolbox_PluginInfo[0].m_ExitInstanceFct = ExitInstance;
    g_Toolbox_PluginInfo[0].m_GUID = CKGUID(0x64DB589F, 0x330BCE);
    g_Toolbox_PluginInfo[0].m_Summary = "TT Toolbox";

    g_Toolbox_PluginInfo[1].m_Author = "Terratools";
    g_Toolbox_PluginInfo[1].m_Description = "TT Time-Manager";
    g_Toolbox_PluginInfo[1].m_Extension = "";
    g_Toolbox_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_Toolbox_PluginInfo[1].m_Version = 0x000001;
    g_Toolbox_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_Toolbox_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_Toolbox_PluginInfo[1].m_GUID = TT_TIME_MANAGER_GUID;
    g_Toolbox_PluginInfo[1].m_Summary = "TT Time-Manager";

    g_Toolbox_PluginInfo[2].m_Author = "Terratools";
    g_Toolbox_PluginInfo[2].m_Description = "TT Debug-Manager";
    g_Toolbox_PluginInfo[2].m_Extension = "";
    g_Toolbox_PluginInfo[2].m_Type = CKPLUGIN_MANAGER_DLL;
    g_Toolbox_PluginInfo[2].m_Version = 0x000001;
    g_Toolbox_PluginInfo[2].m_InitInstanceFct = InitInstance;
    g_Toolbox_PluginInfo[2].m_ExitInstanceFct = ExitInstance;
    g_Toolbox_PluginInfo[2].m_GUID = TT_DEBUG_MANAGER_GUID;
    g_Toolbox_PluginInfo[2].m_Summary = "TT Debug-Manager";

    g_Toolbox_PluginInfo[3].m_Author = "Terratools";
    g_Toolbox_PluginInfo[3].m_Description = "TT DXManager";
    g_Toolbox_PluginInfo[3].m_Extension = "";
    g_Toolbox_PluginInfo[3].m_Type = CKPLUGIN_MANAGER_DLL;
    g_Toolbox_PluginInfo[3].m_Version = 0x000001;
    g_Toolbox_PluginInfo[3].m_InitInstanceFct = InitInstance;
    g_Toolbox_PluginInfo[3].m_ExitInstanceFct = ExitInstance;
    g_Toolbox_PluginInfo[3].m_GUID = TT_DX_MANAGER_GUID;
    g_Toolbox_PluginInfo[3].m_Summary = "TT DXManager";

    return &g_Toolbox_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorHomingMissileDecl);
    RegisterBehavior(reg, FillBehaviorSetDynamicPositionDecl);
    RegisterBehavior(reg, FillBehaviorFollowCameraDecl);

    RegisterBehavior(reg, FillBehaviorGetCurvePointDecl);
    RegisterBehavior(reg, FillBehaviorIsBoundingSphereIntersectedDecl);
    RegisterBehavior(reg, FillBehaviorCheckCurveCollisionDecl);

    RegisterBehavior(reg, FillBehaviorBitmapFontDecl);
    RegisterBehavior(reg, FillBehaviorLensFlareDecl);
    RegisterBehavior(reg, FillBehaviorSetFSAADecl);
    RegisterBehavior(reg, FillBehaviorSkyAroundDecl);
    RegisterBehavior(reg, FillBehaviorSceneStencilShadowDecl);
    RegisterBehavior(reg, FillBehaviorShadowMappingDecl);
    RegisterBehavior(reg, FillBehaviorShadowPlaneDecl);
    RegisterBehavior(reg, FillBehaviorSwitchOnLayerDecl);
    RegisterBehavior(reg, FillBehaviorConvertBoxDecl);
    RegisterBehavior(reg, FillBehaviorFindSubStringDecl);
    RegisterBehavior(reg, FillBehaviorGetFullAngleDecl);
    RegisterBehavior(reg, FillBehaviorGetGroupBoundingBoxDecl);
    RegisterBehavior(reg, FillBehaviorGetNearestCurvePositionDecl);
    RegisterBehavior(reg, FillBehaviorGetZOrderDecl);
    RegisterBehavior(reg, FillBehaviorLookAtDecl);
    RegisterBehavior(reg, FillBehaviorShowVectorDecl);
    RegisterBehavior(reg, FillBehaviorViewFrustumClippingDecl);

    RegisterBehavior(reg, FillBehaviorChronoDecl);
    RegisterBehavior(reg, FillBehaviorSearchRowDecl);
    RegisterBehavior(reg, FillBehaviorChildrenIteratorDecl);
    RegisterBehavior(reg, FillBehaviorConvertPixelHomogenDecl);
    RegisterBehavior(reg, FillBehaviorConvertTimeToStringDecl);
    RegisterBehavior(reg, FillBehaviorParameterMapperDecl);
    RegisterBehavior(reg, FillBehaviorCounterDecl);
    RegisterBehavior(reg, FillBehaviorSplitStringDecl);

    RegisterBehavior(reg, FillBehaviorBumpMapDecl);
    RegisterBehavior(reg, FillBehaviorSetMappingDecl);
    RegisterBehavior(reg, FillBehaviorReflectionMappingDecl);
    RegisterBehavior(reg, FillBehaviorRefractionMappingDecl);
    RegisterBehavior(reg, FillBehaviorSetMaterialValuesDecl);
    RegisterBehavior(reg, FillBehaviorSetMipMappingDecl);
    RegisterBehavior(reg, FillBehaviorAnisoMappingDecl);
    RegisterBehavior(reg, FillBehaviorSaveAlphaDecl);

    RegisterBehavior(reg, FillBehaviorTextureInfoDecl);
    RegisterBehavior(reg, FillBehaviorFlushTexturesDecl);
    RegisterBehavior(reg, FillBehaviorPreloadTexturesDecl);
    RegisterBehavior(reg, FillBehaviorListAllCurrentUsedTexturesDecl);
    RegisterBehavior(reg, FillBehaviorCelShadingDecl);
    RegisterBehavior(reg, FillBehaviorGeneratePlaneDecl);

    RegisterBehavior(reg, FillBehaviorHighMapMeshformDecl);
    RegisterBehavior(reg, FillBehaviorMagnetMeshFormDecl);
    RegisterBehavior(reg, FillBehaviorSinusMeshdeformDecl);
    RegisterBehavior(reg, FillBehaviorPMSMeshdeformDecl);
    RegisterBehavior(reg, FillBehaviorRippleWaveDecl);

    RegisterBehavior(reg, FillBehaviorIsObjectActiveDecl);
    RegisterBehavior(reg, FillBehaviorScaleableProximityDecl);
    RegisterBehavior(reg, FillBehaviorIsScriptActiveDecl);
    RegisterBehavior(reg, FillBehaviorIsObjectVisibleDecl);
    RegisterBehavior(reg, FillBehaviorActivateObjectDecl);
    RegisterBehavior(reg, FillBehaviorRestoreDynamicICDecl);
    RegisterBehavior(reg, FillBehaviorRestoreICDecl);
    RegisterBehavior(reg, FillBehaviorSaveICDecl);
    RegisterBehavior(reg, FillBehaviorReplaceInputparameterDecl);
    RegisterBehavior(reg, FillBehaviorWriteBackDecl);

    RegisterBehavior(reg, FillBehaviorGetEnvironmentVariableDecl);
    RegisterBehavior(reg, FillBehaviorCopyFileDecl);
    RegisterBehavior(reg, FillBehaviorGetCurrentDirectoryDecl);
    RegisterBehavior(reg, FillBehaviorSetCurrentDirectoryDecl);
    RegisterBehavior(reg, FillBehaviorCreateDirectoryDecl);
    RegisterBehavior(reg, FillBehaviorExistFileDecl);
    RegisterBehavior(reg, FillBehaviorDeleteFileDecl);
    RegisterBehavior(reg, FillBehaviorRemoveDirectoryDecl);
    RegisterBehavior(reg, FillBehaviorReplacePathDecl);
    RegisterBehavior(reg, FillBehaviorListDirDecl);
    RegisterBehavior(reg, FillBehaviorGetFileNameFromPathDecl);

    RegisterBehavior(reg, FillBehaviorPushButtonDecl);
    RegisterBehavior(reg, FillBehaviorPushButton2Decl);

    RegisterBehavior(reg, FillBehaviorJoystickVectorMapperDecl);
    RegisterBehavior(reg, FillBehaviorKeySwitchDecl);
    RegisterBehavior(reg, FillBehaviorInputStringDecl);
    RegisterBehavior(reg, FillBehaviorTestForJoystickDecl);
    RegisterBehavior(reg, FillBehaviorKeyWaiterDecl);
    RegisterBehavior(reg, FillBehaviorJoystickWaiterDecl);

    RegisterBehavior(reg, FillBehaviorTimerDecl);
    RegisterBehavior(reg, FillBehaviorGetTimeDecl);

    RegisterBehavior(reg, FillBehaviorMotorSoundDecl);
    RegisterBehavior(reg, FillBehaviorLoadMotorSettingsDecl);
    RegisterBehavior(reg, FillBehaviorLinearVolumeDecl);

    RegisterBehavior(reg, FillBehaviorDebugONDecl);
    RegisterBehavior(reg, FillBehaviorDebugOFFDecl);
    RegisterBehavior(reg, FillBehaviorDebugDecl);

    RegisterBehavior(reg, FillBehaviorSet2DSpriteOldDecl);
    RegisterBehavior(reg, FillBehaviorSet2DSpriteDecl);
    RegisterBehavior(reg, FillBehaviorSet3DSpriteDecl);
    RegisterBehavior(reg, FillBehaviorGetUV2DSpriteDecl);
    RegisterBehavior(reg, FillBehaviorFontCoordinatesToDataArrayDecl);
    RegisterBehavior(reg, FillBehaviorCreateFontExDecl);

    RegisterBehavior(reg, FillBehaviorGetMemoryStatusDecl);
    RegisterBehavior(reg, FillBehaviorOperationSystemDecl);
}