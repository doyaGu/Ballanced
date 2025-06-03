//
// 3DTrans.cpp : Defines the initialization routines for the DLL.
//
#include "CKAll.h"

#ifdef CK_LIB
    #define RegisterBehaviorDeclarations	Register_3DTransfo_BehaviorDeclarations
    #define InitInstance					_3DTransfo_InitInstance
    #define ExitInstance					_3DTransfo_ExitInstance
    #define CKGetPluginInfoCount			CKGet_3DTransfo_PluginInfoCount
    #define CKGetPluginInfo					CKGet_3DTransfo_PluginInfo
    #define g_PluginInfo					g_3DTransfo_PluginInfo
#else
    #define RegisterBehaviorDeclarations	RegisterBehaviorDeclarations
    #define InitInstance					InitInstance
    #define ExitInstance					ExitInstance
    #define CKGetPluginInfoCount			CKGetPluginInfoCount
    #define CKGetPluginInfo					CKGetPluginInfo
    #define g_PluginInfo					g_PluginInfo
#endif

#include "PortalsManager.h"

/**********************************************************************************/
/**********************************************************************************/
CKERROR InitInstance(CKContext *context)
{
    // Manager
    PortalsManager *manager = new PortalsManager(context);

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    // Manager
    PortalsManager *man = (PortalsManager *)context->GetManagerByGuid(PORTALS_MANAGER_GUID);
    delete man;

    return CK_OK;
}

#define TRANSFO_BEHAVIOR CKGUID(0x3c936933, 0x6a5b71c9)

CKPluginInfo g_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo[0].m_Author = "Virtools";
    g_PluginInfo[0].m_Description = "3D Transformation building blocks";
    g_PluginInfo[0].m_Extension = "";
    g_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo[0].m_Version = 0x000001;
    g_PluginInfo[0].m_InitInstanceFct = NULL;
    g_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_PluginInfo[0].m_GUID = TRANSFO_BEHAVIOR;
    g_PluginInfo[0].m_Summary = "3D Transformations";

    g_PluginInfo[1].m_Author = "Virtools";
    g_PluginInfo[1].m_Description = "Portals Manager";
    g_PluginInfo[1].m_Extension = "";
    g_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_PluginInfo[1].m_Version = 0x000001;
    g_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_PluginInfo[1].m_GUID = PORTALS_MANAGER_GUID;
    g_PluginInfo[1].m_Summary = "Portals Manager";

    return &g_PluginInfo[Index];
}

PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);
/**********************************************************************************/
/**********************************************************************************/
void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    // Basic
    RegisterBehavior(reg, FillBehaviorAddChildDecl);
    RegisterBehavior(reg, FillBehaviorAddScaleDecl);
    RegisterBehavior(reg, FillBehaviorGetEulerOrientationDecl);
    RegisterBehavior(reg, FillBehaviorRotateDecl);
    RegisterBehavior(reg, FillBehaviorRotateAroundDecl);
    RegisterBehavior(reg, FillBehaviorSetScaleDecl);
    RegisterBehavior(reg, FillBehaviorSetEulerOrientationDecl);
    RegisterBehavior(reg, FillBehaviorSetLocalMatrixDecl);
    RegisterBehavior(reg, FillBehaviorSetOrientationDecl);
    RegisterBehavior(reg, FillBehaviorSetParentDecl);
    RegisterBehavior(reg, FillBehaviorSetPositionDecl);
    RegisterBehavior(reg, FillBehaviorSetQuaternionOrientationDecl);
    RegisterBehavior(reg, FillBehaviorSetWorldMatrixDecl);
    RegisterBehavior(reg, FillBehaviorTranslateDecl);

    // Constraint
    RegisterBehavior(reg, FillBehaviorBillboardDecl);
    RegisterBehavior(reg, FillBehaviorKeepAtConstantDistanceDecl);
    RegisterBehavior(reg, FillBehaviorLookAtDecl);
    RegisterBehavior(reg, FillBehaviorLookAtPosDecl);
    RegisterBehavior(reg, FillBehaviorMimicDecl);

    // Movement
    RegisterBehavior(reg, FillBehaviorMoveToDecl);

    // Path
    RegisterBehavior(reg, FillBehaviorAddCurvePointDecl);
    RegisterBehavior(reg, FillBehaviorGetCurvePointPropertiesDecl);
    RegisterBehavior(reg, FillBehaviorGetCurvePropertiesDecl);
    RegisterBehavior(reg, FillBehaviorPathFollowDecl);
    RegisterBehavior(reg, FillBehaviorRemoveCurvePointDecl);
    RegisterBehavior(reg, FillBehaviorSetCurvePointPropertiesDecl);
    RegisterBehavior(reg, FillBehaviorSetCurvePropertiesDecl);
    RegisterBehavior(reg, FillBehaviorSetCurveStepDecl);

    // Animation
    RegisterBehavior(reg, FillBehaviorPlayAnimEntityDecl);
    RegisterBehavior(reg, FillBehaviorPlayGlobalAnimationDecl);
    RegisterBehavior(reg, FillBehaviorSetAnimationStepEntityDecl);
    RegisterBehavior(reg, FillBehaviorSetGlobalAnimationStepDecl);

    // Portals
    RegisterBehavior(reg, FillBehaviorPortalsBehaviorDecl);
    RegisterBehavior(reg, FillBehaviorSetPortalDecl);
}

/****************************************************************
 *   BB_Orientate
 *
 * Sets the orientation of a 3dEntity from
 *
 * a direction vector (must be normalized)
 * a dirMode (1=X, 2=-X, 3=Y, 4=-Y, 5=Z, 6=-Z)
 * a up vector (should be normalized, if up.x >> then get the entity up vector)
 * a roll angle
 * a speedValue (1=Instantaneous, 0.5=with attenuation)
 *
 * use bool unitaryScale if your sure about the unitary scaling
 * of the object
 *
 * return values: the up vector just before rolling (normalized)
 *
 * warning: the dir vector is supposed not to be null,
 * and nor the *ent ptr.
 ****************************************************************/
VxVector BB_Orientate(CK3dEntity *ent, VxVector dir, int dirMode, float roll, VxVector up, float speedValue, CKBOOL k, CKBOOL unitaryScale)
{

    VxVector scale;
    if (!unitaryScale)
        ent->GetScale(&scale);

    if ((dirMode & 1) == 0)
        dir = -dir;
    VxVector old_dir, right;

    //--- tend to be Y world axis
    up.x *= 0.5f;
    up.z *= 0.5f;

    //--- direction mode dependant
    switch (dirMode)
    {
    case 5: //  Z
    case 6: // -Z
    {
        if (up.x > 1.5f)
            ent->GetOrientation(NULL, &up); // memorize first up vector

        ent->GetOrientation(&old_dir, NULL);

        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(up, dir);
        up = CrossProduct(dir, right);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&dir, &up, &right, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(0, 0, 1, roll, ent, k);
    }
    break;

    case 1: //  X
    case 2: // -X
    {
        if (up.x > 1.5f)
            ent->GetOrientation(NULL, &up); // memorize first up vector

        ent->GetOrientation(NULL, NULL, &old_dir);

        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(dir, up);
        up = CrossProduct(right, dir);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&right, &up, &dir, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(1, 0, 0, roll, ent, k);
    }
    break;

    case 3: //  Y
    case 4: // -Y
    {
        if (up.x > 1.5f)
            ent->GetOrientation(&up, NULL); // memorize first up vector

        ent->GetOrientation(NULL, &old_dir);
        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(dir, up);
        up = CrossProduct(dir, right);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&right, &dir, &up, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(0, 1, 0, roll, ent, k);
    }
    break;
    }

    if (!unitaryScale)
        ent->SetScale(&scale);

    return up;
}
