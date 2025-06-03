/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Billboard
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBillboardDecl();
CKERROR CreateBillboardProto(CKBehaviorPrototype **pproto);
int Billboard(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBillboardDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Billboard");
    od->SetDescription("Makes a 3dEntity act like a billboard sprite (with possible constant texture ratio for a face).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Face Index: index of the face which orientation should be toward. If you provide a negative number, the building block will only take the Local Direction and the Local Up axis into concideration to orientate the object.</SPAN>.<BR>
    <SPAN CLASS=pin>Local Dir: </SPAN>if you do not specify any face index (nothing or -1), this vector will represent the vector in your object referential which is to be bound to the Z world axis.<BR>
    <SPAN CLASS=pin>Local Up: </SPAN>if you do not specify any face index (nothing or -1), this vector will represent the vector in your object referential which is to be bound to the Y world axis.<BR>
    <SPAN CLASS=pin>Texture 1/1: </SPAN>if TRUE, the object will be positioned so that the texture on the specified face will have exactly the same size that if it were seen in 2d (just like its representation in the texture setup).<BR>
    <SPAN CLASS=pin>Apply Transformation: </SPAN>if TRUE, the object world matrix will be bound to the calculate one. Otherwise, the object won't move, and the world matrix will only be put in the output parameter.<BR>
    <SPAN CLASS=pin>Scale Instead Of Move: </SPAN>if TRUE, the object will be scale instead of moving to match the texture scale.(only implemented on 3D Sprites for now.)<BR>
    <BR>
    This building block is useful to make the texture of an object allways have a constant aspect ratio.<BR>
    Suppose you have a cube with 6 texts written on each faces. If you want the text to be seen exactly as it has been writen (not stretched, but exactly the same aspect),
    you would use this building block, with a "Face Index"=index of your face, and a "Texture 1/1"=TRUE.<BR>
    */
    /* warning:
    - if you specify a "Face Index" (other than a negative number), the Local Direction and Local Up won't be taken into concideration.<BR>
    */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x641822b0, 0x15aa42d7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBillboardProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateBillboardProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Billboard");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Face Index", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Local Dir", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Local Up", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Texture Factor", CKPGUID_FLOAT, "1.0f");
    proto->DeclareInParameter("Apply Transformation", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Scale Instead Of Move", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("World Matrix", CKPGUID_MATRIX);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Billboard);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Billboard(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get the Owner Object
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!CKIsChildClassOf(ent, CKCID_3DENTITY))
        return CKBR_OWNERERROR;

    // Get The face index
    int face = -1;
    beh->GetInputParameterValue(0, &face);

    CK3dEntity *cam = behcontext.CurrentRenderContext->GetViewpoint();
    VxVector vdir, vup, vright;
    cam->GetOrientation(&vdir, &vup, &vright);

    // Object orientation & position
    VxVector n(0, 0, 1.0f);
    VxVector u(0, 1.0f, 0);

    float texfactor = 1.0f;
    beh->GetInputParameterValue(3, &texfactor);

    CKMesh *mesh = ent->GetCurrentMesh();
    CKTexture *tex = NULL;

    if (mesh && (face >= 0))
    { // We billboard regarding the face given
        if (face < mesh->GetFaceCount())
        {
            // direction
            n = mesh->GetFaceNormal(face);

            CKMaterial *mat = mesh->GetFaceMaterial(face);
            if (mat && (tex = mat->GetTexture()))
            {
                int fa, fb, fc;
                mesh->GetFaceVertexIndex(face, fa, fb, fc);

                // Analyse the texture coordinates
                Vx2DVector p0, p1, p2;
                // GEt the Uvs
                mesh->GetVertexTextureCoordinates(fa, &p0.x, &p0.y);
                mesh->GetVertexTextureCoordinates(fb, &p1.x, &p1.y);
                mesh->GetVertexTextureCoordinates(fc, &p2.x, &p2.y);

                // Delta uvs
                float u10 = p1.x - p0.x;
                float u20 = p2.x - p0.x;
                float v10 = p1.y - p0.y;
                float v20 = p2.y - p0.y;

                // Get the 3D Positions
                VxVector pt0, pt1, pt2;
                mesh->GetVertexPosition(fa, &pt0);
                mesh->GetVertexPosition(fb, &pt1);
                mesh->GetVertexPosition(fc, &pt2);

                // the final vector
                u = (u20 * (pt1 - pt0) - u10 * (pt2 - pt0)) / (v10 * u20 - v20 * u10);
                u.Normalize();
                u = -u;
            }
            else
            {
                u = mesh->GetFaceVertex(face, 0) - mesh->GetFaceVertex(face, 1);
                u.Normalize();
            }
        }
    }
    else
    { // We billboard regarding the axis given
        if (CKIsChildClassOf(ent, CKCID_SPRITE3D))
        {
            CKSprite3D *sprite = (CKSprite3D *)ent;

            CKMaterial *mat = sprite->GetMaterial();
            if (mat)
                tex = mat->GetTexture();
        }

        // Get the input axis
        beh->GetInputParameterValue(1, &n);
        beh->GetInputParameterValue(2, &u);
    }

    VxVector cp = CrossProduct(u, n);
    VxMatrix M = VxMatrix::Identity(), N = VxMatrix::Identity();

    // creating Ortho matrix
    // X Axis
    M[0][0] = n.x;
    M[1][0] = n.y;
    M[2][0] = n.z;
    // Y Axis
    M[0][1] = u.x;
    M[1][1] = u.y;
    M[2][1] = u.z;
    // Z Axis
    M[0][2] = cp.x;
    M[1][2] = cp.y;
    M[2][2] = cp.z;

    // creating rotation matrix
    // X Axis
    N[0][0] = -vdir.x;
    N[0][1] = -vdir.y;
    N[0][2] = -vdir.z;
    // Y Axis
    N[1][0] = vup.x;
    N[1][1] = vup.y;
    N[1][2] = vup.z;
    // Z Axis
    N[2][0] = -vright.x;
    N[2][1] = -vright.y;
    N[2][2] = -vright.z;

    VxMatrix B;
    Vx3DMultiplyMatrix(B, N, M);

    // we set back the position
    VxVector pos;
    ent->GetPosition(&pos);
    B[3][0] = pos.x;
    B[3][1] = pos.y;
    B[3][2] = pos.z;

    // we set back the scale if it's not a sprite
    if (mesh)
    {
        VxVector scale;
        ent->GetScale(&scale, FALSE);
        VxVector oldScale;
        oldScale.x = Magnitude(VxVector(B[0][0], B[1][0], B[2][0]));
        oldScale.y = Magnitude(VxVector(B[0][1], B[1][1], B[2][1]));
        oldScale.z = Magnitude(VxVector(B[0][2], B[1][2], B[2][2]));

        B[0][0] *= scale.x / oldScale.x;
        B[1][0] *= scale.x / oldScale.x;
        B[2][0] *= scale.x / oldScale.x;
        B[0][1] *= scale.y / oldScale.y;
        B[1][1] *= scale.y / oldScale.y;
        B[2][1] *= scale.y / oldScale.y;
        B[0][2] *= scale.z / oldScale.z;
        B[1][2] *= scale.z / oldScale.z;
        B[2][2] *= scale.z / oldScale.z;
    }

    CKBOOL move_object = TRUE;
    beh->GetInputParameterValue(4, &move_object);

    if (move_object)
        ent->SetWorldMatrix(B);

    // positioning based on the texture size
    if (tex && (texfactor > 0.0f))
    {
        VxVector posincam;
        ent->GetPosition(&posincam, cam);

        CKRenderContext *context = behcontext.CurrentRenderContext;

        // Get the local Positions
        VxVector pts[3];
        Vx2DVector uvs[3];

        if (mesh)
        {
            int fa, fb, fc;
            mesh->GetFaceVertexIndex(face, fa, fb, fc);

            mesh->GetVertexPosition(fa, &pts[0]);
            mesh->GetVertexPosition(fb, &pts[1]);
            mesh->GetVertexPosition(fc, &pts[2]);

            // GEt the Uvs
            mesh->GetVertexTextureCoordinates(fa, &uvs[0].x, &uvs[0].y);
            mesh->GetVertexTextureCoordinates(fb, &uvs[1].x, &uvs[1].y);
            mesh->GetVertexTextureCoordinates(fc, &uvs[2].x, &uvs[2].y);
        }
        else
        { // it must be a sprite 3D
            CKSprite3D *sprite = (CKSprite3D *)ent;

            Vx2DVector size;
            sprite->GetSize(size);
            size *= 0.5f;

            pts[0].Set(-size.x, -size.y, 0.0f);
            pts[1].Set(-size.x, size.y, 0.0f);
            pts[2].Set(size.x, size.y, 0.0f);

            uvs[0].x = 0.0f;
            uvs[0].y = 1.0f;
            uvs[1].x = 0.0f;
            uvs[1].y = 0.0f;
            uvs[2].x = 1.0f;
            uvs[2].y = 0.0f;
        }

        // transform to the screen
        VxVector screen[3];
        context->Transform(&screen[0], &pts[0], ent);
        context->Transform(&screen[1], &pts[1], ent);
        context->Transform(&screen[2], &pts[2], ent);

        float min = screen[0].y;
        float max = screen[0].y;
        float minv = uvs[0].y;
        float maxv = uvs[0].y;
        for (int i = 1; i < 3; ++i)
        {
            if (screen[i].y < min)
            {
                min = screen[i].y;
                minv = uvs[i].y;
            }
            else
            {
                if (screen[i].y > max)
                {
                    max = screen[i].y;
                    maxv = uvs[i].y;
                }
            }
        }

        float oldsize = (max - min) / (maxv - minv);
        float desiredsize = texfactor * tex->GetHeight();

        CKBOOL scale = FALSE;
        beh->GetInputParameterValue(5, &scale);

        if (!scale)
        {
            float newdist = posincam.z * (oldsize / desiredsize);

            posincam.z = newdist;
            if (move_object)
                ent->SetPosition(&posincam, cam);
            cam->Transform(&pos, &posincam);

            B[3][0] = pos.x;
            B[3][1] = pos.y;
            B[3][2] = pos.z;
        }
        else
        { // for a sprite 3D, we scale instead of moving
            if (mesh)
            {
                // TODO : change the scale in the matrix
            }
            else
            { // Sprite 3D
                CKSprite3D *sprite = (CKSprite3D *)ent;
                Vx2DVector size;
                sprite->GetSize(size);

                size *= (desiredsize / oldsize);

                sprite->SetSize(size);
            }
        }
    }

    if (!move_object)
        beh->SetOutputParameterValue(0, B);

    return CKBR_OK;
}
