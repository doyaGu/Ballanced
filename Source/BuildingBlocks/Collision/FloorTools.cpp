#include "FloorTools.h"

#include "CKAll.h"

/* Not working but may be the normal vector calculation that are false
CKBOOL SegmentIntersection2D(VxVector &lu, VxVector &A2, VxVector &mu, VxVector &B2, VxVector &P)
{
    VxVector ln, lv, mv;
    float lc;

    lv.x = A2.x - lu.x;
    lv.z = A2.z - lu.z;
    lv.Normalize();
    ln.x = lv.z;
    ln.z = lv.x;
    lc = ln.x * lu.z + ln.z * lu.x;
    mv.x = B2.x - mu.x;
    mv.z = B2.z - mu.z;
    mv.Normalize();

    float d = ln.x * mv.z + ln.z * mv.x;

    // line are parallel
    if (d == 0)
        return FALSE;

    float t = ((ln.x * mu.z + ln.z * mu.x) + lc) / d;

    if (t > 0)
        return FALSE;
    if (t < -1)
        return FALSE;

    P.x = mu.x - t * mv.x;
    P.z = mu.z - t * mv.z;

    return TRUE;
}
*/

int CrossedFaces(CK3dEntity *mov, VxVector &a, VxVector &b, int *faces, int facesmaxsize)
{
    CKMesh *mesh = mov->GetCurrentMesh();

    VxVector int1, int2, twup, tempres;

    mov->InverseTransform(&int1, &a);
    mov->InverseTransform(&int2, &b);

    mov->GetOrientation(NULL, &twup, NULL);

    // we first need to know the orientation
    int x;
    int z;

    VxVector planenormal;
    float smcp, min;

    // TODO : correct this..........
    // ASK roro what he did want to do...

    // Suppose first that X axis
    // Is aligned with world Up axis
    min = smcp = (float)fabs(twup.x);
    planenormal.Set(1.0f, 0.0f, 0.0f);
    x = 1;
    z = 2;
    // Is Y axis aligned with world Up axis
    smcp = (float)fabs(twup.y);
    if (smcp > min)
    {
        planenormal.Set(0.0f, 1.0f, 0.0f);
        min = smcp;
        x = 0;
        z = 2;
    }
    // Is Z axis aligned with world Up axis
    smcp = (float)fabs(twup.z);
    if (smcp > min)
    {
        planenormal.Set(0.0f, 0.0f, 1.0f);
        x = 0;
        z = 1;
    }

    float rayxmin, rayxmax, rayzmin, rayzmax;
    if (int1.v[x] < int2.v[x])
    {
        rayxmin = int1.v[x];
        rayxmax = int2.v[x];
    }
    else
    {
        rayxmin = int2.v[x];
        rayxmax = int1.v[x];
    }
    if (int1.v[z] < int2.v[z])
    {
        rayzmin = int1.v[z];
        rayzmax = int2.v[z];
    }
    else
    {
        rayzmin = int2.v[z];
        rayzmax = int1.v[z];
    }

    CKDWORD StridePos;
    CKBYTE *m_VertexArray = (CKBYTE *)mesh->GetPositionsPtr(&StridePos);

    CKWORD *FaceIndices = mesh->GetFacesIndices();
    int m_NbFaces = mesh->GetFaceCount();
    VxVector *pts[3];

    float xmin, xmax, zmin, zmax;

    int crossedfaces = 0;
    for (int i = 0; i < m_NbFaces; i++, FaceIndices += 3)
    {
        pts[0] = (VxVector *)&m_VertexArray[FaceIndices[0] * StridePos];
        pts[1] = (VxVector *)&m_VertexArray[FaceIndices[1] * StridePos];
        pts[2] = (VxVector *)&m_VertexArray[FaceIndices[2] * StridePos];

        // face rejection by Bbox 2D
        if (pts[0]->v[x] < pts[1]->v[x])
        {
            xmin = pts[0]->v[x];
            xmax = pts[1]->v[x];
        }
        else
        {
            xmin = pts[1]->v[x];
            xmax = pts[0]->v[x];
        }
        if (xmin > pts[2]->v[x])
        {
            xmin = pts[2]->v[x];
        }
        else
        {
            if (xmax < pts[2]->v[x])
            {
                xmax = pts[2]->v[x];
            }
        }
        if (rayxmax < xmin)
            continue;
        if (rayxmin > xmax)
            continue;

        if (pts[0]->v[z] < pts[1]->v[z])
        {
            zmin = pts[0]->v[z];
            zmax = pts[1]->v[z];
        }
        else
        {
            zmin = pts[1]->v[z];
            zmax = pts[0]->v[z];
        }
        if (zmin > pts[2]->v[z])
        {
            zmin = pts[2]->v[z];
        }
        else
        {
            if (zmax < pts[2]->v[z])
            {
                zmax = pts[2]->v[z];
            }
        }

        if (rayzmax < zmin)
            continue;
        if (rayzmin > zmax)
            continue;

        if (crossedfaces >= facesmaxsize)
            return crossedfaces;
        faces[crossedfaces] = i;
        crossedfaces++;
    }

    return crossedfaces;
}

float RayExtrudedFacesIntersection(CK3dEntity *floor, VxVector &p1, VxVector &p2, int face, VxVector &intersection, VxVector &edge1, VxVector &edge2)
{
    CKMesh *mesh = floor->GetCurrentMesh();

    float e = 0.001f;

    VxVector a, b, c, inter;
    CKDWORD StridePos;
    CKBYTE *m_VertexArray = (CKBYTE *)mesh->GetPositionsPtr(&StridePos);
    int fa, fb, fc;
    mesh->GetFaceVertexIndex(face, fa, fb, fc);
    floor->Transform(&a, (VxVector *)&m_VertexArray[fa * StridePos]);
    floor->Transform(&b, (VxVector *)&m_VertexArray[fb * StridePos]);
    floor->Transform(&c, (VxVector *)&m_VertexArray[fc * StridePos]);

    VxVector norm;

    // we swap the z and x to make a perpendicual vector to the edge, in the xz plane
    norm.x = b.z - a.z;
    norm.y = 0;
    norm.z = a.x - b.x;
    norm.Normalize();

    float sm, maxsm = 10000000.0f;

    VxPlane plane(a, norm);
    VxRay ray(p1, p2);
    float t;

    CKBOOL inters = FALSE;
    if (VxIntersect::SegmentPlane(ray, plane, inter, t))
    {
        // we now have to test if the intersection is in the edge limit (one axis test is enough)
        if (a.x > b.x)
        {
            // a < b
            if (inter.x >= b.x - e && inter.x <= a.x + e)
                inters = TRUE;
        }
        else
        {
            // b < a
            if (inter.x >= a.x - e && inter.x <= b.x + e)
                inters = TRUE;
        }
    }
    if (inters)
    {
        sm = SquareMagnitude(p2 - inter);
        if (sm < maxsm)
        {
            maxsm = sm;
            intersection = inter;
            edge1 = a;
            edge2 = b;
        }
    }

    // we swap the z and x to make a perpendicual vector to the edge, in the xz plane
    norm.x = c.z - a.z;
    norm.y = 0;
    norm.z = a.x - c.x;
    norm.Normalize();
    inters = FALSE;
    plane.Create(c, norm);
    if (VxIntersect::SegmentPlane(ray, plane, inter, t))
    {
        // we now have to test if the intersection is in the edge limit (one axis test is enough)
        if (a.x > c.x)
        {
            // c < a
            if (inter.x >= c.x - e && inter.x <= a.x + e)
                inters = TRUE;
        }
        else
        {
            // a < c
            if (inter.x >= a.x - e && inter.x <= c.x + e)
                inters = TRUE;
        }
    }
    if (inters)
    {
        sm = SquareMagnitude(p2 - inter);
        if (sm < maxsm)
        {
            maxsm = sm;
            intersection = inter;
            edge1 = a;
            edge2 = c;
        }
    }

    // we swap the z and x to make a perpendicual vector to the edge, in the xz plane
    norm.x = c.z - b.z;
    norm.y = 0;
    norm.z = b.x - c.x;
    norm.Normalize();
    inters = FALSE;
    plane.Create(b, norm);
    if (VxIntersect::SegmentPlane(ray, plane, inter, t))
    {
        // we now have to test if the intersection is in the edge limit (one axis test is enough)
        if (c.x > b.x)
        {
            // b < c
            if (inter.x >= b.x - e && inter.x <= c.x + e)
                inters = TRUE;
        }
        else
        {
            // c < b
            if (inter.x >= c.x - e && inter.x <= b.x + e)
                inters = TRUE;
        }
    }

    if (inters)
    {
        sm = SquareMagnitude(p2 - inter);
        if (sm < maxsm)
        {
            maxsm = sm;
            intersection = inter;
            edge1 = b;
            edge2 = c;
        }
    }

    return maxsm;
}
