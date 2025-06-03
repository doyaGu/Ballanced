#include "CalcCollision.h"

#include "CKAll.h"

///////////////////////////////////////////////////////////////
CollBallFace::CollBallFace(float rad, VxVector *pos_ball, CKMesh *msh)
{
    UpdateValues(rad, pos_ball, msh);
}

/*************************************************************/
// UpdateValues
//
// Update Mesh, Radius and Local_Position
//
// Warning : Does not update the face index !
/*************************************************************/
void CollBallFace::UpdateValues(float rad, VxVector *pos_ball, CKMesh *msh)
{
    ball_pos = pos_ball;
    radius = rad;
    mesh = msh;

    radius2 = radius * radius;

    if (mesh)
    {
        CKDWORD stride = 0;
        void *v = mesh->GetPositionsPtr(&stride);

        positions = XPtrStrided<VxVector>(v, stride);

        v = mesh->GetFaceNormalsPtr(&stride);
        normals = XPtrStrided<VxVector>(v, stride);

        faceindices = mesh->GetFacesIndices();
    }
    else
    {
        positions = XPtrStrided<VxVector>(0, 0);
        normals = XPtrStrided<VxVector>(0, 0);
        faceindices = NULL;
    }
}

/*************************************************************/
// DetectCollisionWithBallAndFace
//
// Detect the collision between a ball and a face.
//
// A_CollBallFaces members variables which must be set :
//
// .mesh = object mesh pointer
// .ball_pos = ball position in the object referential
// .radius = radius expressed in the object referential
// .contact = pointer to an existing vector (loaded with the contact point)
// .face = ptr to the object face
//
/*************************************************************/
CKBOOL CollBallFace::DetectCollisionWithBallAndFace()
{

    norm = normals[face];
    posa = positions[(int)faceindices[face * 3 + 0]];
    // norm = mesh->GetFaceNormal(face);
    // posa = mesh->GetFaceVertex(face,0);
    pos_posa = *ball_pos - posa;

    y = DotProduct(norm, pos_posa);

    if (y >= radius)
        return FALSE;
    if (y <= 0.0f)
        return FALSE;

    posb = positions[(int)faceindices[face * 3 + 1]];
    // posb = mesh->GetFaceVertex(face,1);
    //---
    normalized_edge_ab = posb - posa;
    mab = Magnitude(normalized_edge_ab);
    normalized_edge_ab /= mab;

    edge_norm_ab = CrossProduct(normalized_edge_ab, norm);
    ab = DotProduct(pos_posa, edge_norm_ab);

    if (ab >= radius)
        return FALSE;

    posc = positions[(int)faceindices[face * 3 + 2]];
    // posc  = mesh->GetFaceVertex(face,2);
    //---
    normalized_edge_ac = posc - posa;
    mac = Magnitude(normalized_edge_ac);
    normalized_edge_ac /= mac;

    edge_norm_ac = CrossProduct(norm, normalized_edge_ac);
    ac = DotProduct(pos_posa, edge_norm_ac);

    if (ac >= radius)
        return FALSE;

    //---
    normalized_edge_bc = posc - posb;
    mbc = Magnitude(normalized_edge_bc);
    normalized_edge_bc /= mbc;

    edge_norm_bc = CrossProduct(normalized_edge_bc, norm);
    pos_posb = *ball_pos - posb;
    bc = DotProduct(pos_posb, edge_norm_bc);

    if (bc >= radius)
        return FALSE;

    //---
    if (ab <= 0.0f && bc <= 0.0f && ac <= 0.0f)
    {
        VxVector tmp1 = normalized_edge_ab * DotProduct(pos_posa, normalized_edge_ab);
        *contact = posa + tmp1 + ab * edge_norm_ab;
        return TRUE;
    }

    sphere_flag = 0x111;

    if (bc > 0.0f)
    {
        s = DotProduct(pos_posb, normalized_edge_bc);
        if (s < mbc)
        {
            if (s > 0.0f)
            {
                if (bc * bc + y * y >= radius2)
                    return FALSE;
                *contact = posb + s * normalized_edge_bc;
                return TRUE;
            }
            sphere_flag &= 0x110;
        }
    }

    if (ab > 0.0f)
    {
        s = DotProduct(pos_posa, normalized_edge_ab);
        if (s < mab)
        {
            if (s > 0.0f)
            {
                if (ab * ab + y * y >= radius2)
                    return FALSE;
                *contact = posa + s * normalized_edge_ab;
                return TRUE;
            }
            sphere_flag &= 0x101;
        }
    }

    if (ac > 0.0f)
    {
        s = DotProduct(pos_posa, normalized_edge_ac);
        if (s > 0.0f)
        {
            if (s < mac)
            {
                if (ac * ac + y * y >= radius2)
                    return FALSE;
                *contact = posa + s * normalized_edge_ac;
                return TRUE;
            }
            sphere_flag &= 0x011;
        }
    }

    //---
    if (sphere_flag & 0x100)
    {
        if (Magnitude(pos_posa) < radius)
        {
            *contact = posa;
            return TRUE;
        }
    }

    if (sphere_flag & 0x010)
    {
        if (Magnitude(pos_posb) < radius)
        {
            *contact = posb;
            return TRUE;
        }
    }

    pos_posc = *ball_pos - posc;
    if (sphere_flag & 0x010)
    {
        if (Magnitude(pos_posc) < radius)
        {
            *contact = posc;
            return TRUE;
        }
    }

    return FALSE;
}
