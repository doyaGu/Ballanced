#ifndef PARTICLETOOLS_H
#define PARTICLETOOLS_H

#include "CKAll.h"

#include <time.h>

/// Global Definitions ////////////////////////////////////////////////////////
// const int HALF_RAND = (RAND_MAX / 2);
// const float INVHALF_RAND = 1.0f/HALF_RAND;

#define HALF_RAND (RAND_MAX / 2.0f)
#define INVHALF_RAND 1.0f / (HALF_RAND)
#define INV_RAND 1.0f / (RAND_MAX)

#define RANDNUM ((float)(rand() - HALF_RAND) * INVHALF_RAND)
#define RANDNUMP ((float)rand() * INV_RAND)

// float RandomNum();
CKBOOL RayBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
CKBOOL RayInteriorBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
CKBOOL RaySphereIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
CKBOOL RayPlaneIntersection(VxVector &Normal, VxVector &pt, VxVector &ori, VxVector &dest, VxVector &res);
CKBOOL RayTubeIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);

// //
// //
#endif
