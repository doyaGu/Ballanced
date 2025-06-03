/*************************************************************************/
/*	File : CollisionManager.cpp										     */
/*																		 */
/*************************************************************************/
#include "CKAll.h"
#include "CollisionManager.h"
#include "float.h"

/*************************************************
Name: BoxBoxIntersection

Summary: Detect a collision between two 3d entities at the bounding box level

Arguments:
    ent1: the first 3dEntity
    hiera1: whether or not to test the entire hierarchy of entity 1
    local1: do we want to test the local box(more precise, slower) or the world box (less precise, faster) of the first entity
    ent2: the second 3dEntity
    hiera2: whether or not we want to test the entire hierarchy of entity 2
    local2: do we want to test the local box(more precise, slower) or the world box (less precise, faster) of the second entity

Return Value: TRUE if they intersect, FALSE otherwise

Remarks:

{Group:Advanced Intersection Functions}

See also: CollisionManager::BoxFaceIntersection, CollisionManager::FaceFaceIntersection
*************************************************/
CKBOOL CollisionManager::BoxBoxIntersection(CK3dEntity *ent1, CKBOOL hiera1, CKBOOL local1, CK3dEntity *ent2, CKBOOL hiera2, CKBOOL local2)
{
    // Object 1
    VxMatrix Mat1;
    VxBbox box1;
    if (ent1->GetClassID() == CKCID_CHARACTER)
    {
        CKBodyPart *root = ((CKCharacter *)ent1)->GetRootBodyPart();
        if (!root)
            return FALSE;
        if (local1)
            Mat1 = root->GetWorldMatrix();
        box1 = root->GetHierarchicalBox(local1);
    }
    else
    {
        if (local1)
            Mat1 = ent1->GetWorldMatrix();
        if (hiera1)
            box1 = ent1->GetHierarchicalBox(local1);
        else
            box1 = ent1->GetBoundingBox(local1);
    }

    // Object 2
    VxMatrix Mat2;
    VxBbox box2;
    if (ent2->GetClassID() == CKCID_CHARACTER)
    {
        CKBodyPart *root = ((CKCharacter *)ent2)->GetRootBodyPart();
        if (!root)
            return FALSE;
        if (local2)
            Mat2 = root->GetWorldMatrix();
        box2 = root->GetHierarchicalBox(local2);
    }
    else
    {
        if (local2)
            Mat2 = ent2->GetWorldMatrix();
        if (hiera2)
            box2 = ent2->GetHierarchicalBox(local2);
        else
            box2 = ent2->GetBoundingBox(local2);
    }

    if (local1 | local2)
    {
        if (local1)
        {
            if (local2)
            { // 1 and 2 locals
                return VxIntersect::OBBOBB(VxOBB(box1, Mat1), VxOBB(box2, Mat2));
            }
            else
            { // 1 local and 2 in world
                return VxIntersect::AABBOBB(box2, VxOBB(box1, Mat1));
            }
        }
        else
        { // 1 in world and 2 local
            return VxIntersect::AABBOBB(box1, VxOBB(box2, Mat2));
        }
    }
    else
    { // we want simple world box intersection
        return VxIntersect::AABBAABB(box1, box2);
    }
}

/*************************************************
Name: BoxFaceIntersection

Summary: Detect a collision between two 3d entities at the bounding box level for the first object
and af the face level for the second one.

Arguments:
    ent1: the first 3dEntity
    hiera1: whether or not to test the entire hierarchy of entity 1
    local1: do we want to test the local box(more precise, slower) or the world box (less precise, faster) of the first entity
    ent2: the second 3dEntity

Return Value: TRUE if they intersect, FALSE otherwise

Remarks:

{Group:Advanced Intersection Functions}

See also: CollisionManager::BoxFaceIntersection, CollisionManager::FaceFaceIntersection
*************************************************/
CKBOOL CollisionManager::BoxFaceIntersection(CK3dEntity *ent1, CKBOOL hiera1, CKBOOL local1, CK3dEntity *ent2)
{
    // TODO : some optimisations can be done : flags the face concerned
    // do the intbox like in FaceFace....

#define V_RIGHT     0x01
#define V_LEFT      0x02
#define V_TOP       0x04
#define V_BOTTOM    0x08
#define V_FRONT     0x10
#define V_BACK      0x20
#define V_ALL       0x3F

    // Object 1
    VxMatrix InvMat1 = VxMatrix::Identity();
    VxBbox box1;
    if (ent1->GetClassID() == CKCID_CHARACTER)
    {
        CKBodyPart *root = ((CKCharacter *)ent1)->GetRootBodyPart();
        if (!root)
            return FALSE;
        if (local1)
            InvMat1 = root->GetInverseWorldMatrix();
        box1 = root->GetHierarchicalBox(local1);
    }
    else
    {
        if (local1)
            InvMat1 = ent1->GetInverseWorldMatrix();
        if (hiera1)
            box1 = ent1->GetHierarchicalBox(local1);
        else
            box1 = ent1->GetBoundingBox(local1);
    }

    VxVector pts[8];
    // pts : points of the bounding box expressed in local coordinates
    pts[0].x = box1.Min.x;
    pts[0].y = box1.Min.y;
    pts[0].z = box1.Min.z;
    pts[1].x = box1.Min.x;
    pts[1].y = box1.Min.y;
    pts[1].z = box1.Max.z;
    pts[2].x = box1.Min.x;
    pts[2].y = box1.Max.y;
    pts[2].z = box1.Min.z;
    pts[3].x = box1.Min.x;
    pts[3].y = box1.Max.y;
    pts[3].z = box1.Max.z;
    pts[4].x = box1.Max.x;
    pts[4].y = box1.Min.y;
    pts[4].z = box1.Min.z;
    pts[5].x = box1.Max.x;
    pts[5].y = box1.Min.y;
    pts[5].z = box1.Max.z;
    pts[6].x = box1.Max.x;
    pts[6].y = box1.Max.y;
    pts[6].z = box1.Min.z;
    pts[7].x = box1.Max.x;
    pts[7].y = box1.Max.y;
    pts[7].z = box1.Max.z;

    // Object 2
    CKMesh *Mesh2 = ent2->GetCurrentMesh();
    if (!Mesh2)
        return FALSE;
    int Mesh2FaceCount = Mesh2->GetFaceCount();
    if (!Mesh2FaceCount)
        return FALSE;

    VxMatrix Mat = InvMat1 * ent2->GetWorldMatrix();

    int acount = Mesh2->GetVertexCount();

    XArray<CKDWORD> memp1(acount * 4); // 3 for vector + 1 for vFlags

    CKDWORD *avFlags = memp1.Begin();
    VxVector *av = (VxVector *)(avFlags + acount);

    CKDWORD pStride;
    void *apos = Mesh2->GetPositionsPtr(&pStride);

    // we transform the vertices of 1 in the referential of 2, and vice versa
    VxStridedData src(apos, pStride);
    VxStridedData dst(av, sizeof(VxVector));
    Vx3DMultiplyMatrixVectorStrided(&dst, &src, Mat, acount);

    VxVector *vx = av;
    CKDWORD all = V_ALL;
    for (int x = 0; x < acount; x++, ++vx)
    {
        avFlags[x] = 0;
        if (vx->x < box1.Min.x)
            avFlags[x] |= V_LEFT;
        else if (vx->x > box1.Max.x)
            avFlags[x] |= V_RIGHT;
        if (vx->y < box1.Min.y)
            avFlags[x] |= V_BOTTOM;
        else if (vx->y > box1.Max.y)
            avFlags[x] |= V_TOP;
        if (vx->z < box1.Min.z)
            avFlags[x] |= V_FRONT;
        else if (vx->z > box1.Max.z)
            avFlags[x] |= V_BACK;
        all &= avFlags[x];
    }

    if (!all)
    {
        CKWORD *aindices = Mesh2->GetFacesIndices();

        for (int i = 0; i < Mesh2FaceCount; i++, aindices += 3)
        {
            if (!avFlags[aindices[0]])
                return TRUE;
            if (!avFlags[aindices[1]])
                return TRUE;
            if (!avFlags[aindices[2]])
                return TRUE;
            if (!(avFlags[aindices[0]] & avFlags[aindices[1]] & avFlags[aindices[2]]))
            {
                // we now have to test the edges of the face
                VxVector *p0 = av + aindices[0];
                VxVector *p1 = av + aindices[1];
                VxVector *p2 = av + aindices[2];
                if (VxIntersect::SegmentBox(VxRay(*p0, *p1), box1))
                    return TRUE;
                if (VxIntersect::SegmentBox(VxRay(*p1, *p2), box1))
                    return TRUE;
                if (VxIntersect::SegmentBox(VxRay(*p0, *p2), box1))
                    return TRUE;

                // if no segment of the face intersect the box, maybe one of the box diagonals intersect the face
                // if faut tester les 4 diagonales du cube

                VxVector normal = Mesh2->GetFaceNormal(i);
                ;
                Vx3DRotateVector(&normal, Mat, &normal);

                int i0 = 0;
                int i1 = 7;
                float max = XFabs(DotProduct(normal, pts[0] - pts[7]));
                float dp;

                if ((dp = XFabs(DotProduct(normal, pts[1] - pts[6]))) > max)
                {
                    max = dp;
                    i0 = 1;
                    i1 = 6;
                }
                if ((dp = XFabs(DotProduct(normal, pts[2] - pts[5]))) > max)
                {
                    max = dp;
                    i0 = 2;
                    i1 = 5;
                }
                if ((dp = XFabs(DotProduct(normal, pts[3] - pts[4]))) > max)
                {
                    max = dp;
                    i0 = 3;
                    i1 = 4;
                }

                VxVector res;
                if (VxIntersect::SegmentFace(VxRay(pts[i0], pts[i1]), *p0, *p1, *p2, normal, res, max))
                    return TRUE;

                // well all this work for nothing : there is no intersection for this face
            }
        }
    }

    return FALSE;
}

/*************************************************
Name: FaceFaceIntersection

Summary: Detect a collision between two 3d entities at the face level

Arguments:
    ent1: the first 3dEntity
    ent2: the second 3dEntity

Return Value: TRUE if they intersect, FALSE otherwise

Remarks:
    Detect a collision between two 3d entities at the face level

{Group:Advanced Intersection Functions}

See also: CollisionManager::BoxFaceIntersection, CollisionManager::FaceFaceIntersection
*************************************************/
CKBOOL CollisionManager::FaceFaceIntersection(CK3dEntity *ent1, CK3dEntity *ent2)
{
#define V1_RIGHT    0x01
#define V1_LEFT     0x02
#define V1_TOP      0x04
#define V1_BOTTOM   0x08
#define V1_FRONT    0x10
#define V1_BACK     0x20
#define V1_ALL      0x3F

#define V2_RIGHT    0x0100
#define V2_LEFT     0x0200
#define V2_TOP      0x0400
#define V2_BOTTOM   0x0800
#define V2_FRONT    0x1000
#define V2_BACK     0x2000
#define V2_ALL      0x3F00

#define FACEEPSILON 0.001f
    if (!ent1 || !ent2)
        return FALSE;

    const VxBbox &box1 = ent1->GetBoundingBox();
    const VxBbox &box2 = ent2->GetBoundingBox();

    if (box1.Min.x > box2.Max.x + FACEEPSILON)
        return FALSE;
    if (box1.Min.y > box2.Max.y + FACEEPSILON)
        return FALSE;
    if (box1.Min.z > box2.Max.z + FACEEPSILON)
        return FALSE;

    if (box1.Max.x < box2.Min.x - FACEEPSILON)
        return FALSE;
    if (box1.Max.y < box2.Min.y - FACEEPSILON)
        return FALSE;
    if (box1.Max.z < box2.Min.z - FACEEPSILON)
        return FALSE;

    CKWORD spriteindices[6] = {0, 1, 2, 0, 2, 3};
    VxVector spritenormals[2] = {VxVector(0.0f, 0.0f, 1.0f), VxVector(0.0f, 0.0f, 1.0f)};
    VxVector spritepos1[4];
    VxVector spritepos2[4];

    // Object 1
    int fcount1;
    int lcount1;
    int vcount1;
    void *vpos1;
    CKBYTE *fnormals1;
    CKWORD *findices1;
    CKWORD *lindices1;
    CKDWORD pStride1;
    CKDWORD nStride1;

    if (CKIsChildClassOf(ent1, CKCID_SPRITE3D))
    { // Sprite 3D
        fcount1 = 2;
        lcount1 = 0;
        vcount1 = 4;
        vpos1 = &spritepos1[0];
        findices1 = &spriteindices[0];
        fnormals1 = (CKBYTE *)&spritenormals[0];
        pStride1 = sizeof(VxVector);
        nStride1 = sizeof(VxVector);
        // filling the vectors positions
        CKSprite3D *sprite = (CKSprite3D *)ent1;
        Vx2DVector size;
        sprite->GetSize(size);
        Vx2DVector offset;
        sprite->GetSize(size);
        spritepos1[0].x = (-1.0f + offset.x) * size.x;
        spritepos1[0].y = (-1.0f + offset.y) * size.y;
        spritepos1[1].x = (-1.0f + offset.x) * size.x;
        spritepos1[1].y = (1.0f + offset.y) * size.y;
        spritepos1[2].x = (1.0f + offset.x) * size.x;
        spritepos1[2].y = (1.0f + offset.y) * size.y;
        spritepos1[3].x = (1.0f + offset.x) * size.x;
        spritepos1[3].y = (-1.0f + offset.y) * size.y;
    }
    else
    { // Mesh Normal
        CKMesh *mesh = ent1->GetCurrentMesh();
        if (!mesh)
            return FALSE;
        vcount1 = mesh->GetVertexCount();
        if (!vcount1)
            return FALSE;
        fcount1 = mesh->GetFaceCount();
        lcount1 = mesh->GetLineCount();
        vpos1 = mesh->GetPositionsPtr(&pStride1);
        findices1 = mesh->GetFacesIndices();
        fnormals1 = mesh->GetFaceNormalsPtr(&nStride1);
        lindices1 = mesh->GetLineIndices();
    }

    // Object 2
    int fcount2;
    int lcount2;
    int vcount2;
    void *vpos2;
    CKBYTE *fnormals2;
    CKWORD *findices2;
    CKWORD *lindices2;
    CKDWORD pStride2;
    CKDWORD nStride2;

    if (CKIsChildClassOf(ent2, CKCID_SPRITE3D))
    { // Sprite 3D
        fcount2 = 2;
        lcount2 = 0;
        vcount2 = 4;
        vpos2 = &spritepos2[0];
        findices2 = &spriteindices[0];
        fnormals2 = (CKBYTE *)&spritenormals[0];
        pStride2 = sizeof(VxVector);
        nStride2 = sizeof(VxVector);
        // filling the vectors positions
        CKSprite3D *sprite = (CKSprite3D *)ent2;
        Vx2DVector size;
        sprite->GetSize(size);
        Vx2DVector offset;
        sprite->GetSize(size);
        spritepos2[0].x = (-1.0f + offset.x) * size.x;
        spritepos2[0].y = (-1.0f + offset.y) * size.y;
        spritepos2[1].x = (-1.0f + offset.x) * size.x;
        spritepos2[1].y = (1.0f + offset.y) * size.y;
        spritepos2[2].x = (1.0f + offset.x) * size.x;
        spritepos2[2].y = (1.0f + offset.y) * size.y;
        spritepos2[3].x = (1.0f + offset.x) * size.x;
        spritepos2[3].y = (-1.0f + offset.y) * size.y;
    }
    else
    { // Mesh Normal
        CKMesh *mesh = ent2->GetCurrentMesh();
        if (!mesh)
            return FALSE;
        vcount2 = mesh->GetVertexCount();
        if (!vcount2)
            return FALSE;
        fcount2 = mesh->GetFaceCount();
        lcount2 = mesh->GetLineCount();
        vpos2 = mesh->GetPositionsPtr(&pStride2);
        findices2 = mesh->GetFacesIndices();
        fnormals2 = mesh->GetFaceNormalsPtr(&nStride2);
        lindices2 = mesh->GetLineIndices();
    }

    // Matrix
    VxMatrix Mat21 = ent1->GetInverseWorldMatrix() * ent2->GetWorldMatrix();
    VxMatrix Mat12 = ent2->GetInverseWorldMatrix() * ent1->GetWorldMatrix();

    // we calculate the local boxes of the intersection
    VxBbox intbox1, intbox2;
    VxBbox lbox1 = ent1->GetBoundingBox(TRUE);
    const VxVector faceEpsilon(FACEEPSILON);
    lbox1.Max += faceEpsilon;
    lbox1.Min -= faceEpsilon;
    VxBbox lbox2 = ent2->GetBoundingBox(TRUE);
    lbox2.Max += faceEpsilon;
    lbox2.Min -= faceEpsilon;
    intbox1.TransformFrom(lbox1, Mat12);
    intbox2.TransformFrom(lbox2, Mat21);

    int max = XMax(vcount1, vcount2);
    XArray<CKDWORD> memp1(max + vcount1 * 3); // 3 for vector + 1 for vFlags

    CKDWORD *avFlags = (CKDWORD *)memp1.Begin();
    VxVector *bvIn1 = (VxVector *)(avFlags + max);

    // we transform the vertices of 1 in the referential of 2, and vice versa
    VxStridedData src(vpos2, pStride2);
    VxStridedData dst(bvIn1, sizeof(VxVector));
    Vx3DMultiplyMatrixVectorStrided(&dst, &src, Mat21, vcount2);

    // we clear the vertices
    memset(avFlags, 0, max * 4);

    // We flag the vertices
    VxVector *vx;
    CKBYTE *var = (CKBYTE *)vpos1;
    int x;
    for (x = 0; x < vcount1; x++, var += pStride1)
    {
        vx = (VxVector *)var;
        if (vx->x < intbox2.Min.x)
            avFlags[x] |= V1_LEFT;
        else if (vx->x > intbox2.Max.x)
            avFlags[x] |= V1_RIGHT;
        if (vx->y < intbox2.Min.y)
            avFlags[x] |= V1_BOTTOM;
        else if (vx->y > intbox2.Max.y)
            avFlags[x] |= V1_TOP;
        if (vx->z < intbox2.Min.z)
            avFlags[x] |= V1_FRONT;
        else if (vx->z > intbox2.Max.z)
            avFlags[x] |= V1_BACK;
    }

    var = (CKBYTE *)vpos2;
    for (x = 0; x < vcount2; x++, var += pStride2)
    {
        vx = (VxVector *)var;
        if (vx->x < intbox1.Min.x)
            avFlags[x] |= V2_LEFT;
        else if (vx->x > intbox1.Max.x)
            avFlags[x] |= V2_RIGHT;
        if (vx->y < intbox1.Min.y)
            avFlags[x] |= V2_BOTTOM;
        else if (vx->y > intbox1.Max.y)
            avFlags[x] |= V2_TOP;
        if (vx->z < intbox1.Min.z)
            avFlags[x] |= V2_FRONT;
        else if (vx->z > intbox1.Max.z)
            avFlags[x] |= V2_BACK;
    }

    CKBYTE *av = (CKBYTE *)vpos1;
    VxVector *bv = bvIn1;

    CKWORD *bindices = findices2;

    ///////////////////////////////////
    // Test faces 1 vs Faces 2
    int count = 0;
    VxVector bn;
    int i;
    for (i = 0; i < fcount2; ++i, bindices += 3)
    {
        if ((avFlags[bindices[0]] & avFlags[bindices[1]] & avFlags[bindices[2]]) & V2_ALL)
            continue;

        VxVector *b0 = bv + bindices[0];
        VxVector *b1 = bv + bindices[1];
        VxVector *b2 = bv + bindices[2];
        Vx3DRotateVector(&bn, Mat21, (VxVector *)(fnormals2 + i * nStride2));

        CKWORD *aindices = findices1;
        for (int j = 0; j < fcount1; ++j, aindices += 3)
        {
            if ((avFlags[aindices[0]] & avFlags[aindices[1]] & avFlags[aindices[2]]) & V1_ALL)
                continue;

            VxVector *a0 = (VxVector *)(av + aindices[0] * pStride1);
            VxVector *a1 = (VxVector *)(av + aindices[1] * pStride1);
            VxVector *a2 = (VxVector *)(av + aindices[2] * pStride1);
            VxVector *an = (VxVector *)(fnormals1 + j * nStride1);
            if (VxIntersect::FaceFace(*a0, *a1, *a2, *an, *b0, *b1, *b2, bn))
            {
                m_TouchingFaceIndex = j;
                m_TouchedFaceIndex = i;
                return TRUE;
            }
        }
    }

    //////////////////////////////////
    // we now have to handle lines

    // Test Faces 1 vs Lines 2
    if (lcount2)
    {
        // dummy variables
        VxVector res;
        float t;

        CKWORD *aindices = findices1;
        for (i = 0; i < fcount1; ++i, aindices += 3)
        {
            if ((avFlags[aindices[0]] & avFlags[aindices[1]] & avFlags[aindices[2]]) & V1_ALL)
                continue;

            VxVector *a0 = (VxVector *)(av + aindices[0] * pStride1);
            VxVector *a1 = (VxVector *)(av + aindices[1] * pStride1);
            VxVector *a2 = (VxVector *)(av + aindices[2] * pStride1);
            VxVector *an = (VxVector *)(fnormals1 + i * nStride1);

            bindices = lindices2;
            for (int j = 0; j < lcount2; ++j, bindices += 2)
            {
                if ((avFlags[bindices[0]] & avFlags[bindices[1]]) & V2_ALL)
                    continue;
                VxVector *b0 = bv + bindices[0];
                VxVector *b1 = bv + bindices[1];

                if (VxIntersect::SegmentFace(VxRay(*b0, *b1), *a0, *a1, *a2, *an, res, t))
                {
                    return TRUE;
                }
            }
        }
    }

    // Test Faces 2 vs Lines 1
    if (lcount1)
    {
        // dummy variables
        VxVector res;
        float t;

        bindices = findices2;
        for (i = 0; i < fcount2; ++i, bindices += 3)
        {
            if ((avFlags[bindices[0]] & avFlags[bindices[1]] & avFlags[bindices[2]]) & V2_ALL)
                continue;

            VxVector *b0 = bv + bindices[0];
            VxVector *b1 = bv + bindices[1];
            VxVector *b2 = bv + bindices[2];
            Vx3DRotateVector(&bn, Mat21, (VxVector *)(fnormals2 + i * nStride2));

            CKWORD *aindices = lindices1;
            for (int j = 0; j < lcount1; ++j, aindices += 2)
            {
                if ((avFlags[aindices[0]] & avFlags[aindices[1]]) & V1_ALL)
                    continue;
                VxVector *a0 = (VxVector *)(av + aindices[0] * pStride1);
                VxVector *a1 = (VxVector *)(av + aindices[1] * pStride1);

                if (VxIntersect::SegmentFace(VxRay(*a0, *a1), *b0, *b1, *b2, bn, res, t))
                {
                    return TRUE;
                }
            }
        }
    }

    // Test Lines 1 vs Lines 2
    if (lcount1 && lcount2)
    {
        VxVector res = box1.Max - box1.Min;
        float threshold = SquareMagnitude(res) * 0.0001f;

        bindices = lindices2;
        for (i = 0; i < lcount2; ++i, bindices += 2)
        {
            if ((avFlags[bindices[0]] & avFlags[bindices[1]]) & V2_ALL)
                continue;

            VxVector *b0 = bv + bindices[0];
            VxVector *b1 = bv + bindices[1];

            CKWORD *aindices = lindices1;
            for (int j = 0; j < lcount1; ++j, aindices += 2)
            {
                if ((avFlags[aindices[0]] & avFlags[aindices[1]]) & V1_ALL)
                    continue;

                VxVector *a0 = (VxVector *)(av + aindices[0] * pStride1);
                VxVector *a1 = (VxVector *)(av + aindices[1] * pStride1);

                if (VxDistance::SegmentSegmentSquareDistance(VxRay(*a0, *a1), VxRay(*b0, *b1)) < threshold)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

/*************************************************
Name: IsInCollision

Summary: Check if two 3dEntities are in collision

Arguments:
    ent1: first obstacle
    precis_level1: the geometric precision level you want the ent1 to be at. CKCOLLISION_NONE means nothing for this function
    ent2: second obstacle
    precis_level2: the geometric precision level you want the ent2 to be at. CKCOLLISION_NONE means nothing for this function

Return Value:
    TRUE if the two entites are colliding, NULL otherwise.

Remarks:
    This function is not finished yet. Its only check for obstacle with world bounding box occlusion, which
    is by far too simple...
{Group:Advanced Intersection Functions}

See also: CollisionManager
*************************************************/
CKBOOL CollisionManager::IsInCollision(CK3dEntity *ent1, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2)
{
    switch (precis_level1 | precis_level2)
    {
    case CKCOLLISION_BOX:
        return BoxBoxIntersection(ent1, FALSE, FALSE, ent2, FALSE, FALSE);
        // TODO : reput this line back
        //		return BoxBoxIntersection(ent1,FALSE,TRUE,ent2,FALSE,TRUE);
        break;
    case CKCOLLISION_FACE:
        return FaceFaceIntersection(ent1, ent2);
        break;
    case CKCOLLISION_FACE | CKCOLLISION_BOX:
        if (precis_level1 & CKCOLLISION_BOX)
        {
            return BoxFaceIntersection(ent1, FALSE, TRUE, ent2);
        }
        else
        {
            return BoxFaceIntersection(ent2, FALSE, TRUE, ent1);
        }
        break;
    }
    return FALSE;
}

/*************************************************
Name: IsInCollisionWithHierarchy

Summary: Check if two 3dEntities are in collision, the second one considered with all its sub-hierarchy

Arguments:
    ent1: first obstacle
    precis_level1: the geometric precision level you want the ent1 to be at. CKCOLLISION_NONE means nothing for this function
    ent2: second obstacle, hierarchic.
    precis_level2: the geometric precision level you want the ent2 to be at. CKCOLLISION_NONE means nothing for this function

Return Value:
    The pointer to the sub-object of entity 2 if the two entites are colliding, NULL otherwise.

Remarks:
    Check if two 3dEntities are in collision, the second one considered with all its sub-hierarchy. All the sub objects of
    entity 2 are tested at the same level of precision : precis_level2
{Group:Advanced Intersection Functions}

See also: CollisionManager, CollisionManager::IsHierarchyInCollisionWithHierarchy
*************************************************/
CK3dEntity *CollisionManager::IsInCollisionWithHierarchy(CK3dEntity *ent1, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2)
{
    CK3dEntity *touched;

    // if the hierarchy is a character, we go down to the root immediatly...
    if (ent2->GetClassID() == CKCID_CHARACTER)
    {
        ent2 = ((CKCharacter *)ent2)->GetRootBodyPart();
        // Character has no root : we can't detect collision thus....
        if (!ent2)
            return NULL;
    }

    // hierarchical rejection with world hierarchical box
    if (!BoxBoxIntersection(ent1, FALSE, FALSE, ent2, TRUE, FALSE))
    {
        return NULL;
    }

    // if we arrived here, we know there could be a collision with the root
    // so we test it right now
    if (IsInCollision(ent1, precis_level1, ent2, precis_level2))
        return ent2;

    // the root wasn't in collision so we go down in the hierarchy
    for (int i = 0; i < ent2->GetChildrenCount(); i++)
    {
        CK3dEntity *child = ent2->GetChild(i);
        if (touched = IsInCollisionWithHierarchy(ent1, precis_level1, child, precis_level2))
            return touched;
    }
    return NULL;
}

/*************************************************
Name: IsHierarchyInCollisionWithHierarchy

Summary: Check if two 3dEntities are in collision, the two considered with all their sub-hierarchy

Arguments:
    ent1: first obstacle, hierarchic
    precis_level1: the geometric precision level you want the ent1 to be at. CKCOLLISION_NONE means nothing for this function
    ent2: second obstacle, hierarchic.
    precis_level2: the geometric precision level you want the ent2 to be at. CKCOLLISION_NONE means nothing for this function
    sub: pointer on the sub object of the entity1, if there is a collision
    subob: pointer on the sub object of the entity2, if there is a collision

Return Value:
    TRUE if the two entites are colliding, NULL otherwise.

Remarks:
    Check if two 3dEntities are in collision, the two considered with all their sub-hierarchy

{Group:Advanced Intersection Functions}

See also: CollisionManager, CollisionManager::IsInCollisionWithHierarchy
*************************************************/
CKBOOL CollisionManager::IsHierarchyInCollisionWithHierarchy(CK3dEntity *ent1, CK_GEOMETRICPRECISION precis_level1, CK3dEntity *ent2, CK_GEOMETRICPRECISION precis_level2, CK3dEntity **sub, CK3dEntity **subob)
{
    CK3dEntity *touched;

    // if the hierarchy is a character, we go down to the root immediatly...
    if (ent1->GetClassID() == CKCID_CHARACTER)
    {
        ent1 = ((CKCharacter *)ent1)->GetRootBodyPart();
        // Character has no root : we can't detect collision thus....
        if (!ent1)
            return NULL;
    }

    // if the hierarchy is a character, we go down to the root immediatly...
    if (ent2->GetClassID() == CKCID_CHARACTER)
    {
        ent2 = ((CKCharacter *)ent2)->GetRootBodyPart();
        // Character has no root : we can't detect collision thus....
        if (!ent2)
            return NULL;
    }

    // hierarchical rejection with world hierarchical box
    if (!BoxBoxIntersection(ent1, TRUE, FALSE, ent2, TRUE, FALSE))
    {
        return NULL;
    }

    // if we arrived here, we know there could be a collision with the root and the other hierarchy
    // so we test it right now
    if (touched = IsInCollisionWithHierarchy(ent1, precis_level1, ent2, precis_level2))
    {
        *sub = ent1;
        *subob = touched;
        return TRUE;
    }

    // the root wasn't in collision so we go down in the hierarchy
    for (int i = 0; i < ent1->GetChildrenCount(); i++)
    {
        CK3dEntity *child = ent1->GetChild(i);
        if (IsHierarchyInCollisionWithHierarchy(child, precis_level1, ent2, precis_level2, sub, subob))
            return TRUE;
    }

    return FALSE;
}
