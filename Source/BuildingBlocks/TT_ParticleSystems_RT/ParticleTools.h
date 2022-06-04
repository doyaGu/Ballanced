#ifndef BUILDINGBLOCKS_PARTICLETOOLS_H
#define BUILDINGBLOCKS_PARTICLETOOLS_H

#include "TT_ParticleSystems_RT.h"

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
BOOL RayBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
BOOL RayInteriorBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
BOOL RaySphereIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);
BOOL RayPlaneIntersection(VxVector &Normal, VxVector &pt, VxVector &ori, VxVector &dest, VxVector &res);
BOOL RayTubeIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm);

#endif // BUILDINGBLOCKS_PARTICLETOOLS_H
