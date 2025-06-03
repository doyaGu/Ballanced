/*************************************************************************/
/*	File : CollisionManager.cpp										     */
/*																		 */
/*************************************************************************/
#include "CKAll.h"
#include "CollisionManager.h"

//{secret}
int CollisionManager::GetNearestFace(CK3dEntity *ent1, CK3dEntity *ent2)
{
    float d2, d2min = 10000000.0f;
    int facemin = 0;

    VxVector pos, lpos;
    VxVector *p0;
    VxVector *p1;
    VxVector *p2;
    ent2->GetBaryCenter(&lpos);
    ent1->InverseTransform(&pos, &lpos, ent2);

    CKMesh *mesh = ent1->GetCurrentMesh();
    if (!mesh)
        return -1;
    CKWORD *fidx = mesh->GetFacesIndices();
    int fc = mesh->GetFaceCount();

    CKDWORD vStride;
    CKBYTE *varray = (CKBYTE *)mesh->GetPositionsPtr(&vStride);

    VxVector bary;
    for (int i = 0; i < fc; i++, fidx += 3)
    {
        if (DotProduct(pos, mesh->GetFaceNormal(i)) > 0.0)
        {
            p0 = (VxVector *)(varray + fidx[0] * vStride);
            p1 = (VxVector *)(varray + fidx[1] * vStride);
            p2 = (VxVector *)(varray + fidx[2] * vStride);
            bary = (*p0 + *p1 + *p2) / 3.0f;

            // fisrst method
            d2 = SquareMagnitude(bary - pos);
            if (d2 < d2min)
            {
                d2min = d2;
                facemin = i;
            }

            /*
            // second method
            d2 = DotProduct(pos-bary,f->FaceNormal);
            if(d2 < d2min) {
                d2min = d2;
                facemin = i;
            }
            */

            /*
            // last method : NOT finshed : need to compare the two scalar product
            d2 = SquareMagnitude(bary - pos);
            if(d2 < d2min) {
                float ps = DotProduct(pos-bary,f->FaceNormal);

                d2min = d2;
                facemin = i;
            }
            */
        }
    }

    return facemin;
}

//{secret}
int CollisionManager::GetNearestFace(CK3dEntity *ent, VxVector &wpos)
{
    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return -1;

    int fc = mesh->GetFaceCount();
    CKWORD *fidx = mesh->GetFacesIndices();

    CKDWORD vStride;
    CKBYTE *varray = (CKBYTE *)mesh->GetPositionsPtr(&vStride);

    // we take the position of the given position in the entity referential
    VxVector pos;
    ent->InverseTransform(&pos, &wpos);

    VxVector fpos, bary;
    VxVector *p0;
    float d2, d2min = 10000000.0f;
    int facemin = 0;
    for (int i = 0; i < fc; ++i, fidx += 3)
    {
        p0 = (VxVector *)(varray + *fidx * vStride);
        fpos = pos - *p0;
        if ((d2 = DotProduct(fpos, mesh->GetFaceNormal(i))) > 0.0)
        {

            /*
            p1 = (VxVector*)(varray+fidx[1]*vStride);
            p2 = (VxVector*)(varray+fidx[2]*vStride);
            bary = (*p0+*p1+*p2)*0.3333333f;

            // fisrst method
            d2 = SquareMagnitude(bary - pos);
            */
            if (d2 < d2min)
            {
                d2min = d2;
                facemin = i;
            }
        }
    }

    return facemin;
}

//{secret}
CKBOOL CollisionManager::GetNearestVertex(CK3dEntity *ent1, CK3dEntity *ent2, VxVector &nearest)
{
    //	float d2,d2min=10000000.0f;
    //	int facemin=0;

    VxVector pos1, pos2;
    ent1->GetBaryCenter(&pos2);
    ent1->Transform(&pos2, &pos2);
    ent2->GetBaryCenter(&pos1);
    ent2->Transform(&pos1, &pos1);

    VxIntersectionDesc desc;
    ent1->RayIntersection(&pos1, &pos2, &desc, NULL);
    ent1->Transform(&nearest, &(desc.IntersectionPoint));
    /*
    CKMesh* mesh = ent1->GetCurrentMesh();
    if(!mesh) return FALSE;

    CKDWORD vStride;
    BYTE* varray = (BYTE*)mesh->GetPositionsPtr(&vStride);
    int vcount = mesh->GetVertexCount();


    VxVector* p0;
    VxVector bary;
    for(int i=0;i<vcount;i++,varray += vStride) {
        p0 = (VxVector*)varray;

        // fisrst method
        d2 = SquareMagnitude(*p0 - pos);
        if(d2 < d2min) {
            d2min = d2;
            nearest = *p0;
        }
    }
    */

    return TRUE;
}

//{secret}
int CollisionManager::GetNearestVertex(CK3dEntity *ent, VxVector &wpos)
{
    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return -1;

    CKDWORD vStride;
    CKBYTE *varray = (CKBYTE *)mesh->GetPositionsPtr(&vStride);

    int vcount = mesh->GetVertexCount();
    if (!vcount)
        return -1;

    VxVector *p0;
    float d2, d2min = 10000000.0f;
    int nearest;

    // we take the position of the given position in the entity referential
    VxVector pos;
    ent->InverseTransform(&pos, &wpos);

    // we iterate on each vertex to find the closest one from the
    // position 3d we gave
    for (int i = 0; i < vcount; ++i, varray += vStride)
    {
        p0 = (VxVector *)varray;

        // fisrst method
        d2 = SquareMagnitude(*p0 - pos);
        if (d2 < d2min)
        {
            d2min = d2;
            nearest = i;
        }
    }

    return nearest;
}
