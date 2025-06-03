/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	            General Camera Orbit
//
//	Note : Defines some general functions for the
//		   Camera Orbit Behaviors.
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#ifndef GENERALCAMERAORBIT_H
#define GENERALCAMERAORBIT_H

// Include the CK library

// Defines a new type
typedef void (*INPUTPROCESSFUNCTION)(VxVector *RotationAngles, CKBehavior *beh, CKInputManager *input, float delta, CKBOOL &Returns, CKBOOL &Stopping);

// Infinite Value for reference
#define INF     12345789
#define STEP    0.0001f

// General Input Parameters
#define IN_TARGET_POS       0
#define IN_TARGET_REF       1
#define IN_SPEED_MOVE       2
#define IN_SPEED_RETURN     3
#define IN_MIN_H            4
#define IN_MAX_H            5
#define IN_MIN_V            6
#define IN_MAX_V            7
#define IN_SPEED_ZOOM       8
#define IN_MIN_ZOOM         9
#define IN_MAX_ZOOM         10

// General Local Parameters and Settings
#define LOCAL_INIT          0
#define LOCAL_ROT           1
#define LOCAL_STOPPING      2
#define LOCAL_LIMITS        3
#define LOCAL_RETURN        4

// Joystick Additional Settings
#define LOCAL_JOY_NB        5
#define LOCAL_JOY_ZIN       6
#define LOCAL_JOY_ZOUT      7
#define LOCAL_JOY_INVERSE   8
#define LOCAL_JOY_THRESHOLD 9

// Mouse

// Keyboard Additional Settings
#define LOCAL_KEY_LEFT      5
#define LOCAL_KEY_RIGHT     6
#define LOCAL_KEY_UP        7
#define LOCAL_KEY_DOWN      8
#define LOCAL_KEY_ZIN       9
#define LOCAL_KEY_ZOUT      10

// Generic Additionnal Inputs
#define INPUT_LEFT          2
#define INPUT_RIGHT         3
#define INPUT_UP            4
#define INPUT_DOWN          5
#define INPUT_ZOOMIN        6
#define INPUT_ZOOMOUT       7
#define LOCAL_RETURNING     5

// Defines the functions of this file.
CKERROR FillGeneralCameraOrbitProto(CKBehaviorPrototype *proto);
int GeneralCameraOrbit(const CKBehaviorContext &behcontext, INPUTPROCESSFUNCTION InputFunction);
CKERROR GeneralCameraOrbitCallback(const CKBehaviorContext &behcontext);

#endif
