/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		               TT Sky
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorSkyDecl();
CKERROR CreateSkyProto(CKBehaviorPrototype **pproto);
int Sky(const CKBehaviorContext &behcontext);
CKERROR SkyCallBack(const CKBehaviorContext &behcontext);
int SkyPreRenderCallback(CKRenderContext *dev, CKRenderObject *mov, void *arg);
int SkyPostRenderCallback(CKRenderContext *dev, CKRenderObject *mov, void *arg);

typedef struct
{
    float effect;     // Distortion Effect
    VxMatrix projmat; // Projection Matrix
    CK3dEntity *entity;
    CK3dEntity *orientation;
    CKBOOL is_on;
} Effect_ProjMat;

CKObjectDeclaration *FillBehaviorSkyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Sky");
    od->SetDescription("Creates a sky object with any number of faces and changeable material transparency");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x36691920, 0x3b261630));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSkyProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSkyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Sky");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Distortion", CKPGUID_PERCENTAGE, "30");
    proto->DeclareInParameter("Vertex Color", CKPGUID_COLOR, "1, 1, 1, 0");
    proto->DeclareInParameter("Orientation Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "70.0f");
    proto->DeclareInParameter("Quadratic SideFaces?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("or SideFace-Heigth", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Y-Position of Sky", CKPGUID_FLOAT, "0");

    proto->DeclareSetting("Side Materials", CKPGUID_INT, "4");
    proto->DeclareSetting("Top Material", CKPGUID_BOOL, "True");
    proto->DeclareSetting("Bottom Material", CKPGUID_BOOL, "True");

    proto->DeclareLocalParameter("Skyaround", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Sky);

    proto->SetBehaviorCallbackFct(SkyCallBack);

    *pproto = proto;
    return CK_OK;
}

#define DISTORTION 0
#define VERTEX_COLOR 1
#define ORIENTATION_OBJECT 2
#define RADIUS 3
#define QUADRATIC_SIDE_FACES 4
#define SIDE_FACE_HEIGHT 5
#define SKY_POSITION_Y 6

#define SIDE_MATERIALS 0
#define TOP_MATERIAL 1
#define BOTTOM_MATERIAL 2
#define SKY_AROUND 3

int Sky(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    VxColor vtxColor;
    beh->GetInputParameterValue(VERTEX_COLOR, &vtxColor);
    CKDWORD vertexColor = RGBAFTOCOLOR(vtxColor.r, vtxColor.g, vtxColor.b, vtxColor.a);

    int sideMaterials = 0;
    beh->GetLocalParameterValue(SIDE_MATERIALS, &sideMaterials);
    CKBOOL topMaterial = FALSE;
    beh->GetLocalParameterValue(TOP_MATERIAL, &topMaterial);
    CKBOOL bottomMaterial = FALSE;
    beh->GetLocalParameterValue(BOTTOM_MATERIAL, &bottomMaterial);

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        float distortion = 0.3f;
        beh->GetInputParameterValue(DISTORTION, &distortion);
        if (distortion > 1.0f)
            distortion = 1.0f;

        CK3dEntity *orientation = (CK3dEntity *)beh->GetInputParameterObject(ORIENTATION_OBJECT);

        float radius = 70.0f;
        beh->GetInputParameterValue(RADIUS, &radius);

        CKBOOL quadraticSideFaces = TRUE;
        beh->GetInputParameterValue(QUADRATIC_SIDE_FACES, &quadraticSideFaces);

        float sideFaceHeight = 0.0f;
        if (!quadraticSideFaces)
            beh->GetInputParameterValue(SIDE_FACE_HEIGHT, &sideFaceHeight);

        float skyY = 0.0f;
        beh->GetInputParameterValue(SKY_POSITION_Y, &skyY);

        CKMesh *mesh = (CKMesh *)context->CreateObject(CKCID_MESH, "TT_Sky_Mesh",
                                                       CK_OBJECTCREATION_Dynamic(beh->IsDynamic()));
        if (!mesh)
            return CKBR_PARAMETERERROR;
        mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        CK3dEntity *ent = (CK3dEntity *)context->CreateObject(CKCID_3DENTITY, "TT_Sky_Entity",
                                                              CK_OBJECTCREATION_Dynamic(beh->IsDynamic()));
        if (!ent)
            return CKBR_PARAMETERERROR;
        ent->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        int sideVertexCount = 4;
        int sideFaceCount = 2;

        if (topMaterial)
        {
            sideVertexCount += 3;
            sideFaceCount += 1;
        }

        if (bottomMaterial)
        {
            sideVertexCount += 3;
            sideFaceCount += 1;
        }

        if (quadraticSideFaces)
        {
            float s = (2 * PI) / (float)sideMaterials;
            float cr = cosf(s) * radius;
            float sr = sinf(s) * radius;
            sideFaceHeight = sqrtf(sr * sr + (radius - cr) * (radius - cr));
        }

        int vertexCount = sideMaterials * sideVertexCount;
        mesh->SetVertexCount(vertexCount);

        int a = 0;
        int b = 0;
        int c = 0;
        int d = 0;

        VxVector vertex;
        for (int i = 0; i < vertexCount; i += sideVertexCount)
        {
            float s = 2 * PI / (float)sideMaterials;
            float bpi = s * (float)b + 1.25f * PI;
            float api = s * (float)(a + 1) + 1.25f * PI;
            b = a + 1;
            a = b;

            float cb = cosf(bpi) * radius;
            float ca = cosf(api) * radius;

            float sb = sinf(bpi) * radius;
            float sa = sinf(api) * radius;

            float yt = skyY + sideFaceHeight * 0.5f;
            float yb = skyY - sideFaceHeight * 0.5f;

            vertex.x = cb;
            vertex.y = yb;
            vertex.z = sb;
            mesh->SetVertexPosition(i, &vertex);
            mesh->SetVertexTextureCoordinates(i, 1.0f, 1.0f);

            vertex.x = ca;
            vertex.y = yb;
            vertex.z = sa;
            mesh->SetVertexPosition(i + 1, &vertex);
            mesh->SetVertexTextureCoordinates(i + 1, 0.0f, 1.0f);

            vertex.x = ca;
            vertex.y = yt;
            vertex.z = sa;
            mesh->SetVertexPosition(i + 2, &vertex);
            mesh->SetVertexTextureCoordinates(i + 2, 0.0f, 0.0f);

            vertex.x = cb;
            vertex.y = yt;
            vertex.z = sb;
            mesh->SetVertexPosition(i + 3, &vertex);
            mesh->SetVertexTextureCoordinates(i + 3, 1.0f, 0.0f);

            int j = i + 4;

            if (topMaterial)
            {
                const float r = sinf(PI / 4);
                float inv;
                float u, v;

                vertex.x = cb;
                vertex.y = yt;
                vertex.z = sb;
                mesh->SetVertexPosition(j, &vertex);

                inv = 1.0f / sqrtf(sb * sb + cb * cb);
                u = cb * inv * r + 0.5f;
                v = -sb * inv * r + 0.5f;
                mesh->SetVertexTextureCoordinates(j, u, v);

                ++j;

                vertex.x = ca;
                vertex.y = yt;
                vertex.z = sa;
                mesh->SetVertexPosition(j, &vertex);

                inv = 1.0f / sqrtf(sa * sa + ca * ca);
                u = ca * inv * r + 0.5f;
                v = -sa * inv * r + 0.5f;
                mesh->SetVertexTextureCoordinates(j, u, v);

                ++j;

                vertex.x = 0.0;
                vertex.y = yt;
                vertex.z = 0.0;
                mesh->SetVertexPosition(j, &vertex);
                mesh->SetVertexTextureCoordinates(j, 0.5f, 0.5f);

                ++j;
            }

            if (bottomMaterial)
            {
                const float r = sinf(PI / 4);
                float inv;
                float u, v;

                vertex.x = cb;
                vertex.y = yb;
                vertex.z = sb;
                mesh->SetVertexPosition(j, &vertex);

                inv = 1.0f / sqrtf(sb * sb + cb * cb);
                u = cb * inv * r + 0.5f;
                v = -sb * inv * r + 0.5f;
                mesh->SetVertexTextureCoordinates(j, u, v);

                ++j;

                vertex.x = 0.0;
                vertex.y = yb;
                vertex.z = 0.0;
                mesh->SetVertexPosition(j, &vertex);
                mesh->SetVertexTextureCoordinates(j, 0.5f, 0.5f);

                ++j;

                vertex.x = ca;
                vertex.y = yb;
                vertex.z = sa;
                mesh->SetVertexPosition(j, &vertex);

                inv = 1.0f / sqrtf(sa * sa + ca * ca);
                u = ca * inv * r + 0.5f;
                v = -sa * inv * r + 0.5f;
                mesh->SetVertexTextureCoordinates(j, u, v);
            }
        }

        int faceCount = sideMaterials * sideFaceCount;
        mesh->SetFaceCount(faceCount);

        CKMaterial *mat = NULL;
        a = 0;
        b = 1;
        c = 2;
        d = 3;
        for (int i = 0, pos = 7; i < faceCount; i += sideFaceCount, ++pos)
        {
            mat = (CKMaterial *)beh->GetInputParameterObject(pos);

            mesh->SetFaceVertexIndex(i, c, a, b);
            mesh->SetFaceMaterial(i, mat);

            mesh->SetFaceVertexIndex(i + 1, c, d, a);
            mesh->SetFaceMaterial(i + 1, mat);

            a += 4;
            b += 4;
            c += 4;
            d += 4;

            int j = i + 2;

            if (topMaterial)
            {
                mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 7);

                mesh->SetFaceVertexIndex(j, a, b, c);
                mesh->SetFaceMaterial(j, mat);

                a += 3;
                b += 3;
                c += 3;
                d += 3;

                ++j;
            }

            if (bottomMaterial)
            {
                if (topMaterial)
                    mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 8);
                else
                    mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 7);

                mesh->SetFaceVertexIndex(j, a, b, c);
                mesh->SetFaceMaterial(j, mat);

                a += 3;
                b += 3;
                c += 3;
                d += 3;
            }
        }

        mesh->BuildFaceNormals();

        //--- This object must be rendered first
        ent->SetMoveableFlags(ent->GetMoveableFlags() | VX_MOVEABLE_RENDERFIRST);

        //----- Add Object
        behcontext.CurrentLevel->AddObject(ent);

        ent->Show(CKHIDE);
        ent->AddMesh(mesh);
        ent->SetCurrentMesh(mesh);
        ent->SetPickable(FALSE);

        Effect_ProjMat efp;
        efp.effect = distortion;
        efp.entity = ent;
        efp.orientation = orientation;
        efp.is_on = TRUE;
        beh->SetLocalParameterValue(SKY_AROUND, &efp, sizeof(Effect_ProjMat));

        //--------- set flags for read/write zbuf
        CKDWORD flags = ent->GetMoveableFlags();
        ent->SetMoveableFlags(flags | VX_MOVEABLE_NOZBUFFERTEST | VX_MOVEABLE_NOZBUFFERWRITE);

        Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(SKY_AROUND);
        ent->AddPreRenderCallBack(SkyPreRenderCallback, efp_ptr);
        ent->AddPostRenderCallBack(SkyPostRenderCallback, efp_ptr);
    }

    Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(SKY_AROUND);

    CK3dEntity *ent = efp_ptr->entity;
    if (!ent)
        return CKBR_PARAMETERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();

    int faceCount = mesh->GetFaceCount();

    int sideFaceCount = 2;
    if (topMaterial)
        ++sideFaceCount;
    if (bottomMaterial)
        ++sideFaceCount;

    CKMaterial *mat = NULL;
    for (int i = 0, pos = 7; i < faceCount; i += sideFaceCount, ++pos)
    {
        mat = (CKMaterial *)beh->GetInputParameterObject(pos);

        mesh->SetFaceMaterial(i, mat);

        mesh->SetFaceMaterial(i + 1, mat);

        int j = i + 2;

        if (topMaterial)
        {
            mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 7);
            mesh->SetFaceMaterial(j, mat);
            ++j;
        }

        if (bottomMaterial)
        {
            if (topMaterial)
                mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 8);
            else
                mat = (CKMaterial *)beh->GetInputParameterObject(sideMaterials + 7);

            mesh->SetFaceMaterial(j, mat);
        }
    }

    CKDWORD cStride;
    CKBYTE *colp = (CKBYTE *)mesh->GetColorsPtr(&cStride);
    int vertexCount = mesh->GetVertexCount();
    for (int a = 0; a < vertexCount; a++, colp += cStride)
    {
        *(CKDWORD *)colp = vertexColor; // the vertex color is multiplied by the texture
    }
    mesh->ColorChanged();
    mesh->SetLitMode(VX_PRELITMESH);

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);
        ent->Show(CKHIDE);

        return CKBR_OK;
    }

    ent->Show(CKSHOW);

    CK3dEntity *p = behcontext.CurrentRenderContext->GetAttachedCamera();
    if (!p)
        p = behcontext.CurrentRenderContext->GetViewpoint();

    if (!p)
        return CKBR_PARAMETERERROR;

    VxVector pos;
    p->GetPosition(&pos);

    VxMatrix worldMatrix;
    if (efp_ptr->orientation)
        worldMatrix = efp_ptr->orientation->GetWorldMatrix();
    else
        worldMatrix = VxMatrix::Identity();

    ent->SetWorldMatrix(worldMatrix);
    ent->SetPosition(&pos);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR SkyCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDELETE:
    case CKM_BEHAVIORDETACH:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(SKY_AROUND);
        CK3dEntity *ent = efp_ptr->entity;
        if (ent)
        {
            ent->RemovePreRenderCallBack(SkyPreRenderCallback, efp_ptr);
            ent->RemovePostRenderCallBack(SkyPostRenderCallback, efp_ptr);
            CKDestroyObject(ent);
        }

        Effect_ProjMat efp;
        memset(&efp, 0, sizeof(Effect_ProjMat));
        beh->SetLocalParameterValue(SKY_AROUND, &efp, sizeof(Effect_ProjMat));
    }
    break;
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        Effect_ProjMat efp;
        memset(&efp, 0, sizeof(Effect_ProjMat));
        beh->SetLocalParameterValue(SKY_AROUND, &efp, sizeof(Effect_ProjMat));

        int sideMaterials = 0;
        beh->GetLocalParameterValue(SIDE_MATERIALS, &sideMaterials);
        if (sideMaterials < 4)
            sideMaterials = 4;
        beh->SetLocalParameterValue(SIDE_MATERIALS, &sideMaterials);

        CKBOOL topMaterial = FALSE;
        beh->GetLocalParameterValue(TOP_MATERIAL, &topMaterial);

        CKBOOL bottomMaterial = FALSE;
        beh->GetLocalParameterValue(BOTTOM_MATERIAL, &bottomMaterial);

        for (int i = beh->GetInputParameterCount() - 1; i > 6; --i)
        {
            CKParameterIn *pin = beh->RemoveInputParameter(i);
            CKDestroyObject(pin);
        }

        char buffer[256];
        for (int j = 0; j < sideMaterials; ++j)
        {
            sprintf(buffer, "%d.Side-Mat", j);
            beh->CreateInputParameter(buffer, CKPGUID_MATERIAL);
        }

        if (topMaterial)
            beh->CreateInputParameter("Top Mat", CKPGUID_MATERIAL);

        if (bottomMaterial)
            beh->CreateInputParameter("Bottom Mat", CKPGUID_MATERIAL);
    }
    break;
    case CKM_BEHAVIORPAUSE:
    {
        Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(SKY_AROUND);
        CK3dEntity *ent = efp_ptr->entity;
        if (ent)
        {
            ent->RemovePreRenderCallBack(SkyPreRenderCallback, efp_ptr);
            ent->RemovePostRenderCallBack(SkyPostRenderCallback, efp_ptr);
        }
    }
    break;
    case CKM_BEHAVIORRESUME:
    {
        Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(SKY_AROUND);
        CK3dEntity *ent = efp_ptr->entity;
        if (ent)
        {
            if (efp_ptr->is_on)
                ent->AddPreRenderCallBack(SkyPreRenderCallback, efp_ptr);
            ent->AddPostRenderCallBack(SkyPostRenderCallback, efp_ptr);
        }
    }
    break;
    case CKM_BEHAVIORLOAD:
    {
        Effect_ProjMat efp;
        memset(&efp, 0, sizeof(Effect_ProjMat));
        beh->SetLocalParameterValue(SKY_AROUND, &efp, sizeof(Effect_ProjMat));
    }
    break;
    default:
        break;
    }

    return CKBR_OK;
}

int SkyPreRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    Effect_ProjMat *efp = (Effect_ProjMat *)arg;
    if (!efp)
        return CKBR_PARAMETERERROR;

    VxMatrix mat;

    // we change the fov to make it fun !!!
    float near_plane = 1, far_plane = 200;

    VxRect rect;
    dev->GetViewRect(rect);
    float ViewWidth = rect.GetWidth();
    float ViewHeight = rect.GetHeight();

    efp->projmat = dev->GetProjectionTransformationMatrix();

    float current_fov = fabsf(2.0f * atanf(1.0f / efp->projmat[0][0]));
    float fov = current_fov + efp->effect * (PI - current_fov);
    memset(mat, 0, sizeof(VxMatrix));
    float denom = 1.0f / (far_plane - near_plane);
    mat[0][0] = cosf(fov * 0.5f) / sinf(fov * 0.5f);
    mat[1][1] = mat[0][0] * (float)ViewWidth / (float)ViewHeight;
    mat[2][2] = far_plane * denom;
    mat[3][2] = -mat[2][2] * near_plane;
    mat[2][3] = 1;

    dev->SetProjectionTransformationMatrix(mat);

    // we set the position of the cube to the camera
    CK3dEntity *cam = dev->GetAttachedCamera();
    if (!cam)
        cam = dev->GetViewpoint(); // We get the Camera
    if (cam)
        mov->SetPosition(&VxVector::axis0(), cam);

    return 0;
}

int SkyPostRenderCallback(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    Effect_ProjMat *efp = (Effect_ProjMat *)arg;
    dev->SetProjectionTransformationMatrix(efp->projmat);

    return 0;
}