////////////////////////////////
////////////////////////////////
//
//        TT SkyAround
//
////////////////////////////////
////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSkyAroundDecl();
CKERROR CreateSkyAroundProto(CKBehaviorPrototype **);
int SkyAround(const CKBehaviorContext &behcontext);

CKERROR SkyAroundCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

int prerender(CKRenderContext *dev, CKRenderObject *mov, void *arg);  // UnZ-Buffer the rendering of the cube
int postrender(CKRenderContext *dev, CKRenderObject *mov, void *arg); // Z-Buffer the rendering of the cube

void A_SkyShow(CK3dEntity *ent, CK_OBJECT_SHOWOPTION b)
{
    if ((b == CKHIDE) == (ent->IsVisible()))
        ent->Show(b);
    CKContext *ctx = ent->GetCKContext();
    CKRenderContext *rc = ctx->GetPlayerRenderContext();
    if (rc)
        rc->SetClearBackground(!b);
}

typedef struct
{
    float effect;     // Distortion Effect
    VxMatrix projmat; // Projection Matrix
    CK3dEntity *cube; // Entity of the Sky-Cube
    CKBOOL is_on;
} Effect_ProjMat;

CKObjectDeclaration *FillBehaviorSkyAroundDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SkyAround");
    od->SetDescription("Creates a skyaround object with any number of faces");
    od->SetCategory("TT Toolbox/Object");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2C7D5826, 0x688778F4));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSkyAroundProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSkyAroundProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SkyAround");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Distortion", CKPGUID_PERCENTAGE, "20");
    proto->DeclareInParameter("Filtering Color", CKPGUID_COLOR, "255, 255, 255, 128");
    proto->DeclareInParameter("Additional Color", CKPGUID_COLOR, "0, 0, 0, 0");
    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "One");
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR, "Zero");
    proto->DeclareInParameter("Orientation Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "70.0f");
    proto->DeclareInParameter("Quadratic SideFaces?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("SideFace-Heigth", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Y-Position of Sky", CKPGUID_FLOAT, "0");

    proto->DeclareLocalParameter("Skyaround", CKPGUID_VOIDBUF);
    proto->DeclareSetting("Side Materials", CKPGUID_INT, "4");
    proto->DeclareSetting("Top Materials", CKPGUID_BOOL, "True");
    proto->DeclareSetting("Bottom Materials", CKPGUID_BOOL, "True");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SkyAround);

    proto->SetBehaviorCallbackFct(SkyAroundCallBack);

    *pproto = proto;
    return CK_OK;
}

int SkyAround(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterWriteDataPtr(0);
    efp->effect = 0.2f;
    beh->GetInputParameterValue(6, &(efp->effect));
    if (efp->effect > 1.0f)
        efp->effect = 1.0f;

    // We get the cube
    CK3dEntity *ent = efp->cube;
    if (!ent)
        return CKBR_PARAMETERERROR;

    if (beh->IsInputActive(1))
    { // we enter by 'OFF'	 ( beware the order between the 'OFF' statement and the 'ON' statement is very important)
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);

        A_SkyShow(ent, CKHIDE);
        efp->is_on = FALSE;
        return CKBR_OK;
    }

    CKMesh *mesh = ent->GetCurrentMesh();

    if (beh->IsInputActive(0))
    { // we enter by 'ON'

        CKTexture *texture;
        CKMaterial *mat;
        // We put the textures
        for (int a = 0; a < 6; a++)
        {
            texture = (CKTexture *)beh->GetInputParameterObject(a);
            mat = mesh->GetFaceMaterial(a * 2);

            mat->SetTexture0(texture);
        }
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
        efp->is_on = TRUE;
    }

    if (efp->is_on)
        A_SkyShow(ent, CKSHOW);

    // we change the color of the vertex
    VxColor col(1.0f, 1.0f, 1.0f, 0.5f);
    beh->GetInputParameterValue(7, &col); // we get the filtering color
    CKDWORD dcol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    col.r = 0;
    col.g = 0;
    col.b = 0;
    col.a = 0;
    beh->GetInputParameterValue(8, &col); // we get the additionnal color
    CKDWORD dspeccol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    CKDWORD cStride, sStride;
    CKBYTE *colp = (CKBYTE *)mesh->GetColorsPtr(&cStride);
    CKBYTE *spcp = (CKBYTE *)mesh->GetSpecularColorsPtr(&sStride);
    for (int a = 0; a < 24; a++, colp += cStride, spcp += sStride)
    {
        *(CKDWORD *)colp = dcol;     // the vertex color is multiplied by the texture
        *(CKDWORD *)spcp = dspeccol; // the spec. is added to the global color
    }
    mesh->ColorChanged();

    CK3dEntity *cam = behcontext.CurrentRenderContext->GetAttachedCamera();
    if (!cam)
        cam = behcontext.CurrentRenderContext->GetViewpoint(); // We get the Camera
    if (!cam)
        return CKBR_PARAMETERERROR;

    // we set the position of the cube to the position of the viewpoint.
    VxVector pos_cam;
    cam->GetPosition(&pos_cam, NULL);

    ent->SetPosition3f(0.0f, 0.0f, 0.0f, cam);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR SkyAroundCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKMesh *mesh = (CKMesh *)ctx->CreateObject(CKCID_MESH, "SkyAround-cube", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        if (!mesh)
            return CKBR_PARAMETERERROR;

        mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        //--------------------------------------  Vertices
        int a;
        float s = 10.0f; // length by default (at top)
        VxVector V;

        mesh->SetVertexCount(24);

#define PUTVERTEX(i, a, b, c) \
    V.x = a;                  \
    V.y = b;                  \
    V.z = c;                  \
    mesh->SetVertexPosition(i, &V)

        PUTVERTEX(0, -s, -s, -s);
        PUTVERTEX(1, -s, s, -s);
        PUTVERTEX(2, s, s, -s);
        PUTVERTEX(3, s, -s, -s);

        PUTVERTEX(4, s, -s, -s);
        PUTVERTEX(5, s, s, -s);
        PUTVERTEX(6, s, s, s);
        PUTVERTEX(7, s, -s, s);

        PUTVERTEX(8, s, -s, s);
        PUTVERTEX(9, s, s, s);
        PUTVERTEX(10, -s, s, s);
        PUTVERTEX(11, -s, -s, s);

        PUTVERTEX(12, -s, -s, s);
        PUTVERTEX(13, -s, s, s);
        PUTVERTEX(14, -s, s, -s);
        PUTVERTEX(15, -s, -s, -s);

        PUTVERTEX(16, s, s, s);
        PUTVERTEX(17, s, s, -s);
        PUTVERTEX(18, -s, s, -s);
        PUTVERTEX(19, -s, s, s);

        PUTVERTEX(20, s, -s, -s);
        PUTVERTEX(21, s, -s, s);
        PUTVERTEX(22, -s, -s, s);
        PUTVERTEX(23, -s, -s, -s);

        mesh->SetLitMode((VXMESH_LITMODE)VX_PRELITMESH);

        for (a = 0; a < 24; a++)
        {
            mesh->SetVertexColor(a, RGBAITOCOLOR(255, 255, 255, 255));
            mesh->SetVertexSpecularColor(a, RGBAITOCOLOR(0, 0, 0, 255));
        }

        for (a = 0; a < 24; a += 4)
        {
            mesh->SetVertexTextureCoordinates(a, 1.0f, 1.0f);
            mesh->SetVertexTextureCoordinates(a + 1, 1.0f, 0.0f);
            mesh->SetVertexTextureCoordinates(a + 2, 0.0f, 0.0f);
            mesh->SetVertexTextureCoordinates(a + 3, 0.0f, 1.0f);
        }

        //--------------------------------------  Faces
        mesh->SetFaceCount(12);

        mesh->SetFaceVertexIndex(0, 2, 1, 0); // BACK
        mesh->SetFaceVertexIndex(1, 2, 0, 3);

        mesh->SetFaceVertexIndex(2, 6, 5, 4); // RIGHT
        mesh->SetFaceVertexIndex(3, 6, 4, 7);

        mesh->SetFaceVertexIndex(4, 10, 9, 8); // FRONT
        mesh->SetFaceVertexIndex(5, 10, 8, 11);

        mesh->SetFaceVertexIndex(6, 14, 13, 12); // LEFT
        mesh->SetFaceVertexIndex(7, 14, 12, 15);

        mesh->SetFaceVertexIndex(8, 18, 17, 16); // UP
        mesh->SetFaceVertexIndex(9, 18, 16, 19);

        mesh->SetFaceVertexIndex(10, 22, 21, 20); // DOWN
        mesh->SetFaceVertexIndex(11, 22, 20, 23);

        //--------------------------------------  Normals
        mesh->BuildFaceNormals();

        //--------------------------------------  Material
        CKTexture *texture;
        char str[128];
        for (a = 0; a < 6; a++)
        {
            sprintf(str, "Sky-Cube Material %d", a);

            CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, str, beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
            if (!mat)
                continue;

            mat->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

            mesh->SetFaceMaterial(a * 2, mat);
            mesh->SetFaceMaterial(a * 2 + 1, mat);

            texture = (CKTexture *)beh->GetInputParameterObject(a);
            mat->SetTexture0(texture);
            mat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        }

        //--------------------------------------  Entity
        CK3dEntity *ent = (CK3dEntity *)ctx->CreateObject(CKCID_3DENTITY, "SkyAround", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        if (!ent)
            return CKBR_PARAMETERERROR;

        ent->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        //--- This object must be rendered first
        ent->SetMoveableFlags(ent->GetMoveableFlags() | VX_MOVEABLE_RENDERFIRST);

        //----- Add Object
        behcontext.CurrentLevel->AddObject(ent);

        A_SkyShow(ent, CKHIDE);

        ent->AddMesh(mesh);
        ent->SetCurrentMesh(mesh);

        Effect_ProjMat efp; // we initialize the size of the local structure
        efp.cube = ent;     // and we put the cube in the local structure
        efp.is_on = FALSE;  // we tell that the cube is not visible by default
        beh->SetLocalParameterValue(0, &efp, sizeof(Effect_ProjMat));
        Effect_ProjMat *efp_ptr = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(0);

        //--------------------------------------  Pre-Render CallBack
        ent->AddPreRenderCallBack(prerender, efp_ptr);

        //--------------------------------------  PostRender CallBack
        ent->AddPostRenderCallBack(postrender, efp_ptr);

        //--------------------------------------  Pickable or not
        ent->SetPickable(FALSE);

        //--------- set flags for read/write zbuf
        CKDWORD flags = ent->GetMoveableFlags();

        CKBOOL enable_read = FALSE;
        CKBOOL enable_write = FALSE;

        if (enable_read)
            flags &= ~VX_MOVEABLE_NOZBUFFERTEST;
        else
            flags |= VX_MOVEABLE_NOZBUFFERTEST;

        if (enable_write)
            flags &= ~VX_MOVEABLE_NOZBUFFERWRITE;
        else
            flags |= VX_MOVEABLE_NOZBUFFERWRITE;

        ent->SetMoveableFlags(flags);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterWriteDataPtr(0);
        if (!(efp->cube))
            return CKBR_PARAMETERERROR;
        CKDestroyObject(efp->cube);
    }
    break;

    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(0);
        if (!(efp->cube))
            return CKBR_PARAMETERERROR;

        A_SkyShow(efp->cube, CKHIDE);
    }
    break;

    case CKM_BEHAVIORRESUME:
    {
        int a = 0;
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(0);
        if (!(efp->cube))
            return CKBR_PARAMETERERROR;

        // remove the object from all scene it was in
        for (int i = 0; i < efp->cube->GetSceneInCount(); i++)
        {
            CKScene *scene = efp->cube->GetSceneIn(i);
            if (scene)
                scene->RemoveObjectFromScene(efp->cube);
        }

        // if script actif in scene
        // or if the owner is in the scene
        // or if the owner is the level,    then add the object to the new scene.
        CKBeObject *owner = beh->GetOwner();
        CKScene *scene = behcontext.CurrentScene;
        if (owner->IsInScene(scene) || CKIsChildClassOf(owner, CKCID_LEVEL))
        {
            scene->AddObjectToScene(efp->cube);
            if (!(beh->IsParentScriptActiveInScene(scene)))
            {
                A_SkyShow(efp->cube, CKHIDE);
            }
        }
    }
    break;

    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(0);
        if (!(efp->cube))
            return CKBR_PARAMETERERROR;
        A_SkyShow(efp->cube, CKSHOW);
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterReadDataPtr(0);
        if (!(efp->cube))
            return CKBR_PARAMETERERROR;
        A_SkyShow(efp->cube, CKHIDE);
    }
    break;
    }

    return CKBR_OK;
}

/***********************************************************/
/****************** PRE-RENDER CALLBACK ********************/
int prerender(CKRenderContext *dev, CKRenderObject *obj, void *arg)
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

    float current_fov = (float)fabs(2.0f * (float)atanf(1.0f / efp->projmat[0][0]));
    float fov = (float)(current_fov + efp->effect * (3.141592654 - current_fov));
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

/***********************************************************/
/****************** POST-RENDER CALLBACK *******************/
int postrender(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{

    CK3dEntity *mov = (CK3dEntity *)obj;

    Effect_ProjMat *efp = (Effect_ProjMat *)arg;

    dev->SetProjectionTransformationMatrix(efp->projmat);

    return 0;
}
