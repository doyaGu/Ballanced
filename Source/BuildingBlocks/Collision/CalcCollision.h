#ifndef CALC_COLLISION_H
#define CALC_COLLISION_H

#include "VxMath.h"
#include "CKMesh.h"

class CollBallFace
{
public:
    CollBallFace(float rad = 1.0f, VxVector *pos_ball = NULL, CKMesh *mes = NULL);
    void UpdateValues(float rad = 1.0f, VxVector *pos_ball = NULL, CKMesh *mes = NULL);
    CKBOOL DetectCollisionWithBallAndFace();

    CKMesh *mesh;
    int face;
    VxVector *ball_pos; // ball position transformed in the object referential
    VxVector *contact;  // contact expressed in the object referential
    float radius;       // radius expressed in the object referential (object scale must be homogeneous)
    float radius2;      // radius*radius
    XPtrStrided<VxVector> positions;
    XPtrStrided<VxVector> normals;
    CKWORD *faceindices;

private:
    VxVector norm, normalized_edge_ab, normalized_edge_bc, normalized_edge_ac;
    VxVector posa, posb, posc, tmp;
    VxVector pos_posa, pos_posb, pos_posc;
    VxVector edge_norm_ab, edge_norm_bc, edge_norm_ac;
    int sphere_flag;
    float y, s;
    float ab, bc, ac;
    float mab, mbc, mac;
};

#endif // CALC_COLLISION_H