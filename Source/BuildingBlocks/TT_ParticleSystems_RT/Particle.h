#ifndef PARTICLE_H
#define PARTICLE_H

#include "CK3dEntity.h"
#include "VxVector.h"
#include "Vx2dVector.h"
#include "VxColor.h"

// Impact position and direction
struct ParticleImpact
{
    VxVector m_Position;
    VxVector m_Direction;
    Vx2DVector m_UVs;
    CK3dEntity *m_Object;
};

class Particle
{
public:
    // Current Color
    VxColor m_Color;            // 0	---
    // Change of Color
    VxColor deltaColor;         // 16	---
    // Particle Position
    VxVector pos;               // 32	---
    // angle
    float m_Angle;              // 44
    // Particle direction
    VxVector dir;               // 48	---
    // angular speed delta
    float m_DeltaAngle;         // 60

    // Particle Lifespan
    float m_Life;               // 64
    // links to other particles
    Particle *prev;             // 68
    Particle *next;             // 72
    // last delta time
    float m_DeltaTime;          // 76
    // Current Size
    float m_Size;               // 80
    // Change of Size
    float m_DeltaSize;          // 84
    // bouncing factor
    float m_Bounce;             // 88
    // particle weight
    float m_Weight;             // 92
    // particle surface
    float m_Surface;            // 96

    union
    {
        // particle current frame
        int m_CurrentFrame;     // 100
        // group index (for object rendering)
        int m_GroupIndex;
    };

    // particle current frame time
    float m_CurrentFrametime;   // 104
    // particle delta frame time
    float m_DeltaFrametime;     // 108
    // particle delta frame time
    float m_Density;            // 112
    // Particle previous position
    VxVector prevpos;           // 116	---
}; // Size = 128 !!!!!

// GLOBALS
extern char *ParticleSystemsName;
extern char *ParticleSystemsInteractorsName;
extern char *ParticleSystemsDeflectorsName;

// INTERACTORS
extern char *ParticleGravityName;
extern char *ParticleGlobalWindName;
extern char *ParticleLocalWindName;
extern char *ParticleMagnetName;
extern char *ParticleVortexName;
extern char *ParticleDisruptionBoxName;
extern char *ParticleMutationBoxName;
extern char *ParticleAtmosphereName;
extern char *ParticleTunnelName;
extern char *ParticleProjectorName;

// DEFLECTORS
extern char *ParticleDPlaneName;
extern char *ParticleDInfinitePlaneName;
extern char *ParticleDCylinderName;
extern char *ParticleDSphereName;
extern char *ParticleDBoxName;
extern char *ParticleDObjectName;

#define PO_BOUNDINGBOX      0
#define PO_BOUNDINGSPHERE   1
#define PO_INFINITEPLANE    2
#define PO_OBJECT           3

// Particle Interactors Flags
#define PI_GRAVITY          0x00000001
#define PI_GLOBALWIND       0x00000002
#define PI_LOCALWIND        0x00000004
#define PI_MAGNET           0x00000008
#define PI_VORTEX           0x00000010
#define PI_DISRUPTIONBOX    0x00000020
#define PI_ATMOSPHERE       0x00000040
#define PI_MUTATIONBOX      0x00000080
#define PI_TUNNEL           0x00000100
#define PI_PROJECTOR        0x00000200

#define PI_ALL              0x000001ff

// Particle Deflectors Flags
#define PD_PLANE            0x00000001
#define PD_INFINITEPLANE    0x00000002
#define PD_CYLINDER         0x00000004
#define PD_SPHERE           0x00000008
#define PD_BOX              0x00000010
#define PD_OBJECT           0x00000020
#define PD_IMPACTS          0x00000040
#define PD_DIEONIMPACT      0x00000080

#define PD_ALL              0x0000003f

#define PD_BOUNCE           0
#define PD_FRICTION         1
#define PD_DENSITY          2

// Evolutions flags
#define PE_SIZE             0x00000001
#define PE_COLOR            0x00000002
#define PE_TEXTURE          0x00000004

// Variances flags
#define PV_SPEED            0x00000001
#define PV_ANGULARSPEED     0x00000002
#define PV_LIFESPAN         0x00000004
#define PV_EMISSION         0x00000008
#define PV_INITIALSIZE      0x00000010
#define PV_ENDINGSIZE       0x00000020
#define PV_BOUNCE           0x00000040
#define PV_WEIGHT           0x00000080
#define PV_SURFACE          0x00000100
#define PV_INITIALCOLOR     0x00000200
#define PV_ENDINGCOLOR      0x00000400
#define PV_INITIALTEXTURE   0x00000800
#define PV_SPEEDTEXTURE     0x00001000

// Render mode
#define PR_POINT            0x00000001
#define PR_LINE             0x00000002
#define PR_SPRITE           0x00000003
#define PR_OBJECT           0x00000004
#define PR_OSPRITE          0x00000005
#define PR_RSPRITE          0x00000006
#define PR_FSPRITE          0x00000007
#define PR_CSPRITE          0x00000008

// Loop mode
#define PL_NOLOOP           0x00000000
#define PL_LOOP             0x00000001
#define PL_PINGPONG         0x00000002

// Input Parameters Indices
#define EMISSIONDELAY       0
#define EMISSIONDELAYVAR    1
#define YAWVARIATION        2
#define PITCHVARIATION      3
#define SPEED               4
#define SPEEDVARIATION      5
#define ANGSPEED            6
#define ANGSPEEDVARIATION   7
#define LIFE                8
#define LIFEVARIATION       9
#define NUMBER              10
#define EMITION             11
#define EMITIONVARIATION    12
#define STARTSIZE           13
#define STARTSIZEVARIANCE   14
#define ENDSIZE             15
#define ENDSIZEVARIANCE     16
#define BOUNCE              17
#define BOUNCEVARIATION     18
#define WEIGHT              19
#define WEIGHTVARIATION     20
#define SURFACE             21
#define SURFACEVARIATION    22
#define STARTCOLOR          23
#define STARTCOLORVARIANCE  24
#define ENDCOLOR            25
#define ENDCOLORVARIANCE    26
#define TEXTURE             27
#define STARTTEXTURE        28
#define STARTTEXTUREVARIANCE 29
#define SPEEDTEXTURE        30
#define SPEEDTEXTUREVARIANCE 31
#define TEXTURECOUNT        32
#define TEXTURELOOP         33
#define REALTIMEMODE        34
#define DELTATIME           35

// Settings Indices
#define PARTICLENUMBER      3
#define RENDERMODES         4
#define SRCBLEND            5
#define DESTBLEND           6
#define OBJECTS             7
#define EVOLUTIONS          8
#define VARIANCES           9
#define MANAGEDEFLECTORS    10
#define MESSAGE             11
#define MANAGEINTERACTORS   12
#define DISPLAYINTERACTORS  13

// Bluured Layer Defines
#define BLURREDLAYERWIDTH   256
#define BLURREDLAYERHEIGHT  256

#endif
