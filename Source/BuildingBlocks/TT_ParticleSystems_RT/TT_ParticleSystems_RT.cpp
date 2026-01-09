#include "CKAll.h"

#include "GeneralParticleSystem.h"
#include "ParticleGuids.h"

#ifdef CK_LIB
#define RegisterBehaviorDeclarations    Register_TT_ParticleSystems_BehaviorDeclarations
#define InitInstance                    _TT_ParticleSystems_InitInstance
#define ExitInstance                    _TT_ParticleSystems_ExitInstance
#define CKGetPluginInfoCount            CKGet_TT_ParticleSystems_PluginInfoCount
#define CKGetPluginInfo                 CKGet_TT_ParticleSystems_PluginInfo
#define g_PluginInfo                    g_TT_ParticleSystems_PluginInfo
#else
#define RegisterBehaviorDeclarations    RegisterBehaviorDeclarations
#define InitInstance                    InitInstance
#define ExitInstance                    ExitInstance
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

CKERROR InitInstance(CKContext *context)
{
    // Manager
    new ParticleManager(context);

    CKParameterManager *pm = context->GetParameterManager();

    // we register all the enums related to the particle system
    pm->RegisterNewEnum(CKPGUID_RENDERMODES, "Particle Rendering", "Point=1,Line=2,Sprite=3,Object=4,Orientable Sprite=5,Radial Sprite=6,Fast Sprite=7,Comet=8");
    // Loop mode
    pm->RegisterNewEnum(CKPGUID_LOOPMODE, "Loop Mode", "No Loop=0,Loop=1,To and Fro=2");

    // Evolution Flags
    pm->RegisterNewFlags(CKPGUID_EVOLUTIONS, "Evolutions", "Size=1,Color=2,Texture=4");
    // Variance Flags
    pm->RegisterNewFlags(CKPGUID_VARIANCES, "Variances", "Speed=1,Angular Speed=2,Lifespan=4,Emission=8,Initial Size=16,Ending Size=32,Bounce=64,Weight=128,Surface=256,Initial Color=512,Ending Color=1024,Initial Texture=2048,Texture Speed=4096");
    // Interactors Managing Flags
    pm->RegisterNewFlags(CKPGUID_INTERACTORS, "Particle Interactors Managing", "Gravity=1,Global Wind=2,Local Wind=4,Magnet=8,Vortex=16,Disruption Box=32,Atmosphere=64,Mutation Box=128,Tunnel=256,Projector=512");
    // Deflectors Managing Flags
    pm->RegisterNewFlags(CKPGUID_DEFLECTORS, "Particle Deflectors Managing", "Plane=1,Infinite Plane=2,Cylinder=4,Sphere=8,Box=16,Object=32,Outputs Impacts=64,Die On Impact=128");

    // Interactors Structure
    pm->RegisterNewStructure(CKPGUID_PARTICLEMUTATION, "Mutation Elements", "Size,SizeSpeed,Color,ColorSpeed", CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_COLOR, CKPGUID_FLOAT);
    pm->RegisterNewStructure(CKPGUID_PARTICLETUNNEL, "Tunnel Values", "Velocity,Radius,Attraction Shape,Attraction Speed,Attraction Direction", CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_FLOAT);

    // Deflectors Structure
    pm->RegisterNewStructure(CKPGUID_PDEFLECTORS, "Deflectors", "Response,Friction,Density", CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_PERCENTAGE);
    pm->RegisterNewStructure(CKPGUID_PODEFLECTORS, "Deflectors", "Response,Friction,Density,Smoothed Normal", CKPGUID_FLOAT, CKPGUID_FLOAT, CKPGUID_PERCENTAGE, CKPGUID_BOOL);

    // We hide the parameters (structures and flags)
    CKParameterTypeDesc *param_type;
    param_type = pm->GetParameterTypeDescription(CKPGUID_RENDERMODES);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_EVOLUTIONS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_VARIANCES);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_INTERACTORS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_DEFLECTORS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_PARTICLEMUTATION);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_PARTICLETUNNEL);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_PDEFLECTORS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;
    param_type = pm->GetParameterTypeDescription(CKPGUID_PODEFLECTORS);
    if (param_type)
        param_type->dwFlags |= CKPARAMETERTYPE_HIDDEN;

    return CK_OK;
}

CKERROR ExitInstance(CKContext *context)
{
    // Manager
    ParticleManager *man = ParticleManager::GetManager(context);
    if (man)
        delete man;

    CKParameterManager *pm = context->GetParameterManager();

    pm->UnRegisterParameterType(CKPGUID_RENDERMODES);
    pm->UnRegisterParameterType(CKPGUID_LOOPMODE);
    pm->UnRegisterParameterType(CKPGUID_EVOLUTIONS);
    pm->UnRegisterParameterType(CKPGUID_VARIANCES);
    pm->UnRegisterParameterType(CKPGUID_INTERACTORS);
    pm->UnRegisterParameterType(CKPGUID_DEFLECTORS);
    pm->UnRegisterParameterType(CKPGUID_PARTICLEMUTATION);
    pm->UnRegisterParameterType(CKPGUID_PARTICLETUNNEL);
    pm->UnRegisterParameterType(CKPGUID_PDEFLECTORS);
    pm->UnRegisterParameterType(CKPGUID_PODEFLECTORS);

    return CK_OK;
}

#define PARTICLES_BEHAVIOR CKGUID(0x5E7B2D90, 0x40CC0AB4)

CKPluginInfo g_PluginInfo[2];

PLUGIN_EXPORT int CKGetPluginInfoCount() { return 2; }

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo[0].m_Author = "Virtools";
    g_PluginInfo[0].m_Description = "Particle Systems Building Blocks";
    g_PluginInfo[0].m_Extension = "";
    g_PluginInfo[0].m_Type = CKPLUGIN_BEHAVIOR_DLL;
    g_PluginInfo[0].m_Version = 0x000001;
    g_PluginInfo[0].m_InitInstanceFct = NULL;
    g_PluginInfo[0].m_ExitInstanceFct = NULL;
    g_PluginInfo[0].m_GUID = PARTICLES_BEHAVIOR;
    g_PluginInfo[0].m_Summary = "Particle Systems Building Blocks";

    g_PluginInfo[1].m_Author = "Virtools";
    g_PluginInfo[1].m_Description = "Particle Systems Manager";
    g_PluginInfo[1].m_Extension = "";
    g_PluginInfo[1].m_Type = CKPLUGIN_MANAGER_DLL;
    g_PluginInfo[1].m_Version = 0x000001;
    g_PluginInfo[1].m_InitInstanceFct = InitInstance;
    g_PluginInfo[1].m_ExitInstanceFct = ExitInstance;
    g_PluginInfo[1].m_GUID = PARTICLE_MANAGER_GUID;
    g_PluginInfo[1].m_Summary = "Particle Manager";

    return &g_PluginInfo[Index];
}

//	This function should be present and exported for Nemo
//	to be able to retrieve objects declarations.
//	Nemo will call this function at initialization
PLUGIN_EXPORT void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg);

void RegisterBehaviorDeclarations(XObjectDeclarationArray *reg)
{
    RegisterBehavior(reg, FillBehaviorPointParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorLinearParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorPlanarParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorCubicParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorDiscParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorObjectParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorCurveParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorCylindricalParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorSphericalParticleSystemDecl);

    // Added by Terratools
    RegisterBehavior(reg, FillBehaviorTimeDependentPointParticleSystemDecl);
    RegisterBehavior(reg, FillBehaviorWaveParticleSystemDecl);
}