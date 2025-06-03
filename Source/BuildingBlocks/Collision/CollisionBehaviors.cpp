//
// CollisionBehaviors.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_Collisions_BehaviorDeclarations
#define InitInstance                    _Collisions_InitInstance
#define ExitInstance                    _Collisions_ExitInstance
#define CKGetPluginInfoCount            CKGet_Collisions_PluginInfoCount
#define CKGetPluginInfo                 CKGet_Collisions_PluginInfo
#define g_PluginInfo                    g_Collisions_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

#include "FloorManager.h"
#include "CollisionManager.h"

#define COLLISION_BEHAVIOR CKGUID(0x7ee30a69, 0x6c475c56)

#define IMPACTFLAGS CKGUID(0x507f4ca4, 0x548e153c)
#define OBJECTEXTENTS CKGUID(0x193e0d51, 0x54625c8a)

CKPluginInfo g_PluginInfo;

// {secret}
const char *CollisionManagerName = "Collision Manager";
// {secret}
const char *FloorManagerName = "Floor Manager";

CKERROR CreateCollisionManager(CKContext *context)
{
    // we register all the enums related to collision manager
    CKParameterManager *pm = context->GetParameterManager();
    pm->RegisterNewEnum(CKPGUID_OBSTACLEPRECISION, "Geometry Precision", "BoundingBox=1,Faces=2");
    pm->RegisterNewEnum(CKPGUID_OBSTACLEPRECISIONBEH, "Geometry Precision (Behavioral)", "Automatic=0,BoundingBox=1,Faces=2");
    pm->RegisterNewStructure(CKPGUID_OBSTACLE, "Obstacle", "Obstacle Type,Use Hierarchy?", CKPGUID_OBSTACLEPRECISION, CKPGUID_BOOL);

    CKParameterTypeDesc *param_type;
    param_type = pm->GetParameterTypeDescription(CKPGUID_OBSTACLEPRECISION);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    new CollisionManager(context);
    return CK_OK;
}

CKERROR RemoveCollisionManager(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();
    pm->UnRegisterParameterType(CKPGUID_OBSTACLEPRECISION);
    pm->UnRegisterParameterType(CKPGUID_OBSTACLEPRECISIONBEH);
    pm->UnRegisterParameterType(CKPGUID_OBSTACLE);

    CollisionManager *man = (CollisionManager *)context->GetManagerByGuid(COLLISION_MANAGER_GUID);
    delete man;
    return CK_OK;
}

CKERROR CreateFloorManager(CKContext *context)
{
    new FloorManager(context);
    return CK_OK;
}

CKERROR RemoveFloorManager(CKContext *context)
{
    FloorManager *man = (FloorManager *)context->GetManagerByGuid(FLOOR_MANAGER_GUID);
    delete man;
    return CK_OK;
}

extern char *Network3dName;

CKERROR InitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    pm->RegisterNewFlags(IMPACTFLAGS, "Impact Data", "Touched Obstacle=1,Touched Sub Obstacle=2,Touched Face=4,Touching Face=8,Touched Vertex=16,Touching Vertex=32,Impact World Matrix=64,Impact Point=128,Impact Normal=256,Object Part=512");
    pm->RegisterNewEnum(OBJECTEXTENTS, "Object Extents", "Point=1,BoundingBox=2");

    // We hide the parameters (structures and flags)
    CKParameterTypeDesc *param_type;
    param_type = pm->GetParameterTypeDescription(IMPACTFLAGS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(OBJECTEXTENTS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    CKAttributeManager *attman = context->GetAttributeManager();
    int att = attman->RegisterNewAttributeType(Network3dName, CKPGUID_VOIDBUF, CKCID_GROUP, (CK_ATTRIBUT_FLAGS)(CK_ATTRIBUT_SYSTEM | CK_ATTRIBUT_HIDDEN));

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(IMPACTFLAGS);
    pm->UnRegisterParameterType(OBJECTEXTENTS);

    CKAttributeManager *attman = context->GetAttributeManager();
    attman->UnRegisterAttribute(Network3dName);

    return CK_OK;
}

PLUGIN_EXPORT int CKGetPluginInfoCount()
{
    return 3;
}
PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    switch (Index)
    {
    case 0:
        g_PluginInfo.m_Author = "Virtools";
        g_PluginInfo.m_Description = "Collision Manager";
        g_PluginInfo.m_Extension = "";
        g_PluginInfo.m_Type = CKPLUGIN_MANAGER_DLL;
        g_PluginInfo.m_Version = 0x000001;
        g_PluginInfo.m_InitInstanceFct = CreateCollisionManager;
        g_PluginInfo.m_ExitInstanceFct = RemoveCollisionManager;
        g_PluginInfo.m_GUID = COLLISION_MANAGER_GUID;
        g_PluginInfo.m_Summary = CollisionManagerName;
        break;
    case 1:
        g_PluginInfo.m_Author = "Virtools";
        g_PluginInfo.m_Description = "Floor Manager";
        g_PluginInfo.m_Extension = "";
        g_PluginInfo.m_Type = CKPLUGIN_MANAGER_DLL;
        g_PluginInfo.m_Version = 0x000001;
        g_PluginInfo.m_InitInstanceFct = CreateFloorManager;
        g_PluginInfo.m_ExitInstanceFct = RemoveFloorManager;
        g_PluginInfo.m_GUID = FLOOR_MANAGER_GUID;
        g_PluginInfo.m_Summary = FloorManagerName;
        break;
    case 2:
        g_PluginInfo.m_Author = "Virtools";
        g_PluginInfo.m_Description = "Collision Building Blocks";
        g_PluginInfo.m_Extension = "";
        g_PluginInfo.m_Type = CKPLUGIN_BEHAVIOR_DLL;
        g_PluginInfo.m_Version = 0x000001;
        g_PluginInfo.m_InitInstanceFct = InitInstance;
        g_PluginInfo.m_ExitInstanceFct = ExitInstance;
        g_PluginInfo.m_GUID = COLLISION_BEHAVIOR;
        g_PluginInfo.m_Summary = "Collision Building Blocks";
        break;
    }
    return &g_PluginInfo;
}

/**********************************************************************************/
/**********************************************************************************/
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    //////////////////////////////
    // COLLISIONS

    // Collisions/Obstacle
    RegisterBehavior(reg, FillBehaviorAddObstacleDecl);
    RegisterBehavior(reg, FillBehaviorDeclareObstaclesDecl);

    // Collisions/Character
    RegisterBehavior(reg, FillBehaviorCharacterPreventFromCollisionDecl);

    // Collisions/3D Entity
    RegisterBehavior(reg, FillBehaviorDetectCollisionDecl);
    RegisterBehavior(reg, FillBehaviorMultiDetectCollisionDecl);
    RegisterBehavior(reg, FillBehaviorObjectBallSliderDecl);
    RegisterBehavior(reg, FillBehaviorSimpleManageCollisionDecl);
    RegisterBehavior(reg, FillBehaviorTestBBYOverlappingDecl);

    // Collisions/Influence
    RegisterBehavior(reg, FillBehaviorAvoidingObstacles2Decl);
    RegisterBehavior(reg, FillBehaviorHomingMissileDecl);

    // Collisions/Intersection
    RegisterBehavior(reg, FillBehaviorBoundingSphereIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorBoxBoxIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorBoxFaceIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorFaceFaceIntersectionDecl);
    RegisterBehavior(reg, FillBehaviorFrustumObjectIntersectionDecl);

    //////////////////////////
    // FLOORS

    // Characters/Constraint
    RegisterBehavior(reg, FillBehaviorCharacterKeepOnFloorDecl);
    RegisterBehavior(reg, FillBehaviorCharacterKeepOnFloorV2Decl);
    RegisterBehavior(reg, FillBehaviorCharacterKeepOnFloorLimitsDecl);

    // 3D Transfo/Constraint
    RegisterBehavior(reg, FillBehaviorObjectKeepOnFloorDecl);
    RegisterBehavior(reg, FillBehaviorObjectKeepOnFloorV2Decl);

    // Collisions/Floors
    RegisterBehavior(reg, FillBehaviorDeclareFloorsDecl);
    RegisterBehavior(reg, FillBehaviorFloorManagerSetupDecl);
    RegisterBehavior(reg, FillBehaviorGetNearestFloorsDecl);
    RegisterBehavior(reg, FillBehaviorFloorSliderDecl);

    ////////////////////////////
    // NODAL PATH
    RegisterBehavior(reg, FillBehaviorActivateLinkDecl);
    RegisterBehavior(reg, FillBehaviorActivateNodeDecl);
    RegisterBehavior(reg, FillBehaviorCreateNetwork3dDecl);
    RegisterBehavior(reg, FillBehaviorEntityFindPathDecl);
    RegisterBehavior(reg, FillBehaviorFindCurvedPathDecl);
    RegisterBehavior(reg, FillBehaviorFindPathDecl);
    RegisterBehavior(reg, FillBehaviorGoToNodeDecl);
    RegisterBehavior(reg, FillBehaviorUpdateNodalPathDecl);
}
