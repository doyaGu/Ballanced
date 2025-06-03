//
// Narratives.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_Narratives_BehaviorDeclarations
    #define InitInstance					_Narratives_InitInstance
    #define ExitInstance					_Narratives_ExitInstance
    #define CKGetPluginInfoCount			CKGet_Narratives_PluginInfoCount
    #define CKGetPluginInfo					CKGet_Narratives_PluginInfo
    #define g_PluginInfo					g_Narratives_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

#define NARRATIVES_BEHAVIOR CKGUID(0x5bc80700, 0x24183459)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Narratives Building Blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = NULL;
    g_PluginInfo.m_ExitInstanceFct = NULL;
    g_PluginInfo.m_GUID = NARRATIVES_BEHAVIOR;
    g_PluginInfo.m_Summary = "Narratives Building Blocks";
    return &g_PluginInfo;
}

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Scene Management
    RegisterBehavior(reg, FillBehaviorGetCurrentSceneDecl);
    RegisterBehavior(reg, FillBehaviorLaunchSceneDecl);
    RegisterBehavior(reg, FillBehaviorAddToSceneDecl);
    RegisterBehavior(reg, FillBehaviorRemoveFromSceneDecl);

    RegisterBehavior(reg, FillBehaviorSaveICDecl);
    RegisterBehavior(reg, FillBehaviorRestoreICDecl);
    RegisterBehavior(reg, FillBehaviorSaveStateDecl);
    RegisterBehavior(reg, FillBehaviorReadStateDecl);
    RegisterBehavior(reg, FillBehaviorEnterStateDecl);
    RegisterBehavior(reg, FillBehaviorIsInStateDecl);
    RegisterBehavior(reg, FillBehaviorExitStateDecl);

    // Object Management
    RegisterBehavior(reg, FillObjectCreatorDecl);
    RegisterBehavior(reg, FillObjectLoaderDecl);
    RegisterBehavior(reg, FillObjectCopierDecl);
    RegisterBehavior(reg, FillObjectDeleterDecl);
    RegisterBehavior(reg, FillObjectRenameDecl);
    RegisterBehavior(reg, FillObsoleteObjectRenamerDecl);
    RegisterBehavior(reg, FillDynamicDeleterDecl);

    RegisterBehavior(reg, FillTextureLoaderDecl);
    RegisterBehavior(reg, FillSpriteLoaderDecl);
    RegisterBehavior(reg, FillSoundLoaderDecl);

    // Scripts Management
    RegisterBehavior(reg, FillBehaviorActivateObjectDecl);
    RegisterBehavior(reg, FillBehaviorActivateScriptDecl);
    RegisterBehavior(reg, FillBehaviorDeactivateObjectDecl);
    RegisterBehavior(reg, FillBehaviorDeactivateScriptDecl);
    RegisterBehavior(reg, FillBehaviorExecuteScriptDecl);
    RegisterBehavior(reg, FillBehaviorCallScriptDecl);
    RegisterBehavior(reg, FillBehaviorAttachScriptDecl);

    RegisterBehavior(reg, FillBehaviorDUMMY0Decl);
}

CKERROR CreateDUMMY0Proto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("yop");
    if (!proto)
        return CKERR_OUTOFMEMORY;
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(NULL);

    *pproto = proto;
    return CK_OK;
}

CKObjectDeclaration *FillBehaviorDUMMY0Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Dummy0");
    od->SetDescription("");
    od->SetCategory("");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd0b7adf3, 0xd3ff3cf6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDUMMY0Proto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(CKGUID(0x44700134, 0x553862c0)); // INTERFACE_BEHAVIOR
    return od;
}