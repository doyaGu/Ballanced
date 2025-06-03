/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SkyAroundCubeMap
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSkyAroundCubeMapDecl();
CKERROR CreateSkyAroundCubeMapProto(CKBehaviorPrototype **);
int SkyAroundCubeMap(const CKBehaviorContext &behcontext);
void A_SkyShow(CK3dEntity *ent, CK_OBJECT_SHOWOPTION b);

CKERROR SkyAroundCubeMapCallBackObject(const CKBehaviorContext &behcontext); // CallBack Functioon

int prerenderCM(CKRenderContext *dev, CKRenderObject *mov, void *arg);  // UnZ-Buffer the rendering of the cube
int postrenderCM(CKRenderContext *dev, CKRenderObject *mov, void *arg); // Z-Buffer the rendering of the cube

#define CUBEVERTEXCOUNT 8

typedef struct
{
    VX_EFFECT BackupEffect;
    float effect;     // Distortion Effect
    VxMatrix projmat; // Projection Matrix
    CK3dEntity *cube; // Entity of the Sky-Cube
    CKBOOL is_on;
    CKBehavior *beh;
} Effect_ProjMat;

CKObjectDeclaration *FillBehaviorSkyAroundCubeMapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sky Around(CubeMap)");
    od->SetDescription("Simulates a Sky Effect on the Scene using a cube map texture on a cube; This behavior doesn't need loop.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>enables the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>disables the behavior.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>texture to use as back texture of the sky-cube.<BR>
    <SPAN CLASS=pin>Right Texture: </SPAN>texture to use as right texture of the sky-cube.<BR>
    <SPAN CLASS=pin>Front Texture: </SPAN>texture to use as front texture of the sky-cube.<BR>
    <SPAN CLASS=pin>Left Texture: </SPAN>texture to use as left texture of the sky-cube.<BR>
    <SPAN CLASS=pin>Up Texture: </SPAN>texture to use as up texture of the sky-cube.<BR>
    <SPAN CLASS=pin>Down Texture: </SPAN>texture to use as down texture of the sky-cube.<BR>
    <BR>
    <SPAN CLASS=pin>Distortion:  Increases the sky field of view, combined with the camera fov.</SPAN><BR>
    <SPAN CLASS=pin>Filtering Color: </SPAN>This is the color by which the texture colors are multiplied... e.g : if you'd like just to see red coponents of the the texture, you should put (255, 0, 0, 128) as filtering color.<BR>
    Note that the alpha component is the fourth member of the filtering color
    <BR>
    <SPAN CLASS=pin>Additional Color: </SPAN>This is the color which is added to the texture color.<BR>
    Use 6 rendered images of the environment, with a FOV of 90 degrees in each direction.<BR>
    [in 3ds Max (90 degrees camera), Bryce (112.5 degrees camera) or Terragen (Zoom = 1) for instance].<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("World Environments/Background");
    od->SetGuid(CKGUID(0x1157f66, 0xaf04fd2));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSkyAroundCubeMapProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSkyAroundCubeMapProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sky Around(CubeMap)");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

#define DIST_P   1
#define DISTO_P  2
#define FCOLOR_P 3
#define ACOLOR_P 4

    proto->DeclareInParameter("Cube Map Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Distance", CKPGUID_FLOAT, "1.0f");
    proto->DeclareInParameter("Distortion", CKPGUID_PERCENTAGE, "20");
    proto->DeclareInParameter("Filtering Color", CKPGUID_COLOR, "255, 255, 255, 128");
    proto->DeclareInParameter("Additional Color", CKPGUID_COLOR, "0, 0, 0, 0");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); //"Struct Effect ProjMat"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SkyAroundCubeMap);

    proto->SetBehaviorCallbackFct(SkyAroundCubeMapCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int SkyAroundCubeMap(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    Effect_ProjMat *efp = (Effect_ProjMat *)beh->GetLocalParameterWriteDataPtr(0);
    efp->effect = 0.2f;
    beh->GetInputParameterValue(DISTO_P, &(efp->effect));
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
        CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(0);

        mesh->ReplaceMaterial(NULL, mat);
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
        efp->is_on = TRUE;
    }

    efp->beh = beh;
    if (efp->is_on)
        A_SkyShow(ent, CKSHOW);

    // we change the color of the vertex
    VxColor col(1.0f, 1.0f, 1.0f, 0.5f);
    beh->GetInputParameterValue(FCOLOR_P, &col); // we get the filtering color
    CKDWORD dcol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    col.r = 0;
    col.g = 0;
    col.b = 0;
    col.a = 0;
    beh->GetInputParameterValue(ACOLOR_P, &col); // we get the additionnal color
    CKDWORD dspeccol = RGBAFTOCOLOR(col.r, col.g, col.b, col.a);

    CKDWORD cStride, sStride;
    CKBYTE *colp = (CKBYTE *)mesh->GetColorsPtr(&cStride);
    CKBYTE *spcp = (CKBYTE *)mesh->GetSpecularColorsPtr(&sStride);
    for (int a = 0; a < CUBEVERTEXCOUNT; a++, colp += cStride, spcp += sStride)
    {
        *(CKDWORD *)colp = dcol;     // the vertex color is multiplied by the texture
        *(CKDWORD *)spcp = dspeccol; // the spec. is added to the global color
    }
    mesh->ColorChanged();

    float Distance = 1.0f;
    beh->GetInputParameterValue(DIST_P, &Distance);
    VxVector Scale(Distance, Distance, Distance);
    ent->SetScale(&Scale, TRUE);

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

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SkyAroundCubeMapCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKMesh *mesh = (CKMesh *)ctx->CreateObject(CKCID_MESH, "SkyAroundCubeMap-cube", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        if (!mesh)
            return CKBR_PARAMETERERROR;

        mesh->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        //--------------------------------------  Vertices
        int a;
        float s = 10.0f; // length by default (at top)
        VxVector V;

        mesh->SetVertexCount(CUBEVERTEXCOUNT);

#define PUTVERTEX(i, a, b, c) \
    V.x = a;                  \
    V.y = b;                  \
    V.z = c;                  \
    mesh->SetVertexPosition(i, &V)

        PUTVERTEX(0, -s, -s, -s);
        PUTVERTEX(1, -s, s, -s);
        PUTVERTEX(2, s, s, -s);
        PUTVERTEX(3, s, -s, -s);
        PUTVERTEX(4, -s, s, s);
        PUTVERTEX(5, -s, -s, s);
        PUTVERTEX(6, s, s, s);
        PUTVERTEX(7, s, -s, s);

        mesh->SetLitMode((VXMESH_LITMODE)VX_PRELITMESH);

        for (a = 0; a < CUBEVERTEXCOUNT; a++)
        {
            mesh->SetVertexColor(a, RGBAITOCOLOR(255, 255, 255, 255));
            mesh->SetVertexSpecularColor(a, RGBAITOCOLOR(0, 0, 0, 255));
        }

        //--------------------------------------  Faces
        mesh->SetFaceCount(12);

        mesh->SetFaceVertexIndex(0, 2, 1, 0); // BACK
        mesh->SetFaceVertexIndex(1, 2, 0, 3);

        mesh->SetFaceVertexIndex(2, 6, 2, 3); // RIGHT
        mesh->SetFaceVertexIndex(3, 6, 3, 7);

        mesh->SetFaceVertexIndex(4, 4, 6, 7); // FRONT
        mesh->SetFaceVertexIndex(5, 4, 7, 5);

        mesh->SetFaceVertexIndex(6, 1, 4, 5); // LEFT
        mesh->SetFaceVertexIndex(7, 1, 5, 0);

        mesh->SetFaceVertexIndex(8, 1, 2, 6); // UP
        mesh->SetFaceVertexIndex(9, 1, 6, 4);

        mesh->SetFaceVertexIndex(10, 5, 7, 3); // DOWN
        mesh->SetFaceVertexIndex(11, 5, 3, 0);

        //--------------------------------------  Normals
        mesh->BuildFaceNormals();

        //--------------------------------------  Material
        for (a = 0; a < 12; a++)
        {
            mesh->SetFaceMaterial(a, NULL);
        }

        //--------------------------------------  Entity
        CK3dEntity *ent = (CK3dEntity *)ctx->CreateObject(CKCID_3DENTITY, "SkyAroundCubeMap", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
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
        efp_ptr->beh = beh;

        //--------------------------------------  Pre-Render CallBack
        ent->AddPreRenderCallBack(prerenderCM, efp_ptr);

        //--------------------------------------  postrenderCM CallBack
        ent->AddPostRenderCallBack(postrenderCM, efp_ptr);

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
int prerenderCM(CKRenderContext *dev, CKRenderObject *obj, void *arg)
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

    CKMaterial *Mat = (CKMaterial *)efp->beh->GetInputParameterObject(0);
    if (Mat)
    {
        efp->BackupEffect = Mat->GetEffect();
        Mat->SetEffect(VXEFFECT_TEXGEN);
        VX_EFFECTTEXGEN TexGen = VXEFFECT_TGCUBEMAP_SKYMAP;
        Mat->GetEffectParameter()->SetValue(&TexGen);
    }

    return 0;
}

/***********************************************************/
/****************** POST-RENDER CALLBACK *******************/
int postrenderCM(CKRenderContext *dev, CKRenderObject *obj, void *arg)
{
    CK3dEntity *mov = (CK3dEntity *)obj;
    Effect_ProjMat *efp = (Effect_ProjMat *)arg;

    dev->SetProjectionTransformationMatrix(efp->projmat);

    CKMaterial *Mat = (CKMaterial *)efp->beh->GetInputParameterObject(0);
    if (Mat)
        Mat->SetEffect(efp->BackupEffect);

    return 0;
}
