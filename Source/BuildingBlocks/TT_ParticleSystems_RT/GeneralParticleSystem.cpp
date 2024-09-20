/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GeneralParticleSystem
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "GeneralParticleSystem.h"

#define INACTIVE 0
#define ACTIVE 1
#define FREEZED 2

CKERROR CreateGeneralParticleSystemProto(CKBehaviorPrototype **);
int GeneralParticleSystem(const CKBehaviorContext &behcontext);
CKERROR GeneralParticleSystemCallback(const CKBehaviorContext &behcontext);
void EmitterSetMesh(CKBOOL Set, CKGUID guid, CKBehavior *beh, ParticleEmitter *em);

CKERROR CreateGeneralParticleSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype(ParticleSystemsName);
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Freeze");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Exit Freeze");

    proto->DeclareInParameter("Emission Delay", CKPGUID_TIME, "0m 0s 200ms");
    proto->DeclareInParameter("Emission Delay Variance", CKPGUID_TIME, "0m 0s 0ms");
    proto->DeclareInParameter("Yaw Variance", CKPGUID_ANGLE, "0:20");
    proto->DeclareInParameter("Pitch Variance", CKPGUID_ANGLE, "0:20");
    proto->DeclareInParameter("Speed", CKPGUID_FLOAT, "0.005");
    proto->DeclareInParameter("Speed Variance", CKPGUID_FLOAT, "0.001");
    proto->DeclareInParameter("Angular Speed/Spreading", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Angular Speed Variance/Spreading Variation", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Lifespan", CKPGUID_TIME, "0m 1s 0ms");
    proto->DeclareInParameter("Lifespan Variance", CKPGUID_TIME, "0m 0s 250ms");
    proto->DeclareInParameter("Maximum Number", CKPGUID_INT, "100");
    proto->DeclareInParameter("Emission", CKPGUID_INT, "10");
    proto->DeclareInParameter("Emission Variance", CKPGUID_INT, "5");
    proto->DeclareInParameter("Initial Size", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Initial Size Variance", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Ending Size", CKPGUID_FLOAT, "0.1");
    proto->DeclareInParameter("Ending Size Variance", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Bounce", CKPGUID_FLOAT, "0.8");
    proto->DeclareInParameter("Bounce Variance", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Weight", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Weight Variance", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Surface", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Surface Variance", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Initial Color and Alpha", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Variance", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Ending Color and Alpha", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Variance", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Initial Texture Frame", CKPGUID_INT, "0");
    proto->DeclareInParameter("Initial Texture Frame Variance", CKPGUID_INT, "0");
    proto->DeclareInParameter("Texture Speed", CKPGUID_INT, "100");
    proto->DeclareInParameter("Texture Speed Variance", CKPGUID_INT, "20");
    proto->DeclareInParameter("Texture Frame Count", CKPGUID_INT, "1");
    proto->DeclareInParameter("Texture Loop", CKPGUID_LOOPMODE, "No Loop");
    proto->DeclareInParameter("Real-Time Mode", CKPGUID_BOOL, "true");
    proto->DeclareInParameter("DeltaTime", CKPGUID_FLOAT, "20.0");

    proto->DeclareLocalParameter("Emitter", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("Activity", CKPGUID_INT);
    proto->DeclareLocalParameter("EmissionTime", CKPGUID_FLOAT, "0");

    // Settings
    proto->DeclareSetting("Maximum Number", CKPGUID_INT, "100");
    proto->DeclareSetting("Particle Rendering", CKPGUID_RENDERMODES, "3");
    proto->DeclareSetting("Source Blend", CKPGUID_BLENDFACTOR, "Source Alpha");
    proto->DeclareSetting("Destination Blend", CKPGUID_BLENDFACTOR, "One");
    proto->DeclareSetting("Objects", CKPGUID_GROUP);
    proto->DeclareSetting("Evolutions", CKPGUID_EVOLUTIONS, "Color,Size,Texture");
    proto->DeclareSetting("Variances", CKPGUID_VARIANCES, "Speed,Angular Speed,Lifespan,Emission,Initial Size,Ending Size,Bounce,Weight,Surface,Initial Color,Ending Color,Initial Texture,Texture Speed");
    proto->DeclareSetting("Manage Deflectors", CKPGUID_DEFLECTORS, "Plane,Infinite Plane,Cylinder,Sphere,Box,Object");
    proto->DeclareSetting("Message To Deflectors", CKPGUID_MESSAGE, "NULL");
    proto->DeclareSetting("Manage Interactors", CKPGUID_INTERACTORS, "Gravity,Global Wind,Local Wind,Magnet,Vortex,Disruption Box,Mutation Box,Atmosphere,Tunnel,Projector");
    proto->DeclareSetting("Interactors/Deflectors Display", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GeneralParticleSystem);
    proto->SetBehaviorCallbackFct(GeneralParticleSystemCallback);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));

    *pproto = proto;
    return CK_OK;
}

void ShowParticles(CKBehavior *beh, CKBOOL show)
{
    CKGUID guid = beh->GetPrototypeGuid();

    // We get the emitter
    ParticleEmitter *pe = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
    if (!pe) return;

    // we now set the good mesh of the emitter
    if (show)
        EmitterSetMesh(FALSE, guid, beh, pe);

    CK3dEntity *entity = (CK3dEntity *)beh->GetCKObject(pe->m_Entity);
    if (!entity)
        return;

    entity->RemovePostRenderCallBack(RenderParticles_P, pe);
    entity->RemovePostRenderCallBack(RenderParticles_L, pe);
    entity->RemovePostRenderCallBack(RenderParticles_S, pe);
    entity->RemovePostRenderCallBack(RenderParticles_O, pe);
    entity->RemovePostRenderCallBack(RenderParticles_FS, pe);
    entity->RemovePostRenderCallBack(RenderParticles_OS, pe);
    entity->RemovePostRenderCallBack(RenderParticles_CS, pe);
    entity->RemovePostRenderCallBack(RenderParticles_RS, pe);
    if (show)
    {
        entity->AddPostRenderCallBack(pe->m_RenderParticlesCallback, pe);
    }
}

int GeneralParticleSystem(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKGUID guid = beh->GetPrototypeGuid();

    // We get the emitter
    ParticleEmitter *pe = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
    if (!pe)
        return CKBR_PARAMETERERROR;

    CK3dEntity *entity = (CK3dEntity *)ctx->GetObject(pe->m_Entity);

    // We get the activity
    int activity = 0;
    beh->GetLocalParameterValue(1, &activity);

    CKBOOL realTimeMode = 0;
    beh->GetInputParameterValue(REALTIMEMODE, &realTimeMode);

    float deltaTime = 0.0f;
    beh->GetInputParameterValue(DELTATIME, &deltaTime);
    if (realTimeMode)
        deltaTime = behcontext.DeltaTime;

    if (beh->IsInputActive(3))
    {
        beh->ActivateInput(3, FALSE);
    }
    else
    {
        // Froze Input
        if (beh->IsInputActive(2))
        {
            beh->ActivateInput(2, FALSE);
            beh->ActivateOutput(2);
            // it was frozen, we unfreeze it
            if (activity & FREEZED)
            {
                activity &= ~FREEZED;
                pe->m_Active = FALSE;
            }
            else
            {
                activity |= FREEZED;
                pe->m_Active = TRUE;
            }
        }
        else
        {
            // Activate input
            if (beh->IsInputActive(0))
            {
                beh->ActivateInput(0, FALSE);
                beh->ActivateOutput(0);

                // we write the emission time to 0
                float emissiontime = 0.0f;
                beh->GetInputParameterValue(EMISSIONDELAY, &emissiontime);
                // we init the time with the delay to have one particle emitted at the activation
                beh->SetLocalParameterValue(2, &emissiontime);

                pe->m_Active = TRUE;

                activity |= ACTIVE;
                ShowParticles(beh);
            }
            else
            {
                // Inactivate input
                if (beh->IsInputActive(1))
                {
                    beh->ActivateInput(1, FALSE);
                    activity = INACTIVE;
                    pe->m_Active = FALSE;
                }
            }
        }

        // we save the activity
        beh->SetLocalParameterValue(1, &activity);

        if (activity & FREEZED)
            return CKBR_OK;

        // Reading Inputs
        if (pe->m_IsTimePointEmitter)
            ((TimePointEmitter *)pe)->ReadInputs(beh);
        else
            pe->ReadInputs(beh);

        // shape of emitter object
        if (guid == OBJECTSYSTEM_GUID)
        {
            ((ObjectEmitter *)pe)->m_Shape = entity->GetCurrentMesh();
        }

        // we update the time spent since last emission
        float emissiontime = 0.0f;
        beh->GetLocalParameterValue(2, &emissiontime);
        emissiontime += deltaTime;

        pe->m_DeltaTime = deltaTime;

        // We create new particles only if we are active
        if (activity)
        {
            if (pe->m_IsWaveEmitter)
            {
                pe->AddParticles3();
            }
            else
            {
                float emissiondelay = 0;
                beh->GetInputParameterValue(EMISSIONDELAY, &emissiondelay);
                float emissiondelayvariance = 0;
                beh->GetInputParameterValue(EMISSIONDELAYVAR, &emissiondelayvariance);
                emissiondelay += emissiondelayvariance * RANDNUM;

                if (emissiondelay > 0) // linked with time
                {
                    while (emissiontime > emissiondelay)
                    {
                        emissiontime -= emissiondelay;
                        pe->AddParticles();
                    }
                }
                else // Not linked with time
                {
                    pe->AddParticles();
                }
            }
        }

        // We update the particles (position, color, size...)
        if (pe->m_IsWaveEmitter)
        {
            pe->UpdateParticles2(behcontext.DeltaTime);
        }
        else
        {
            pe->UpdateParticles(behcontext.DeltaTime);
        }

        // Saving Locals

        // we write the time
        beh->SetLocalParameterValue(2, &emissiontime);
    }

    // This code needs to be moved into thread function, however
    // it imposes a condition that the values are not ready until the next frame potentially
    // Deflector Impacts Management
    if (pe->m_CurrentImpact < pe->m_Impacts.Size())
    {
        beh->SetOutputParameterValue(0, &pe->m_Impacts[pe->m_CurrentImpact].m_Position);
        beh->SetOutputParameterValue(1, &pe->m_Impacts[pe->m_CurrentImpact].m_Direction);
        beh->SetOutputParameterObject(2, pe->m_Impacts[pe->m_CurrentImpact].m_Object);
        beh->SetOutputParameterValue(3, &pe->m_Impacts[pe->m_CurrentImpact].m_UVs);
        pe->m_CurrentImpact++;
        beh->ActivateOutput(3);
    }

    // This section of code can go into the render call back, and return premature
    // if particle is shutdown
    // if there is no more particles and the behavior is inactive
    if (!activity && !(pe->GetParticles()))
    {
        ShowParticles(beh, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR GeneralParticleSystemCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    // we get the guid
    CKGUID guid = beh->GetPrototypeGuid();

    // we get the frame entity
    CK3dEntity *ement = (CK3dEntity *)beh->GetOwner();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    {
        if ((guid != OBJECTSYSTEM_GUID) &&
            (guid != CURVESYSTEM_GUID))
        {
            // we test if it's a frame : if not -> reject
            if (!(ement->GetFlags() & CK_3DENTITY_FRAME))
            {
                ctx->OutputToConsole("You can only attach this particular system to a Frame");
                return CKBR_OWNERERROR;
            }
        }

        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);

        ///////////////////////////
        // Interactors Diplay
        ///////////////////////////
        beh->SetLocalParameterValue(DISPLAYINTERACTORS, &pm->m_ShowInteractors);
    }
    case CKM_BEHAVIORLOAD:
    {
        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);

        // init of the local param
        ParticleEmitter *em = pm->CreateNewEmitter(guid, CKOBJID(ement));

        beh->SetLocalParameterValue(0, &em, sizeof(ParticleEmitter *));

        // we now set the good mesh of the emitter
        if (!ctx->IsPlaying())
            EmitterSetMesh(TRUE, guid, beh, em);

        /////////////////////////////////////
        // CALLBACKS
        /////////////////////////////////////

        // We read the settings
        em->ReadSettings(beh);

        // We read the inputs
        if (guid == TIMEPOINTSYSTEM_GUID)
            ((TimePointEmitter*)em)->ReadInputs(beh);
        else
            em->ReadInputs(beh);

        // we initialize the time
        float time = 0.0f;
        beh->SetLocalParameterValue(2, &time);
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        ParticleEmitter *pe = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!pe)
            return CKBR_PARAMETERERROR;

        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);
        if (pm)
            pm->InteractorsSetRemoveMesh(TRUE);

        ement->ModifyMoveableFlags(VX_MOVEABLE_RENDERLAST, 0);
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        ParticleEmitter *pe = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        pe->ReadSettings(beh);

        if (ctx->IsPlaying() && beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
            ShowParticles(beh);
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        // we check if it's not the cancel button
        if (em)
        {
            // we now remove the emitter mesh of the frame
            EmitterSetMesh(FALSE, guid, beh, em);

            // We remove the render callback
            if (ement)
            {
                ement->RemovePostRenderCallBack(em->m_RenderParticlesCallback, em);
            }

            // we delete the particle system
            ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);
            pm->DeleteEmitter(em);

            beh->SetLocalParameterObject(0, NULL);
        }
    }
    break;
    case CKM_BEHAVIORPOSTSAVE:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!em) return CKBR_PARAMETERERROR;

        // we now set the good mesh of the emitter
        EmitterSetMesh(TRUE, guid, beh, em);

        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);
        if (pm)
            pm->InteractorsSetRemoveMesh(TRUE);
    }
    break;
    case CKM_BEHAVIORRESET:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!em) return CKBR_PARAMETERERROR;

        em->InitParticleSystem();

        // we now set the good mesh of the emitter
        EmitterSetMesh(TRUE, guid, beh, em);
    }
    break;
    case CKM_BEHAVIORPAUSE:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!em)
            return CKBR_PARAMETERERROR;

        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);
        if (pm)
            pm->InteractorsSetRemoveMesh(TRUE);

        // we now set the good mesh of the emitter
        EmitterSetMesh(TRUE, guid, beh, em);

        ShowParticles(beh, FALSE);
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        ShowParticles(beh, FALSE);
    }
    break;
    case CKM_BEHAVIORPRESAVE:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!em)
            return CKBR_PARAMETERERROR;

        // we now set the good mesh of the emitter
        EmitterSetMesh(FALSE, guid, beh, em);

        ParticleManager *pm = (ParticleManager *)ctx->GetManagerByGuid(PARTICLE_MANAGER_GUID);
        if (pm)
            pm->InteractorsSetRemoveMesh(FALSE);
    }
    break;
    case CKM_BEHAVIORRESUME:
    {
        ParticleEmitter *em = *(ParticleEmitter **)beh->GetLocalParameterReadDataPtr(0);
        if (!em)
            return CKBR_PARAMETERERROR;

        // we now remove the emitter mesh of the frame
        EmitterSetMesh(FALSE, guid, beh, em);
    }
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        if (beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
            ShowParticles(beh);
    }
    break;
    }
    return CKBR_OK;
}

void EmitterSetMesh(CKBOOL Set, CKGUID guid, CKBehavior *beh, ParticleEmitter *em)
{
    CKContext *ctx = beh->GetCKContext();
    // we get the frame entity
    CK3dEntity *ement = (CK3dEntity *)beh->GetOwner();
    if (!ement)
        return;

    // If a mesh, it's not an object or a curve PS
    if (em->m_Mesh)
    {
        if (Set)
        {
            ement->SetCurrentMesh((CKMesh *)ctx->GetObject(em->m_Mesh));
        }
        else
        {
            ement->RemoveMesh((CKMesh *)ctx->GetObject(em->m_Mesh));
        }
    }
}