// BuildingBlocksAddons1.cpp : Defines the entry point for the DLL application.
//
#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_BBAddons_BehaviorDeclarations
#define InitInstance                    _BBAddons_InitInstance
#define ExitInstance                    _BBAddons_ExitInstance
#define CKGetPluginInfoCount            CKGet_BBAddons_PluginInfoCount
#define CKGetPluginInfo                 CKGet_BBAddons_PluginInfo
#define g_PluginInfo                    g_BBAddons_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

#if defined(BBADONS_2_0_0_14)
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
#else
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
#endif

#define ADDONS1_BEHAVIOR CKGUID(0x42c66d45, 0x4d7c47f6)
#define CKPGUID_GAUGE_SIZEMODE (0x2e5645ba, 0x75f43767)
#define CKPGUID_GAUGE_ORIENTATION (0x3391466e, 0x27ce3568)
#define CKPGUID_FRAMESYNC CKDEFINEGUID(0x5e4c1dcf, 0x662c4495)
#define CKPGUID_BEHAVSYNC CKDEFINEGUID(0x677f3843, 0x4e7a3fd0)
#define CKPGUID_APIENUM CKDEFINEGUID(0xebe7d72, 0x4a07b9f)

CKPluginInfo g_PluginInfo;

#if defined(BBADONS_2_0_0_14)
int CKGetPluginInfoCount()
{
    return 1;
}
#else
PLUGIN_EXPORT int CKGetPluginInfoCount()
{
    return 1;
}
#endif

void ShadowStencilAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);
void ShadowStencilMeshAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);

char *shadowStencilCasterAttributeName = "Shadow Stencil Caster";
char *shadowStencilCasterMeshAttributeName = "SSCMesh";

CKERROR InitInstance(CKContext *context)
{
#if !defined(BBADONS_2_0_0_14)
    ///////////////////////////////////
    // ATTRIBUTES
    ///////////////////////////////////
    CKAttributeManager *attman = context->GetAttributeManager();
    int att;
    CKParameterManager *pm = context->GetParameterManager();

    // Stencil Caster Attribute
    att = attman->RegisterNewAttributeType(shadowStencilCasterAttributeName, CKPGUID_NONE, CKCID_3DOBJECT);
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeCallbackFunction(att, ShadowStencilAttributeCallback, NULL);

    // Stencil Caster Mesh Attribute
    att = attman->RegisterNewAttributeType(shadowStencilCasterMeshAttributeName, CKPGUID_POINTER, CKCID_MESH, (CK_ATTRIBUT_FLAGS)(CK_ATTRIBUT_DONOTSAVE | CK_ATTRIBUT_HIDDEN | CK_ATTRIBUT_SYSTEM));
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeCallbackFunction(att, ShadowStencilMeshAttributeCallback, NULL);

    // Time Settings
    pm->RegisterNewEnum(CKPGUID_FRAMESYNC, "Frame Rate", "Free=1,Synchronize to Screen=2,Limit=3");
    pm->RegisterNewEnum(CKPGUID_BEHAVSYNC, "Behavioral Rate", "Linked to Frame Rate=1,Limit=2");

    // Hardware Level enum
    pm->RegisterNewEnum(CKPGUID_APIENUM, "Rasterizer Api", "Unknown=0,OpenGL=1,DirectX5=2,DirectX7=3,DirectX8=4,DirectX9=5");
#endif

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
#if !defined(BBADONS_2_0_0_14)
    CKParameterManager *pm = context->GetParameterManager();
    CKAttributeManager *attman = context->GetAttributeManager();

    attman->UnRegisterAttribute(shadowStencilCasterAttributeName);
    attman->UnRegisterAttribute(shadowStencilCasterMeshAttributeName);
    pm->UnRegisterParameterType(CKPGUID_FRAMESYNC);
    pm->UnRegisterParameterType(CKPGUID_BEHAVSYNC);
#endif

    return CK_OK;
}

#if defined(BBADONS_2_0_0_14)
CKPluginInfo *CKGetPluginInfo(int Index)
#else
PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
#endif
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Building blocks AddOns 1";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = ADDONS1_BEHAVIOR;
    g_PluginInfo.m_Summary = "AddOns1";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
#if defined(BBADONS_2_0_0_14)
    RegisterBehavior(reg, FillBehaviorCombineTextureDecl);
    RegisterBehavior(reg, FillBehaviorGetTextureInfoDecl);
    RegisterBehavior(reg, FillBehaviorMovieLoaderDecl);
#else
    RegisterBehavior(reg, FillBehaviorAnimRecorderDecl);
    RegisterBehavior(reg, FillBehaviorCopyBufferToTextureDecl);
    // Anims
    RegisterBehavior(reg, FillBehaviorSynchroCatcherDecl);
    RegisterBehavior(reg, FillBehaviorSynchroPulsarDecl);

    // FX
    RegisterBehavior(reg, FillBehaviorLensFlareDecl);
    RegisterBehavior(reg, FillBehaviorReplaceRenderingDecl);

    // Materials
    RegisterBehavior(reg, FillBehaviorCombineTextureDecl);
    RegisterBehavior(reg, FillBehaviorFilterTextureDecl);

    // Procedural textures
    RegisterBehavior(reg, FillBehaviorWaterTextureDecl);
    RegisterBehavior(reg, FillBehaviorBlendTexturesDecl);

    // Optimization System
    RegisterBehavior(reg, FillBehaviorHardwareLevelDecl);
    // RegisterBehavior(reg, FillBehaviorFullscreenResolutionsDecl);

    // Volumetric
    RegisterBehavior(reg, FillBehaviorVolumetricFogDecl);

    // Shadow Stencil
    RegisterBehavior(reg, FillBehaviorShadowStencilDecl);

    // Light Map
    RegisterBehavior(reg, FillBehaviorLightMapDecl);
    RegisterBehavior(reg, FillBehaviorStaticLightMapDecl);

    // Gauge (Visual/2D)
    RegisterBehavior(reg, FillBehaviorGaugeDecl);

    // Time
    RegisterBehavior(reg, FillBehaviorTimeSettingsDecl);

    // Texture
    RegisterBehavior(reg, FillBehaviorGetTextureInfoDecl);
    RegisterBehavior(reg, FillBehaviorMovieLoaderDecl);
    RegisterBehavior(reg, FillBehaviorPixelValueDecl);

    // Get Special Parameters Info
    // RegisterBehavior(reg, FillBehaviorGetSpecialParameterInfoDecl);

    // Debug
    RegisterBehavior(reg, FillBehaviorOutputToConsoleDecl);

    // Listener Settings
    RegisterBehavior(reg, FillBehaviorListenerPropertiesDecl);

    // Create Decal
    RegisterBehavior(reg, FillBehaviorCreateDecalDecl);

    // Set Material Effect
    RegisterBehavior(reg, FillBehaviorSetMaterialEffectDecl);

    // Config
    RegisterBehavior(reg, FillBehaviorReadConfigDecl);
    RegisterBehavior(reg, FillBehaviorWriteConfigDecl);
#endif
}
