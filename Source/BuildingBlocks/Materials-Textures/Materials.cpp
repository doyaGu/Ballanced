//
// Materials.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_Materials_BehaviorDeclarations
#define InitInstance                    _Materials_InitInstance
#define ExitInstance                    _Materials_ExitInstance
#define CKGetPluginInfoCount            CKGet_Materials_PluginInfoCount
#define CKGetPluginInfo                 CKGet_Materials_PluginInfo
#define g_PluginInfo                    g_Materials_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

void A_listparameteroperation();

#define MATERIALS_BEHAVIOR CKGUID(0x4c47773a, 0x108369ae)
#define CKPGUID_WRITEMODE CKDEFINEGUID(0x675e3903, 0x7a06003c)
#define CKPGUID_TEXTUREVIDEOFORMAT CKGUID(0x2c1e5d83, 0x37b9284b)

CKERROR InitInstance(CKContext *context);
CKERROR ExitInstance(CKContext *context);

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Material building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = MATERIALS_BEHAVIOR;
    g_PluginInfo.m_Summary = "Materials";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    // Write Mode for Write In Texture Behavior
    pm->RegisterNewEnum(CKPGUID_WRITEMODE, "Write Mode", "Replace=0,Blend=1,Add Saturate=2,Add Unsat.=3");

    // This parameter will only be used in the behavior so we hide it
    // so that it does not appear in the available parameters in the schematic window
    CKParameterTypeDesc *param_type = pm->GetParameterTypeDescription(CKPGUID_WRITEMODE);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

#define CKPGUID_ENVMAPPINGFLIPMODE CKGUID(0x11416cd2, 0x293c701e)
    // Write Mode for Write In Texture Behavior
    pm->RegisterNewFlags(CKPGUID_ENVMAPPINGFLIPMODE, "", "FlipU=1,FlipV=2,Use Ref Orientation=4,Reflective Mapping=8");

    // This parameter will only be used in the behavior so we hide it
    // so that it does not appear in the available parameters in the schematic window
    param_type = pm->GetParameterTypeDescription(CKPGUID_ENVMAPPINGFLIPMODE);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    // Texture format enum
    CKERROR er = pm->RegisterNewEnum(CKPGUID_TEXTUREVIDEOFORMAT, "Texture Video Format",
                                     "32 bits ARGB 8888=0,32 bits RGB 888=1,24 bits RGB 888=2,16 bits RGB 565=3,16 bits RGB 555=4,16 bits ARGB 1555=5,16 bits ARGB 4444=6,8 bits RGB 332=7,8 bits ARGB 2222=8,Compressed DXT1=9,Compressed DXT3=10,Compressed DXT5=11,V8U8 BumpMap=12,L6V5U5 BumpMap=13");

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(CKPGUID_WRITEMODE);
    pm->UnRegisterParameterType(CKPGUID_TEXTUREVIDEOFORMAT);

    return CK_OK;
}

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
/**********************************************************************************/
/**********************************************************************************/
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Materials/Animation
    RegisterBehavior(reg, FillBehaviorMoviePlayerDecl);
    RegisterBehavior(reg, FillBehaviorTextureScrollerDecl);
    RegisterBehavior(reg, FillBehaviorTextureSinusDecl);

    // Materials/Basic
    RegisterBehavior(reg, FillBehaviorChangeTextureSlotDecl);
    RegisterBehavior(reg, FillBehaviorSetAmbientDecl);
    RegisterBehavior(reg, FillBehaviorSetColorKeyDecl);
    RegisterBehavior(reg, FillBehaviorSetDestBlendDecl);
    RegisterBehavior(reg, FillBehaviorSetBlendModesDecl);
    RegisterBehavior(reg, FillBehaviorSetDiffuseDecl);
    RegisterBehavior(reg, FillBehaviorSetEmissiveDecl);
    RegisterBehavior(reg, FillBehaviorSetFillModeDecl);
    RegisterBehavior(reg, FillBehaviorSetLitModeDecl);
    RegisterBehavior(reg, FillBehaviorSetMaterialDecl);
    RegisterBehavior(reg, FillBehaviorSetMaterialSprite3DDecl);
    RegisterBehavior(reg, FillBehaviorSetPowerDecl);
    RegisterBehavior(reg, FillBehaviorSetPrelitColorDecl);
    RegisterBehavior(reg, FillBehaviorSetShadeModeDecl);
    RegisterBehavior(reg, FillBehaviorSetSpecularDecl);
    RegisterBehavior(reg, FillBehaviorSetScrBlendDecl);
    RegisterBehavior(reg, FillBehaviorSetTextureDecl);
    RegisterBehavior(reg, FillBehaviorSetTextureMagDecl);
    RegisterBehavior(reg, FillBehaviorSetTextureMinDecl);
    RegisterBehavior(reg, FillBehaviorSetTransparentDecl);
    RegisterBehavior(reg, FillBehaviorSetTwoSidedDecl);
    RegisterBehavior(reg, FillBehaviorSetWrapModeDecl);
    RegisterBehavior(reg, FillBehaviorSetZBufferDecl);
    RegisterBehavior(reg, FillBehaviorSetAlphaTestDecl);

    // Materials/Channel
    RegisterBehavior(reg, FillBehaviorActivateChannelDecl);
    RegisterBehavior(reg, FillBehaviorAddChannelDecl);
    RegisterBehavior(reg, FillBehaviorRemoveChannelDecl);
    RegisterBehavior(reg, FillBehaviorSetChannelDestBlendDecl);
    RegisterBehavior(reg, FillBehaviorSetChannelMaterialDecl);
    RegisterBehavior(reg, FillBehaviorSetChannelSrcBlendDecl);

    // Materials/Texture
    RegisterBehavior(reg, FillBehaviorWriteInTextureDecl);
    RegisterBehavior(reg, FillBehaviorSetMipMapLevelDecl);
    RegisterBehavior(reg, FillBehaviorChangeTextureSizeDecl);
    RegisterBehavior(reg, FillBehaviorChangeVideoFormatDecl);

    // Materials/Mapping
    RegisterBehavior(reg, FillBehaviorEnvMappingDecl);
    RegisterBehavior(reg, FillBehaviorPlanarMappingDecl);
    RegisterBehavior(reg, FillBehaviorCylindricalMappingDecl);
    RegisterBehavior(reg, FillBehaviorSphericalMappingDecl);
    RegisterBehavior(reg, FillBehaviorScreenMappingDecl);
    //	RegisterBehavior(reg, FillBehaviorScreenMapping2Decl);
    RegisterBehavior(reg, FillBehaviorSetUVsDecl);

    RegisterBehavior(reg, FillBehaviorCreateMipMapTextureDecl);

    // A_listparameteroperation();
}

/**********************************************************************************/
/**********************************************************************************/
/*
/* GetMultiPassMaxTextureNumber
/*
/* Used to switch methods of mapping
/* If single pass support we shouldn't clip out range faces
/*
/* return: number of supported single pass textures, or -1 if problem
/**********************************************************************************/
/**********************************************************************************/
int GetSinglePassMaxTextureNumber(CKContext *ctx)
{
    if (CKRenderManager *rm = ctx->GetRenderManager())
    {
        if (CKRenderContext *rc = rm->GetRenderContext(0))
        {
            int current_driver = rc->GetDriverIndex();
            if (VxDriverDesc *desc = rm->GetRenderDriverDescription(current_driver))
            {
                return desc->Caps3D.MaxNumberTextureStage;
            }
        }
    }
    return -1;
}
