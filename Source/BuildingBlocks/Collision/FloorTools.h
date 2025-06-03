#ifndef FLOOR_TOOLS_H
#define FLOOR_TOOLS_H

#include "CKAll.h"

int CrossedFaces(CK3dEntity* mov,VxVector& a,VxVector& b,int* faces,int facesmaxsize);
float RayExtrudedFacesIntersection(CK3dEntity* floor,VxVector& p1,VxVector& p2,int face,VxVector& intersection,VxVector& edge1,VxVector& edge2);

#endif // FLOOR_TOOLS_H