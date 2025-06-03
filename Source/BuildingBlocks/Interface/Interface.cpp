// Interface.cpp : Defines the initialization routines for the behavior DLL.
//
#include "CKAll.h"

#include "CKFontManager.h"
#include "Layout.h"

#define INTERFACE_BEHAVIOR CKGUID(0x44700134, 0x553862c0)

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Interface_BehaviorDeclarations
    #define InitInstance					_Interface_InitInstance
    #define ExitInstance					_Interface_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Interface_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Interface_PluginInfo
    #define g_PluginInfo					g_Interface_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

#define CKPGUID_PUSHBUTTONOPTION CKGUID(0x25064124, 0x1fbe6aae)

CKERROR InitInstance(CKContext *context)
{
    // Manager
    new CKFontManager(context);

    CKParameterManager *pm = context->GetParameterManager();

    ///
    // Fonts / Text
    pm->RegisterNewEnum(CKPGUID_ALIGNMENT, "Alignment", "Top-Left=5,Top=4,Top-Right=6,Left=1,Center=0,Right=2,Bottom-Left=9,Bottom=8,Bottom-Right=10");
    pm->RegisterNewFlags(CKPGUID_FONTPROPERTIES, "", "Gradient=1,Shadow=2,Lighting=4,Disable Filter=8");
    pm->RegisterNewFlags(CKPGUID_TEXTPROPERTIES, "", "Screen Proportionnal=1,Background=2,Clip To Dimension=4,Resize Verticaly=8,Resize Horizontaly=16,WordWrap=32,Justified=64,Compiled=128,Multiple=256,Show Caret=512");

#ifndef FONTMANAGER_NOSYSFONT
    // System font
    pm->RegisterNewEnum(CKPGUID_FONTWEIGHT, "Font Weight", "THIN=100,EXTRALIGHT=200,LIGHT=300,NORMAL=400,MEDIUM=500,DEMIBOLD=600,BOLD=700,EXTRABOLD=800,HEAVY=900");
    pm->RegisterNewEnum(CKPGUID_FONTRESOLUTION, "Font Resolution", "128x128=1,256x256=2,512x512=4,1024x1024=8");
    pm->RegisterNewEnum(CKPGUID_FONTNAME, "Font Name", "");
#endif

    // Mouse Pointers
#define CKPGUID_MOUSEPOINTERS CKGUID(0x37a05bdd, 0x1ed83f40)
    pm->RegisterNewEnum(CKPGUID_MOUSEPOINTERS, "Mouse Pointers", "Normal Select=1,Busy=2,Move=3,Link Select=4");

    // Layouts
    pm->RegisterNewEnum(CKPGUID_FLOW_DIRECTION, "Flow Direction", "Left/Right=1,Up/Down=2,Rear/Front=4");
    pm->RegisterNewEnum(CKPGUID_FLOW_ALIGNMENT, "Flow Alignment", "Left=1,Center=2,Right=4");
    pm->RegisterNewEnum(CKPGUID_FLOW_SUPPORT, "Flow Support", "Front=1, In=2, Rear=4");

    // Controls
    pm->RegisterNewFlags(CKPGUID_PUSHBUTTONOPTION, "Button Events", "Pressed=1,Released=2,Active=4,Enter Button=8,Exit Button=16,In Button=32");

    CKParameterTypeDesc *param_type;
    param_type = pm->GetParameterTypeDescription(CKPGUID_FONTPROPERTIES);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_TEXTPROPERTIES);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    // Manager
    CKFontManager *fm = (CKFontManager *)context->GetManagerByGuid(FONT_MANAGER_GUID);
    delete fm;

    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(CKPGUID_ALIGNMENT);
    pm->UnRegisterParameterType(CKPGUID_FONTPROPERTIES);
    pm->UnRegisterParameterType(CKPGUID_TEXTPROPERTIES);

    // Layouts
    pm->UnRegisterParameterType(CKPGUID_FLOW_DIRECTION);
    pm->UnRegisterParameterType(CKPGUID_FLOW_ALIGNMENT);
    pm->UnRegisterParameterType(CKPGUID_FLOW_SUPPORT);

#ifndef FONTMANAGER_NOSYSFONT
    // System fonts
    pm->UnRegisterParameterType(CKPGUID_FONTWEIGHT);
#endif

    pm->UnRegisterParameterType(CKPGUID_MOUSEPOINTERS);

    return CK_OK;
}

CKPluginInfo g_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo[0].m_Author = "Virtools";
    g_PluginInfo[0].m_Description = "Interface Building Blocks";
    g_PluginInfo[0].m_Extension = "";
    g_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo[0].m_Version = 0x000001;
    g_PluginInfo[0].m_InitInstanceFct = NULL;
    g_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_PluginInfo[0].m_GUID = INTERFACE_BEHAVIOR;
    g_PluginInfo[0].m_Summary = "Interface Building Blocks";

    g_PluginInfo[1].m_Author = "Virtools";
    g_PluginInfo[1].m_Description = "Textured Font Manager";
    g_PluginInfo[1].m_Extension = "";
    g_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_PluginInfo[1].m_Version = 0x000001;
    g_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_PluginInfo[1].m_GUID = FONT_MANAGER_GUID;
    g_PluginInfo[1].m_Summary = CKFontManager::Name;

    return &g_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Layouts
    RegisterBehavior(reg, FillBehaviorAlign2DDecl);
    RegisterBehavior(reg, FillBehaviorAlign3DDecl);
    RegisterBehavior(reg, FillBehaviorCurveLayout2DDecl);
    RegisterBehavior(reg, FillBehaviorCurveLayout3DDecl);
    RegisterBehavior(reg, FillBehaviorFlowLayout2DDecl);
    RegisterBehavior(reg, FillBehaviorFlowLayout3DDecl);
    RegisterBehavior(reg, FillBehaviorGridLayout2DDecl);
    RegisterBehavior(reg, FillBehaviorGridLayout3DDecl);

    // Controls
    RegisterBehavior(reg, FillBehaviorDragNDropDecl);
    RegisterBehavior(reg, FillBehaviorPushButtonDecl);
    //	RegisterBehavior(reg,FillBehaviorSliderDecl);

    // Text
    RegisterBehavior(reg, FillBehaviorText2DDecl);
    RegisterBehavior(reg, FillBehaviorText3DDecl);
    RegisterBehavior(reg, FillBehaviorSetCaretPositionDecl);
    // Old
    RegisterBehavior(reg, FillBehaviorBitmapTextDisplayDecl);
    RegisterBehavior(reg, FillBehaviorTextDisplayDecl);

    // Screen
    RegisterBehavior(reg, FillBehaviorPicking2dDecl);
    RegisterBehavior(reg, FillBehaviorCameraViewDecl);
    RegisterBehavior(reg, FillBehaviorMouseCursorDecl);
    RegisterBehavior(reg, FillBehaviorResizeViewDecl);
    RegisterBehavior(reg, FillBehaviorSetPickableDecl);
    RegisterBehavior(reg, FillBehaviorGetProportionalScreenPosDecl);
    RegisterBehavior(reg, FillBehaviorGetScreenPosProportionalDecl);

    // Fonts
    RegisterBehavior(reg, FillBehaviorCreateFontDecl);
    RegisterBehavior(reg, FillBehaviorDeleteFontDecl);
    RegisterBehavior(reg, FillBehaviorSetFontPropertiesDecl);
#ifndef FONTMANAGER_NOSYSFONT
    // System font blocks
    RegisterBehavior(reg, FillBehaviorCreateSystemFontDecl);
    // Mouse Cursor System
    RegisterBehavior(reg, FillBehaviorMouseCursorSystemDecl);
#endif

    // Primitives
    RegisterBehavior(reg, FillBehaviorDrawRectangleDecl);
}