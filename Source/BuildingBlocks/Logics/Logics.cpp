//
// Logics.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_Logics_BehaviorDeclarations
#define InitInstance                    _Logics_InitInstance
#define ExitInstance                    _Logics_ExitInstance
#define CKGetPluginInfoCount            CKGet_Logics_PluginInfoCount
#define CKGetPluginInfo                 CKGet_Logics_PluginInfo
#define g_PluginInfo                    g_Logics_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

CKERROR InitInstance(CKContext *context);
CKERROR ExitInstance(CKContext *context);
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

#define LOGICS_BEHAVIOR CKGUID(0x6f1369c8, 0xf561b89)

CKPluginInfo g_PluginInfo;

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 1; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    //	InitVrt();
    g_PluginInfo.m_Author = "Virtools";
    g_PluginInfo.m_Description = "Logic building blocks";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = InitInstance;
    g_PluginInfo.m_ExitInstanceFct = ExitInstance;
    g_PluginInfo.m_GUID = LOGICS_BEHAVIOR;
    g_PluginInfo.m_Summary = "Logics";
    return &g_PluginInfo;
}

#define CKPGUID_INTERSECTIONPRECISIONTYPE CKDEFINEGUID(0x6cf55733, 0x5af72dae)
#define CKPGUID_RECTBOXMODE CKDEFINEGUID(0x5a6a3bd9, 0x7e2797d)
#define CKPGUID_PROXIMITY CKDEFINEGUID(0x7fff5699, 0x7571336d)

extern void InitializeModifyComponent(CKContext *);
extern void DestroyModifyComponent(CKContext *);

/**********************************************************************************/
/**********************************************************************************/
CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    CKParameterTypeDesc *param_type;

    pm->RegisterNewEnum(CKPGUID_INTERSECTIONPRECISIONTYPE, "Intersection Precision Level", "Bounding Box=1,Face=2");

    pm->RegisterNewEnum(CKPGUID_RECTBOXMODE, "Rectangle Mode", "Corners=1,Minimum Position/Sizes=2,Center/HalfSizes=3");
    param_type = pm->GetParameterTypeDescription(CKPGUID_RECTBOXMODE);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    // Settings Flags for proximity
    pm->RegisterNewFlags(CKPGUID_PROXIMITY, "Proximity", "In Range=1,Out Range=2,Enter Range=4,Exit Range=8");

    // This parameter will only be used in the behavior so we hide it
    // so that it does not appear in the available parameters in the schematic window
    param_type = pm->GetParameterTypeDescription(CKPGUID_PROXIMITY);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    InitializeModifyComponent(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(CKPGUID_INTERSECTIONPRECISIONTYPE);
    pm->UnRegisterParameterType(CKPGUID_RECTBOXMODE);
    pm->UnRegisterParameterType(CKPGUID_PROXIMITY);

    DestroyModifyComponent(context);

    return CK_OK;
}

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Logics/Array
    RegisterBehavior(reg, FillBehaviorAddArrayDecl);
    RegisterBehavior(reg, FillBehaviorArrayChangeValueIfDecl);
    RegisterBehavior(reg, FillBehaviorArrayClearDecl);
    RegisterBehavior(reg, FillBehaviorArrayProductDecl);
    RegisterBehavior(reg, FillBehaviorArraySumDecl);
    RegisterBehavior(reg, FillBehaviorArrayColumnTransformDecl);
    RegisterBehavior(reg, FillBehaviorArrayColumnsOperateDecl);
    RegisterBehavior(reg, FillBehaviorCreateGroupFromArrayDecl);
    RegisterBehavior(reg, FillBehaviorGetArrayDecl);
    RegisterBehavior(reg, FillBehaviorArraySetElementDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetElementDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetHighestDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetLowestDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetNearestDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetLineDecl);
    RegisterBehavior(reg, FillBehaviorArrayGetColumnTitleDecl);
    RegisterBehavior(reg, FillBehaviorArrayIteratorDecl);
    RegisterBehavior(reg, FillBehaviorArrayIteratorIfDecl);
    RegisterBehavior(reg, FillBehaviorArrayLoadDecl);
    RegisterBehavior(reg, FillBehaviorRemoveArrayDecl);
    RegisterBehavior(reg, FillBehaviorArrayRemoveLineDecl);
    RegisterBehavior(reg, FillBehaviorArrayRemoveIfDecl);
    RegisterBehavior(reg, FillBehaviorArrayReverseDecl);
    RegisterBehavior(reg, FillBehaviorArraySearchDecl);
    RegisterBehavior(reg, FillBehaviorArrayTestRowDecl);
    RegisterBehavior(reg, FillBehaviorArraySetDecl);
    RegisterBehavior(reg, FillBehaviorArraySetLineDecl);
    RegisterBehavior(reg, FillBehaviorArrayShuffleDecl);
    RegisterBehavior(reg, FillBehaviorArraySortDecl);
    RegisterBehavior(reg, FillBehaviorArraySwapDecl);
    RegisterBehavior(reg, FillBehaviorArrayUniqueDecl);
    RegisterBehavior(reg, FillBehaviorArrayValueCountDecl);
    RegisterBehavior(reg, FillBehaviorArrayWriteDecl);
    RegisterBehavior(reg, FillBehaviorArrayRemoveColumnDecl);
    RegisterBehavior(reg, FillBehaviorArrayInsertColumnDecl);
    RegisterBehavior(reg, FillBehaviorArrayMoveDecl);
    RegisterBehavior(reg, FillBehaviorArrayMoveColumnDecl);

    // Logics/Attribute
    RegisterBehavior(reg, FillBehaviorHasAttributeDecl);
    RegisterBehavior(reg, FillBehaviorObjectsWithAttributeIteratorDecl);
    RegisterBehavior(reg, FillBehaviorRemoveAttributeDecl);
    RegisterBehavior(reg, FillBehaviorSetAttributeDecl);

    RegisterBehavior(reg, FillBehaviorHasAttribute_oldDecl);
    RegisterBehavior(reg, FillBehaviorRemoveAttribute_oldDecl);
    RegisterBehavior(reg, FillBehaviorSetAttribute_oldDecl);

    // Logics/Strings
    RegisterBehavior(reg, FillBehaviorCreateStringDecl);
    RegisterBehavior(reg, FillBehaviorSplitStringDecl);
    RegisterBehavior(reg, FillBehaviorCutStringDecl);
    RegisterBehavior(reg, FillBehaviorLoadStringDecl);
    RegisterBehavior(reg, FillBehaviorReverseStringDecl);

    // Logics/Calculator
    RegisterBehavior(reg, FillBehaviorModifyComponentDecl);
    RegisterBehavior(reg, FillBehaviorCalculatorDecl);
    RegisterBehavior(reg, FillBehaviorGetComponentDecl);
    RegisterBehavior(reg, FillBehaviorGetDeltaTimeDecl);
    RegisterBehavior(reg, FillBehaviorIdentityDecl);
    RegisterBehavior(reg, FillBehaviorMiniCalculatorDecl);
    RegisterBehavior(reg, FillBehaviorDoOperaDecl);
    RegisterBehavior(reg, FillBehaviorPerSecondDecl);
    RegisterBehavior(reg, FillBehaviorRandomDecl);
    RegisterBehavior(reg, FillBehaviorSetComponentDecl);
    RegisterBehavior(reg, FillBehaviorThresholdDecl);
    RegisterBehavior(reg, FillBehaviorVariationDecl);
    RegisterBehavior(reg, FillBehaviorBezierTransformDecl);

    // Logics/Groups
    RegisterBehavior(reg, FillBehaviorAddObjectToGroupDecl);
    RegisterBehavior(reg, FillBehaviorAddToGroupDecl);
    RegisterBehavior(reg, FillBehaviorFillGroupByTypeDecl);
    RegisterBehavior(reg, FillBehaviorGroupOperatorDecl);
    RegisterBehavior(reg, FillBehaviorGroupClearDecl);
    RegisterBehavior(reg, FillBehaviorGetNearestInGroupDecl);
    RegisterBehavior(reg, FillBehaviorGroupIteratorDecl);
    RegisterBehavior(reg, FillBehaviorIsInGroupDecl);
    RegisterBehavior(reg, FillBehaviorRemoveFromGroupDecl);
    RegisterBehavior(reg, FillBehaviorRemoveObjectFromGroupDecl);

    // Logics/Interpolator
    RegisterBehavior(reg, FillBehaviorBezierInterpolatorDecl);
    RegisterBehavior(reg, FillBehaviorColorHSVInterpolatorDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorColorDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorFloatDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorIntDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorMatrixDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorOrientationDecl);
    RegisterBehavior(reg, FillBehaviorInterpolatorVectorDecl);

    RegisterBehavior(reg, FillBehaviorInterpolatorDecl); // Generic Interpolator

    RegisterBehavior(reg, FillBehaviorTimeBezierInterpolatorDecl);

    // Logics/Loops
    RegisterBehavior(reg, FillBehaviorBezierProgressionDecl);
    RegisterBehavior(reg, FillBehaviorCounterDecl);
    RegisterBehavior(reg, FillBehaviorIBCQDecl);
    RegisterBehavior(reg, FillBehaviorLinearProgressionDecl);
    RegisterBehavior(reg, FillBehaviorNoiseProgressionDecl);
    RegisterBehavior(reg, FillBehaviorTimeTimerDecl);
    RegisterBehavior(reg, FillBehaviorTimerMiniDecl);
    RegisterBehavior(reg, FillBehaviorWhileDecl);
    RegisterBehavior(reg, FillBehaviorHierarchyParserDecl);
    RegisterBehavior(reg, FillBehaviorChronoDecl);
    RegisterBehavior(reg, FillBehaviorCollectionIteratorDecl);

    // Logics/Message
    RegisterBehavior(reg, FillBehaviorBroadcastMessageDecl);
    RegisterBehavior(reg, FillBehaviorCheckForMessageDecl);
    RegisterBehavior(reg, FillBehaviorGetDataMessageDecl);
    RegisterBehavior(reg, FillBehaviorSendMessageDecl);
    RegisterBehavior(reg, FillBehaviorSendMessageToGroupDecl);
    RegisterBehavior(reg, FillBehaviorSwitchOnMessageDecl);
    RegisterBehavior(reg, FillBehaviorWaitMessageDecl);

    // Logics/Streaming
    RegisterBehavior(reg, FillBehaviorAllButOneDecl);
    RegisterBehavior(reg, FillBehaviorBinaryMemoryDecl);
    RegisterBehavior(reg, FillBehaviorBinarySwitchDecl);
    RegisterBehavior(reg, FillBehaviorBoolEventDecl);
    RegisterBehavior(reg, FillBehaviorFIFODecl);
    RegisterBehavior(reg, FillBehaviorLIFODecl);
    RegisterBehavior(reg, FillBehaviorOneAtATimeDecl);
    RegisterBehavior(reg, FillBehaviorParameterSelectorDecl);
    RegisterBehavior(reg, FillBehaviorPriorityDecl);
    RegisterBehavior(reg, FillBehaviorRandomSwitchDecl);
    RegisterBehavior(reg, FillBehaviorSequencerDecl);
    RegisterBehavior(reg, FillBehaviorSpecificBoolEventDecl);
    RegisterBehavior(reg, FillBehaviorStreamingNOTDecl);
    RegisterBehavior(reg, FillBehaviorStreamingEventDecl);
    RegisterBehavior(reg, FillBehaviorSwitchOnParameterDecl);
    RegisterBehavior(reg, FillBehaviorKeepActivateDecl);
    RegisterBehavior(reg, FillBehaviorNopDecl);
    RegisterBehavior(reg, FillBehaviorTriggerEventDecl);

    // Logics/Synchro
    RegisterBehavior(reg, FillBehaviorEnterCriticalSectionDecl);
    RegisterBehavior(reg, FillBehaviorLeaveCriticalSectionDecl);
    RegisterBehavior(reg, FillBehaviorRendezVousDecl);
    RegisterBehavior(reg, FillBehaviorWaitForAllDecl);

    // Logics/Test
    RegisterBehavior(reg, FillBehaviorIsInViewFrustrumDecl);
    RegisterBehavior(reg, FillBehaviorObjectBetweenDecl);
    RegisterBehavior(reg, FillBehaviorProximityDecl);
    RegisterBehavior(reg, FillBehaviorRayIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorRayBoxIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorTestDecl);
}
