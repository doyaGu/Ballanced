#ifndef MOTOR_SETTINGS_H
#define MOTOR_SETTINGS_H

// Global motor sound settings, loaded by TT_LoadMotorSettings
// and used by TT_MotorSound

// Structure to hold motor sound settings for one channel
struct MotorChannelSettings
{
    float pitchMax;   // Maximum pitch
    float pitchMin;   // Minimum pitch
    float vMax;       // Maximum velocity for this channel
    float vMin;       // Minimum velocity for this channel
    float vFadeIn;    // Velocity at which gain starts fading in
    float vFadeOut;   // Velocity at which gain starts fading out
};

// Global settings structure
extern float g_MotorVolume;                   // Master volume
extern MotorChannelSettings g_MotorChannel1;  // Low speed channel
extern MotorChannelSettings g_MotorChannel2;  // Mid speed channel
extern MotorChannelSettings g_MotorChannel3;  // High speed channel
extern float g_MotorGlobalVolume;             // Current global volume multiplier

// Linear interpolation helper function
inline float MotorLerp(float value, float outMin, float outMax, float inMax, float inMin)
{
    if (inMax == inMin)
        return outMax;
    return (outMin - outMax) / (inMax - inMin) * (value - inMin) + outMax;
}

#endif
