//
// Visuals.cpp : Defines the initialization routines for the DLL.
//

#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_Visuals_BehaviorDeclarations
#define InitInstance                    _Visuals_InitInstance
#define ExitInstance                    _Visuals_ExitInstance
#define CKGetPluginInfoCount            CKGet_Visuals_PluginInfoCount
#define CKGetPluginInfo                 CKGet_Visuals_PluginInfo
#define g_PluginInfo                    g_Visuals_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
CKERROR InitInstance(CKContext *context);
CKERROR ExitInstance(CKContext *context);

#define VISUALS_BEHAVIOR CKGUID(0x1a357e90, 0x30f82648)
#define CKPGUID_SPRITEEDITION CKGUID(0x11cb48a5, 0x47e5424b)
#define CKPGUID_TEXTALIGN CKGUID(0x11223faf, 0x1a9315f9)
#define CKPGUID_CURVEMODE CKGUID(0x4d236301, 0x5be530bc)
#define CKPGUID_GENERALSTAT CKGUID(0xfe82e7c, 0x98173f)
#define CKPGUID_BEHAVIORSTAT CKGUID(0x3b542289, 0x2f627abb)
#define CKPGUID_RENDERSTAT CKGUID(0x54a578a6, 0x53f144d4)

void ZBufOnCallbackFunc(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);
void ShadowCasterAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg);

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Visual building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = VISUALS_BEHAVIOR;
    g_PluginInfo.m_Summary = "Visuals";
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
#define CKPGUID_REFLECTED CKGUID(0x7e3745c9, 0x79a84e4a)

CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->RegisterNewEnum(CKPGUID_TEXTALIGN, "Text Alignment", "Left=1,Right=2,Center=3");
    pm->RegisterNewEnum(CKPGUID_CURVEMODE, "Curve Rendering", "Plane=0,Cross=1,Tube=2,2D=3,Line=4");

    // Settings Flags for sprite edition
    pm->RegisterNewFlags(CKPGUID_SPRITEEDITION, "SpriteEdition", "Position=1,Z Ordering=2,Background=4,Size=8,Pickable=16,Clip To Parent=32,Parent Alignment=64");

    // Settings Flags for statistic behavior
    pm->RegisterNewFlags(CKPGUID_GENERALSTAT, "General Time Statistics", "Interface=1,Processing=2,Rendering=4,Parametric Operation=8,Animation Management=16,IK Management=32,Total Behavior=64,Behavior Execution=128");
    pm->RegisterNewFlags(CKPGUID_BEHAVIORSTAT, "Behavioral Statistics", "Active Objects Executed=1,Behaviors Executed=2,Building Blocks Executed=4,Links Parsed=8,Links Delayed=16");
    pm->RegisterNewFlags(CKPGUID_RENDERSTAT, "Renderal Statistics", "Faces Drawn=1, Lines Drawn=2, Points Drawn=4, Objects Drawn=8, Vertices Processed=16");

    // This parameter will only be used in the behavior so we hide it
    // so that it does not appear in the available parameters in the schematic window
    CKParameterTypeDesc *param_type = pm->GetParameterTypeDescription(CKPGUID_SPRITEEDITION);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_GENERALSTAT);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_BEHAVIORSTAT);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_RENDERSTAT);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    // Reflected Objects Structure
    pm->RegisterNewStructure(CKPGUID_REFLECTED, "", "Group,Mesh", CKPGUID_FILTER, CKPGUID_MESH);
    param_type = pm->GetParameterTypeDescription(CKPGUID_REFLECTED);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    ///////////////////////////////////
    // ATTRIBUTES
    ///////////////////////////////////
    CKAttributeManager *attman = context->GetAttributeManager();
    int att;

    attman->AddCategory("Visuals FX");

    // Reflected Objects Attribute
    att = attman->RegisterNewAttributeType("Reflected Object", CKPGUID_REFLECTED, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeDefaultValue(att, "1;NULL");

    // Planar Shadow Attribute
    att = attman->RegisterNewAttributeType("Planar Shadow Object", CKPGUID_REFLECTED, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeDefaultValue(att, "1;NULL");

    // ZBuffer Only Attribute
    att = attman->RegisterNewAttributeType("ZBuffer Only", CKPGUID_NONE, CKCID_3DENTITY);
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeCallbackFunction(att, ZBufOnCallbackFunc, NULL);

    // Shadow Caster Attribute
    att = attman->RegisterNewAttributeType("Shadow Caster Receiver", CKPGUID_NONE, CKCID_3DOBJECT);
    attman->SetAttributeCategory(att, "Visuals FX");
    attman->SetAttributeCallbackFunction(att, ShadowCasterAttributeCallback, NULL);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    CKAttributeManager *attman = context->GetAttributeManager();

    pm->UnRegisterParameterType(CKPGUID_TEXTALIGN);
    pm->UnRegisterParameterType(CKPGUID_SPRITEEDITION);
    pm->UnRegisterParameterType(CKPGUID_GENERALSTAT);
    pm->UnRegisterParameterType(CKPGUID_BEHAVIORSTAT);
    pm->UnRegisterParameterType(CKPGUID_RENDERSTAT);
    pm->UnRegisterParameterType(CKPGUID_REFLECTED);

    attman->UnRegisterAttribute("Reflected Object");
    attman->UnRegisterAttribute("Planar Shadow Object");
    attman->UnRegisterAttribute("ZBuffer Only");
    attman->UnRegisterAttribute("Shadow Caster Receiver");

    return CK_OK;
}

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Visuals/2D
    RegisterBehavior(reg, FillBehaviorChangeSpriteSlotDecl);
    RegisterBehavior(reg, FillBehaviorDisplayProgressionBarDecl);
    RegisterBehavior(reg, FillBehaviorDisplay2DSpriteDecl);
    RegisterBehavior(reg, FillBehaviorSetColorKey2dDecl);
    RegisterBehavior(reg, FillBehaviorSetRectanglesDecl);
    RegisterBehavior(reg, FillBehaviorSet2DParentDecl);
    RegisterBehavior(reg, FillBehaviorSet2DPositionDecl);
    RegisterBehavior(reg, FillBehaviorSet2DMaterialDecl);

    // Visuals/FX
    RegisterBehavior(reg, FillBehaviorBlinkDecl);
    RegisterBehavior(reg, FillBehaviorMakeTransparentDecl);
    RegisterBehavior(reg, FillBehaviorMeshLightingDecl);
    RegisterBehavior(reg, FillBehaviorBlurObjectDecl);
    RegisterBehavior(reg, FillBehaviorBlurDecl);
    RegisterBehavior(reg, FillBehaviorPlanarFilterDecl);
    RegisterBehavior(reg, FillBehaviorSetZBufDecl);
    RegisterBehavior(reg, FillBehaviorSetZOrderDecl);
    RegisterBehavior(reg, FillBehaviorSimpleShadowDecl);
    RegisterBehavior(reg, FillBehaviorSolidTrailDecl);
    RegisterBehavior(reg, FillBehaviorTextureRenderDecl);
    //	RegisterBehavior(reg, FillBehaviorCaptureScreenRegionDecl);
    RegisterBehavior(reg, FillBehaviorUseZDecl);
    RegisterBehavior(reg, FillBehaviorPlanarReflectionDecl);
    RegisterBehavior(reg, FillBehaviorPlanarShadowDecl);
    RegisterBehavior(reg, FillBehaviorShadowCasterDecl);
    RegisterBehavior(reg, FillBehaviorRenderCurveDecl);
    RegisterBehavior(reg, FillBehaviorMarkSystemDecl);

    // Visuals/Show-Hide
    RegisterBehavior(reg, FillBehaviorHideDecl);
    RegisterBehavior(reg, FillBehaviorHierarchyHideDecl);
    RegisterBehavior(reg, FillBehaviorHide2dDecl);
    RegisterBehavior(reg, FillBehaviorShowDecl);
    RegisterBehavior(reg, FillBehaviorShow2dDecl);
    RegisterBehavior(reg, FillBehaviorShowBoundingBoxDecl);
    RegisterBehavior(reg, FillBehaviorShowMouseCursorDecl);

    // Visuals/Sprite
    RegisterBehavior(reg, FillBehaviorSet3DSpriteModeDecl);
    RegisterBehavior(reg, FillBehaviorSpriteMoviePlayerDecl);
    RegisterBehavior(reg, FillBehaviorSpriteMultiAngleDecl);

    // Visuals/Obsolete
    RegisterBehavior(reg, FillBehaviorDisplayScoreDecl);
    RegisterBehavior(reg, FillBehaviorFpsDecl);

    RegisterBehavior(reg, FillBehaviorStatsDecl);

    // World Environments/Global
    RegisterBehavior(reg, FillBehaviorSetRenderOptionsDecl);
}
