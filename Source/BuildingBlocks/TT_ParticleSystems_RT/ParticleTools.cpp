#include "CKAll.h"

#include "ParticleTools.h"

CKBOOL RayBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm)
{
    CKBOOL inside = TRUE;
    int quadrant[3];
    int i;
    int whichPlane;
    VxVector maxT;
    VxVector candidatePlane;
    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perspective view)
    for (i = 0; i < 3; i++)
        if (p0.v[i] < box.Min.v[i])
        {
            quadrant[i] = 1;
            candidatePlane.v[i] = box.Min.v[i];
            norm.v[i] = -1.0f;
            inside = FALSE;
        }
        else if (p0.v[i] > box.Max.v[i])
        {
            quadrant[i] = 0;
            candidatePlane.v[i] = box.Max.v[i];
            norm.v[i] = 1.0f;
            inside = FALSE;
        }
        else
        {
            norm.v[i] = 0.0f;
            candidatePlane.v[i] = 0.0f;
            quadrant[i] = 2;
        }

    // Ray p0.v inside bounding box
    if (inside)
    {
        res = p0;
        return -1;
    }

    VxVector dir = p1 - p0;

    // Calculate T distances to candidate planes
    for (i = 0; i < 3; i++)
        if (quadrant[i] != 2 && dir[i] != 0.)
            maxT.v[i] = (candidatePlane.v[i] - p0.v[i]) / dir[i];
        else
            maxT.v[i] = -1.;

    // Get largest of the maxT.v's for final choice of intersection
    whichPlane = 0;
    for (i = 1; i < 3; i++)
        if (maxT.v[whichPlane] < maxT.v[i])
            whichPlane = i;

    // Check final candidate actually inside box
    if (maxT.v[whichPlane] < 0.)
        return (FALSE);
    if (maxT.v[whichPlane] > 1.)
        return (FALSE);
    for (i = 0; i < 3; i++)
        if (whichPlane != i)
        {
            norm.v[i] = 0.0f;
            res.v[i] = p0.v[i] + maxT.v[whichPlane] * dir[i];
            if (res.v[i] < box.Min.v[i] || res.v[i] > box.Max.v[i])
                return (FALSE);
        }
        else
        {
            res.v[i] = candidatePlane.v[i];
        }
    return 1;
}

CKBOOL RayInteriorBoxIntersection(const VxBbox &box, VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm)
{
    CKBOOL inside = TRUE;
    int quadrant[3];
    int i;
    int whichPlane;
    VxVector maxT;
    VxVector candidatePlane;
    // Find candidate planes; this loop can be avoided if
    // rays cast all from the eye(assume perpsective view)
    for (i = 0; i < 3; i++)
        if (p0.v[i] < box.Min.v[i])
        {
            quadrant[i] = 1;
            candidatePlane.v[i] = box.Min.v[i];
            norm.v[i] = -1.0f;
            inside = FALSE;
        }
        else if (p0.v[i] > box.Max.v[i])
        {
            quadrant[i] = 0;
            candidatePlane.v[i] = box.Max.v[i];
            norm.v[i] = 1.0f;
            inside = FALSE;
        }
        else
        {
            norm.v[i] = 0.0f;
            candidatePlane.v[i] = 0.0f;
            quadrant[i] = 2;
        }

    // Ray p0.v inside bounding box
    if (inside)
    {
        res = p0;
        return TRUE;
    }

    VxVector dir = p1 - p0;

    // Calculate T distances to candidate planes
    for (i = 0; i < 3; i++)
        if (quadrant[i] != 2 && dir[i] != 0.)
            maxT.v[i] = (candidatePlane.v[i] - p0.v[i]) / dir[i];
        else
            maxT.v[i] = -1.;

    // Get largest of the maxT.v's for final choice of intersection
    whichPlane = 0;
    for (i = 1; i < 3; i++)
        if (maxT.v[whichPlane] < maxT.v[i])
            whichPlane = i;

    // Check final candidate actually inside box
    if (maxT.v[whichPlane] < 0.)
        return (FALSE);
    if (maxT.v[whichPlane] > 1.)
        return (FALSE);
    for (i = 0; i < 3; i++)
        if (whichPlane != i)
        {
            norm.v[i] = 0.0f;
            res.v[i] = p0.v[i] + maxT.v[whichPlane] * dir[i];
            if (res.v[i] < box.Min.v[i] || res.v[i] > box.Max.v[i])
                return (FALSE);
        }
        else
        {
            res.v[i] = candidatePlane.v[i];
        }
    return (TRUE);
}

CKBOOL RaySphereIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm)
{
    float dist0 = SquareMagnitude(p0);
    float dist1 = SquareMagnitude(p1);
    if (((dist0 < 1) && (dist1 > 1)) || ((dist0 > 1) && dist1 < 1))
    { // in the tube limits radius
        VxVector V = Normalize(p1 - p0);
        float v;
        if (dist0 < 1)
        {
            v = DotProduct(p0, V);
        }
        else
        {
            v = DotProduct(-p0, V);
        }
        float disc = 1 - (DotProduct(p0, p0) - v * v);
        res = p0 + (v - sqrtf(disc)) * V;
        norm = res;
        return TRUE;
    }
    return FALSE;
}

CKBOOL RayTubeIntersection(VxVector &p0, VxVector &p1, VxVector &res, VxVector &norm)
{
    VxVector O(0, 0, 0);
    VxVector d(0, 0, 1);
    VxRay ray(O, d, NULL);
    float dist0 = ray.SquareDistance(p0);
    float dist1 = ray.SquareDistance(p1);
    if (((dist0 < 1) && (dist1 > 1)) || ((dist0 > 1) && dist1 < 1))
    { // in the tube limits radius
        if (((p1.z < 1) || (p0.z < 1)) && ((p1.z > -1) || (p0.z > -1)))
        { // in the tube height limit
            float a, b, c, delta, t;
            VxVector ray = p1 - p0;

            // the tube itself
            a = ray.v[0] * ray.v[0] + ray.v[1] * ray.v[1];
            b = 2 * (p0.v[0] * p0.v[0] + p0.v[1] * p0.v[1]);
            c = p0.v[0] * p0.v[0] + p0.v[1] * p0.v[1] - 2;

            delta = b * b - 4 * a * c;

            if (delta >= 0)
            {
                delta = (float)sqrt(delta);
                t = (-b - delta) / (2 * a);
                if (t < 0)
                {
                    t = (-b + delta) / (2 * a);
                    if (t < 0)
                    {
                        return FALSE;
                    }
                }
            }

            res = p0 + t * ray;
            norm = res;
            norm.z = 0;
            return TRUE;
        }
    }
    return FALSE;
}

CKBOOL RayPlaneIntersection(VxVector &Normal, VxVector &pt, VxVector &ori, VxVector &dest, VxVector &res)
{
    VxVector V = dest - ori;
    VxVector AP = ori - pt;

    float denom = DotProduct(V, Normal);
    if (denom == 0.0f)
        return FALSE;
    float t = -DotProduct(Normal, AP) / denom;
    if (t < 0)
        return FALSE;
    if (t > 1)
        return FALSE;
    res = ori + t * V;

    return TRUE;
}
