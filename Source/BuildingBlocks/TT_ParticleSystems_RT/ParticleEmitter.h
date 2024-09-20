#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Particle.h"
#include "ParticleTools.h"

class ParticleManager;

class ParticleEmitter
{
public:
    // Constructor
    ParticleEmitter(CKContext *ctx, CK_ID entity, char *name);
    // Destructor
    ~ParticleEmitter();

    // initialize the pool
    void InitParticleSystem();
    // Updates the particles
    void UpdateParticles(float deltat);
    void UpdateParticles2(float deltat);
    // Add new particles
    void AddParticles();
    void AddParticles2();
    void AddParticles3();

    void SetState(CKRenderContext *dev, CKBOOL gouraud = FALSE);

    Particle *GetParticles() { return particles; }

    // the emitter 3d entity
    CK_ID m_Entity;
    CK_ID m_Texture;
    CK_ID m_Group;
    int m_MessageType;

    // render callback
    int (*m_RenderParticlesCallback)(CKRenderContext *dev, CKRenderObject *mov, void *arg);
    // read input function
    void ReadInputs(CKBehavior *beh);
    // read settings function
    void ReadSettings(CKBehavior *beh);

    // yaw variation
    float m_YawVariation;
    // pitch variation
    float m_PitchVariation;

    // medium speed
    float m_Speed;
    // speed variation
    float m_SpeedVariation;

    // angular speed
    float m_AngularSpeed;
    // angular speed variation
    float m_AngularSpeedVariation;

    // Sizes
    /////////////
    // start size
    float m_StartSize;
    float m_StartSizeVar;
    // end size
    float m_EndSize;
    float m_EndSizeVar;

    // medium weight
    float m_Weight;
    // weight variation
    float m_WeightVariation;

    // medium weight
    float m_Surface;
    // weight variation
    float m_SurfaceVariation;

    // bounce effect
    float m_Bounce;
    // bounce variation
    float m_BounceVariation;

    // NULL terminated linked list
    Particle *particles;
    // the particles pool
    CKBYTE *m_BackPool;
    Particle *m_Pool;
    // maximum number of particles
    int m_MaximumParticles;
    // total number of particles
    int totalParticles;
    // particles already emitted
    int particleCount;
    // emits per frame
    int emitsPerFrame;
    // emits variation
    int emitsVar;
    // medium lifeSpan
    float m_Life;
    // life variation
    float m_LifeVariation;

    // Blend Modes
    VXBLEND_MODE m_SrcBlend;
    VXBLEND_MODE m_DestBlend;

    // Colors
    /////////////
    VxColor m_StartColor;
    VxColor m_StartColorVar;
    VxColor m_EndColor;
    VxColor m_EndColorVar;

    // Texture
    /////////////
    int m_InitialTextureFrame;
    int m_InitialTextureFrameVariance;
    int m_SpeedTextureFrame;
    int m_SpeedTextureFrameVariance;
    int m_TextureFrameCount;
    int m_TextureFrameloop;

    // FLAGS
    int m_EvolutionsFlags;
    int m_VariancesFlags;
    int m_InteractorsFlags;
    int m_DeflectorsFlags;
    int m_RenderMode;

    // Mesh
    CK_ID m_Mesh;

    // Context
    CKContext *m_Context;

    // Impact Array
    XArray<ParticleImpact> m_Impacts;
    int m_CurrentImpact;

    static CKWORD *m_GlobalIndices;
    static int m_GlobalIndicesCount;

    CKBOOL m_Flag0x10C;
    CKBOOL m_IsTimePointEmitter;
    CKBOOL m_IsWaveEmitter;

    float m_DeltaTime;
    CKBOOL m_Active;

protected:
    // create the particle at its initial position : depends on the emitter type
    virtual void InitiateParticle(Particle *p) = 0;
    virtual void InitiateDirection(Particle *p);
};

void ShowParticles(CKBehavior *beh, CKBOOL show = TRUE);

#endif