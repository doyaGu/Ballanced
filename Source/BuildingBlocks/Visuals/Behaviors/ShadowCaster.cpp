/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              ShadowCaster
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define PlanarProjector 0
#define PerspectiveProjector 1
#define PsycheProjector 2
#define SphericalProjector 3

char *materialname = "SC Material";

CKObjectDeclaration *FillBehaviorShadowCasterDecl();
CKERROR CreateShadowCasterProto(CKBehaviorPrototype **);
int ShadowCaster(const CKBehaviorContext &behcontext);
CKERROR ShadowCasterCallBackObject(const CKBehaviorContext &behcontext);

typedef struct
{
    CKCamera *m_Camera;
    CKTexture *m_ShadowTexture;
    CKMaterial *m_Material;
    int m_ReceiverAttribute;
} ShadowCasterStruct;

CKObjectDeclaration *FillBehaviorShadowCasterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("ShadowCaster");
    od->SetDescription("Projects a shadow of the target object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Light: </SPAN>Light from which the shadow must be casted. If the light is directionnal, the projection
    will be orthogonal, else it will be in perspective.<BR>
    <SPAN CLASS=pin>Min Light Distance: </SPAN>distance beneath which the shadow is fully black. Between this min and themax distance,
    the shadow continuously fades to transparent.<BR>
    <SPAN CLASS=pin>Max Light Distance: </SPAN>distance beyond which no more shadow is projected.<BR>
    <SPAN CLASS=pin>Soft Shadow: </SPAN>apply a smoothing to the shadow to simulate a soft shadow.<BR>
    <BR>
    <SPAN CLASS=setting>Shadow Resolution Size: </SPAN>size of the texture in which the shadow will be rendered. The greater the sharperr the shadow will be.<BR>
    <BR>
    The target object and all its hierarchy is rendered into the shadow, so you can use this building block on a character.<BR>
    Objects that must receive the shadow must have the <B>Visuals FX/Shadow Caster Receiver</B> attribute.
    For a good effect, these objects need to be well tesselated.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x718360b, 0x71e5002f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShadowCasterProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/Shadows");
    return od;
}

CKERROR CreateShadowCasterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("ShadowCaster");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Min Light Distance", CKPGUID_FLOAT, "5.0f");
    proto->DeclareInParameter("Max Light Distance", CKPGUID_FLOAT, "50.0f");
    proto->DeclareInParameter("Soft Shadow", CKPGUID_BOOL, "FALSE");

    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);
    proto->DeclareSetting("Shadow Resolution Size", CKPGUID_INT, "128");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShadowCaster);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(ShadowCasterCallBackObject);

    *pproto = proto;
    return CK_OK;
}
#define MaterialMemory ShadowCasterMaterialMemory
struct MaterialMemory
{
    VxColor diffuse;
    VxColor emissive;
    VxColor ambient;
};

void BlackFunction(CKRenderContext *rc, CK3dEntity *Mov, CKMesh *Object, CKMaterial *mat, void *Argument)
{
    MaterialMemory *mm = (MaterialMemory *)Argument;
    static VxColor blackColor(0.0f, 0.0f, 0.0f, 1.0f);
    ;

    if (mat)
    {
        mm->diffuse = mat->GetDiffuse();
        mm->emissive = mat->GetEmissive();
        mm->ambient = mat->GetAmbient();

        mat->SetDiffuse(blackColor);
        mat->SetEmissive(blackColor);
        mat->SetAmbient(blackColor);

        mat->SetAsCurrent(rc);
    }

    /*
    rc->SetState(VXRENDERSTATE_SRCBLEND,			VXBLEND_SRCALPHA);
    rc->SetState(VXRENDERSTATE_DESTBLEND,			VXBLEND_ZERO);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE,	TRUE);
    */

    /*	rc->SetState(VXRENDERSTATE_ALPHATESTENABLE,		0);
        rc->SetState(VXRENDERSTATE_WRAP0 ,				0);
        rc->SetState(VXRENDERSTATE_CULLMODE,			VXCULL_CW);

        rc->SetState(VXRENDERSTATE_SRCBLEND,			VXBLEND_ONE);
        rc->SetState(VXRENDERSTATE_DESTBLEND,			VXBLEND_ZERO);
        rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE,	FALSE);
        rc->SetState(VXRENDERSTATE_ZENABLE ,			FALSE);
        rc->SetState(VXRENDERSTATE_ZWRITEENABLE ,		FALSE);
        */
}

void BlackRestoreFunction(CKRenderContext *Dev, CK3dEntity *Mov, CKMesh *Object, CKMaterial *mat, void *Argument)
{
    MaterialMemory *mm = (MaterialMemory *)Argument;

    if (mat)
    {
        mat->SetDiffuse(mm->diffuse);
        mat->SetEmissive(mm->emissive);
        mat->SetAmbient(mm->ambient);
    }
}

void RenderMeshPrimitives(CKRenderContext *dev, CK3dEntity *ent, CKMesh *mesh)
{
    MaterialMemory materialMem;

    mesh->AddSubMeshPreRenderCallBack(BlackFunction, &materialMem);
    mesh->AddSubMeshPostRenderCallBack(BlackRestoreFunction, &materialMem);

    mesh->Render(dev, ent);

    mesh->RemoveSubMeshPreRenderCallBack(BlackFunction, &materialMem);
    mesh->RemoveSubMeshPostRenderCallBack(BlackRestoreFunction, &materialMem);
}

void HierarchicalRendering(CK3dEntity *ent, CKRenderContext *dev)
{
    if (ent->IsHierarchicallyHide())
        return;

    if (ent->IsVisible())
    {
        // rendering itself
        CKMesh *mesh = ent->GetCurrentMesh();
        if (mesh && mesh->IsVisible())
        {
            dev->SetWorldTransformationMatrix(ent->GetWorldMatrix());
            RenderMeshPrimitives(dev, ent, mesh);
        }
    }

    // rendering the children
    for (int i = 0; i < ent->GetChildrenCount(); ++i)
    {
        HierarchicalRendering(ent->GetChild(i), dev);
    }
}

void DrawFillRectangleGradient(CKRenderContext *dev, VxRect &rect, const VxColor &start, const VxColor &end)
{
    dev->SetTexture(NULL);
    dev->SetState(VXRENDERSTATE_ALPHATESTENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_FOGENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZFUNC, VXCMP_ALWAYS);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
    dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ONE);

    VxDrawPrimitiveData *data;
    data = dev->GetDrawPrimitiveStructure(CKRST_DP_CL_VC, 4);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
    VxVector *normals = (VxVector *)data->NormalPtr;
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
#else
    VxVector4 *positions = (VxVector4 *)data->Positions.Ptr;
    VxVector *normals = (VxVector *)data->Normals.Ptr;
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
#endif

    /////////////////
    // Colors

    CKDWORD col = RGBAFTOCOLOR(&start);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxFillStructure(2, colors, data->ColorStride, sizeof(CKDWORD), &col);
    col = RGBAFTOCOLOR(&end);
    VxFillStructure(2, (CKBYTE *)colors + 2 * data->ColorStride, data->ColorStride, sizeof(CKDWORD), &col);
#else
    VxFillStructure(2, colors, data->Colors.Stride, sizeof(CKDWORD), &col);
    col = RGBAFTOCOLOR(&end);
    VxFillStructure(2, (CKBYTE *)colors + 2 * data->Colors.Stride, data->Colors.Stride, sizeof(CKDWORD), &col);
#endif
    /////////////////
    // Positions

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    const unsigned int positionStride = data->PositionStride;
#else
    const unsigned int positionStride = data->Positions.Stride;
#endif

    // Vertex 0
    positions->x = rect.left;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + positionStride);
    // Vertex 1
    positions->x = rect.right;
    positions->y = rect.top;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + positionStride);
    // Vertex 2
    positions->x = rect.right;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + positionStride);
    // Vertex 3
    positions->x = rect.left;
    positions->y = rect.bottom;
    positions->z = 0.0f;
    positions->w = 1.0f;
    positions = (VxVector4 *)((CKBYTE *)positions + positionStride);

    // Indices
    CKWORD *indices = dev->GetDrawPrimitiveIndices(4);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    // the drawing itself
    dev->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, data);

    if (dev->GetFogMode() != VXFOG_NONE)
        dev->SetState(VXRENDERSTATE_FOGENABLE, FALSE);
}

CKBOOL RenderObjectsToTexture(CKContext *ctx, CK3dEntity *ent, CKRenderContext *rc, CKTexture *tex, CK3dEntity *light, CKCamera *cam, float lmin, float lmax, float &minz, CKBOOL softshadow, float *retrievedMaxx, float *retrievedMaxy)
{
    //
    // storing old render context values

    CKMaterial *BackgroundMat = rc->GetBackgroundMaterial();
    CKCamera *oldcam = rc->GetAttachedCamera();		 // we store the old camera
    CKTexture *oldtex = BackgroundMat->GetTexture(); // we store the old texture
    VxColor oldcol = BackgroundMat->GetDiffuse();

    ///////////////////////////////
    // Object Light interactions

    const VxBbox &entbox = ent->GetHierarchicalBox();
    VxVector centerbox = entbox.GetCenter();

    VxVector lightpos;
    light->GetPosition(&lightpos);

    cam->SetPosition(&lightpos);
    cam->LookAt(&centerbox);

    lightpos -= centerbox;
    float distance = Magnitude(lightpos);
    lightpos += centerbox;

    VxBbox localbox;
    localbox.TransformFrom(const_cast<VxBbox &>(entbox), cam->GetInverseWorldMatrix());

    // the light is considered too far away
    CKBOOL render = TRUE;
    if (distance >= lmax)
    {
        render = FALSE;
        distance = lmax;
    }

    float delta = (distance - lmin) / (lmax - lmin);
    if (delta < 0.0f)
        delta = 0.0f;
    else if (delta > 1.0f)
        delta = 1.0f;

    // We set the background color
    VxColor shadowColor(1.0f, 1.0f, 1.0f, 1.0f);
    BackgroundMat->SetDiffuse(shadowColor);
    BackgroundMat->SetTexture0(NULL);

    // the color of the shadow
    shadowColor *= delta;

    const VxMatrix &invcam = cam->GetInverseWorldMatrix();

    VxVector pts[8];
    entbox.TransformTo(pts, invcam);

    VxVector tcenter;
    Vx3DMultiplyMatrixVector(&tcenter, invcam, &centerbox);

    // we find the maxima of the transformed points
    float minx = pts[0].x, maxx = pts[0].x, miny = pts[0].y, maxy = pts[0].y, maxz = pts[0].z;
    minz = pts[0].z;
    for (int i = 1; i < 8; i++)
    {
        if (pts[i].x < minx)
            minx = pts[i].x;
        else if (pts[i].x > maxx)
            maxx = pts[i].x;
        if (pts[i].y < miny)
            miny = pts[i].y;
        else if (pts[i].y > maxy)
            maxy = pts[i].y;
        if (pts[i].z < minz)
            minz = pts[i].z;
        else if (pts[i].z > maxz)
            maxz = pts[i].z;
    }
    maxx -= minx;
    maxy -= miny;

    *retrievedMaxx = maxx;
    *retrievedMaxy = maxy;

    float rapport;
    float tanfov;
    if (maxx > maxy)
    {
        rapport = maxx * 0.5f;
    }
    else
    {
        rapport = maxy * 0.5f;
    }
    float fov = cam->GetFov();
    tanfov = rapport / tanf(fov * 0.5f);

    cam->Translate3f(0, 0, -(tanfov - minz), cam);

    rc->AttachViewpointToCamera(cam);

    if (rc->SetRenderTarget(tex))
    {
        // We prepare the cameras
        CKDWORD RenderOptions = rc->GetCurrentRenderOptions();
        RenderOptions &= ~(CK_RENDER_USECAMERARATIO | CK_RENDER_DOBACKTOFRONT);
        rc->PrepareCameras((CK_RENDER_FLAGS)RenderOptions);

        // we Clear the back buffer
        rc->Clear((CK_RENDER_FLAGS)(CK_RENDER_CLEARBACK | CK_RENDER_CLEARZ | CK_RENDER_CLEARVIEWPORT));

        if (render)
        {
            // Here Comes the rendering
            rc->SetCurrentMaterial(NULL);
            rc->SetState(VXRENDERSTATE_ALPHATESTENABLE, 0);
            rc->SetState(VXRENDERSTATE_WRAP0, 0);
            rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_CW);

            rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
            rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ZERO);
            rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, FALSE);
            rc->SetState(VXRENDERSTATE_ZENABLE, FALSE);
            rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);

            ///
            // Saving Matrix
            VxMatrix oldViewMat = rc->GetViewTransformationMatrix();
            VxMatrix oldProjMat = rc->GetProjectionTransformationMatrix();
            VxMatrix oldWordMatrix = rc->GetWorldTransformationMatrix();

            rc->SetViewTransformationMatrix(cam->GetInverseWorldMatrix());
            VxMatrix projmat;
            cam->ComputeProjectionMatrix(projmat);
            rc->SetProjectionTransformationMatrix(projmat);

            // the hierarchical rendering
            HierarchicalRendering(ent, rc);

            VxRect screen(0.0f, 0.0f, (float)tex->GetWidth(), (float)tex->GetHeight());
            DrawFillRectangleGradient(rc, screen, shadowColor, shadowColor);

            if (softshadow)
            { // we draw a gradient rectangle from white to black, top to bottom
                VxColor white(1.0f, 1.0f, 1.0f, 1.0f);
                VxColor black(0.2f, 0.2f, 0.2f, 1.0f);
                DrawFillRectangleGradient(rc, screen, white, black);
            }

            ///
            // we're restoring matrices the transfo mat
            rc->SetWorldTransformationMatrix(oldWordMatrix);
            rc->SetProjectionTransformationMatrix(oldProjMat);
            rc->SetViewTransformationMatrix(oldViewMat);

            rc->SetState(VXRENDERSTATE_ZENABLE, TRUE);
        }

        // Some rasterizer will skip this phase as they directly
        // render to the texture but others (OpenGl) may need
        // to update the content of the texture at this moment
        rc->BackToFront((CK_RENDER_FLAGS)RenderOptions);

        rc->SetRenderTarget(NULL);
    }

    // we restore the old render context values
    rc->AttachViewpointToCamera(oldcam);
    // We need to prepare the cameras...
    CKDWORD RenderOptions = rc->GetCurrentRenderOptions();
    rc->PrepareCameras((CK_RENDER_FLAGS)RenderOptions);
    BackgroundMat->SetTexture0(oldtex);
    BackgroundMat->SetDiffuse(oldcol);

    minz = maxz;
    return render;
}

int ShadowCaster(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKRenderContext *rcontext = behcontext.CurrentRenderContext;

    ShadowCasterStruct *tss = (ShadowCasterStruct *)beh->GetLocalParameterReadDataPtr(0);
    CKMaterial *cmat = (CKMaterial *)tss->m_Material;

    CKAttributeManager *attman = behcontext.AttributeManager;
    const XObjectPointerArray &oArray = attman->GetAttributeListPtr(tss->m_ReceiverAttribute);

    if (beh->IsInputActive(1))
    { // We enter by off
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);

        // we remove the channels
        for (CKObject **it = oArray.Begin(); it != oArray.End(); ++it)
        {
            CK3dEntity *o = (CK3dEntity *)*it;
            if (!o)
                continue;

            CKMesh *mesh = (CKMesh *)o->GetCurrentMesh();
            if (!mesh)
                continue;

            mesh->RemoveChannelByMaterial(cmat);
        }

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // We enter by on
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    // No receivers, we skip the whole thing
    if (!oArray.Size())
        return CKBR_ACTIVATENEXTFRAME;

    float fov = 0.2f;
    tss->m_Camera->SetFov(fov);
    float tanfov = tanf(fov * 0.5f);

    // READING INPUTS
    // we get the input texture id
    CKLight *light = (CKLight *)beh->GetInputParameterObject(0);
    if (!light)
        return CKBR_ACTIVATENEXTFRAME;

    CKBOOL perspective = TRUE;
    if (light->GetType() == VX_LIGHTDIREC)
        perspective = FALSE;

    VxVector barycenter;
    ent->GetBaryCenter(&barycenter);

    // Light ranges
    float lmin = 5.0f;
    beh->GetInputParameterValue(1, &lmin);
    float lmax = 50.0f;
    beh->GetInputParameterValue(2, &lmax);
    CKBOOL softshadow = FALSE;
    beh->GetInputParameterValue(3, &softshadow);

    // Rendering objects to shadow texture
    float minz;
    float retrievedMaxx, retrievedMaxy;
    CKBOOL inrange = RenderObjectsToTexture(ctx, ent, rcontext, tss->m_ShadowTexture, light, tss->m_Camera, lmin, lmax, minz, softshadow, &retrievedMaxx, &retrievedMaxy);

    const VxMatrix &InvWorldMat = tss->m_Camera->GetInverseWorldMatrix();

    VxFrustum lightFrustum;
    {
        int width, height;
        VxVector pos, dir, up, right;
        tss->m_Camera->GetPosition(&pos);
        tss->m_Camera->GetOrientation(&dir, &up, &right);
        tss->m_Camera->GetAspectRatio(width, height);
        lightFrustum = VxFrustum(pos, right, up, dir, tss->m_Camera->GetFrontPlane(), tss->m_Camera->GetBackPlane(), tss->m_Camera->GetFov(), (float)height / width);
    }

    VxVector pts[8];
    for (CKObject **it = oArray.Begin(); it != oArray.End(); ++it)
    {
        CK3dEntity *o = (CK3dEntity *)*it;
        if (!o)
            continue;

        CKMesh *mesh = (CKMesh *)o->GetCurrentMesh();
        if (!mesh)
            continue;

        if (!inrange)
        {
            int channel = mesh->GetChannelByMaterial(tss->m_Material);
            if (channel >= 0)
                mesh->ActivateChannel(channel, FALSE);
            continue;
        }

        const VxBbox &box = o->GetBoundingBox(TRUE);
        const VxMatrix &mat1 = o->GetWorldMatrix();

        CKBOOL allOutside = !(VxIntersect::FrustumOBB(lightFrustum, o->GetBoundingBox(TRUE), o->GetWorldMatrix()));

        int channel = mesh->GetChannelByMaterial(tss->m_Material);
        if (allOutside)
        {
            mesh->ActivateChannel(channel, FALSE);
            continue;
        }
        else
        {
            if (channel < 0)
            { // The object wasn't having yet the material as channel
                channel = mesh->AddChannel(tss->m_Material, FALSE);
                // Channel Lighting
                mesh->LitChannel(channel, FALSE);
                // Channel blending
                mesh->SetChannelSourceBlend(channel, VXBLEND_ZERO);
                mesh->SetChannelDestBlend(channel, VXBLEND_SRCCOLOR);
            }
            mesh->ActivateChannel(channel);
        }

        if (o->IsVisible() && !o->IsAllOutsideFrustrum())
        {
            CKDWORD Stride, cStride, nStride;
            Vx2DVector *uvarray;
            VxVector *VertexArray;
            VxVector *NormalArray;

            VertexArray = (VxVector *)mesh->GetPositionsPtr(&Stride);
            NormalArray = (VxVector *)mesh->GetNormalsPtr(&nStride);
            uvarray = (Vx2DVector *)mesh->GetTextureCoordinatesPtr(&cStride, channel);

            if (!uvarray)
                continue;

            int verticescount = mesh->GetVertexCount();
            CKDWORD *VtxFlags = new CKDWORD[verticescount];

            VxVector camdir;
            tss->m_Camera->GetOrientation(&camdir, NULL, NULL, o);
            camdir.Normalize();

            VxVector vPos;
            Vx3DMultiplyMatrixVector(&vPos, InvWorldMat, &barycenter);

            VxMatrix mat2;
            Vx3DMultiplyMatrix(mat2, InvWorldMat, o->GetWorldMatrix());

            float barycenterZ = vPos.z;

            if (perspective)
            {
                VxFrustum frustum(VxVector::axis0(), VxVector::axisX(), VxVector::axisY(), VxVector::axisZ(), barycenterZ, tss->m_Camera->GetBackPlane(), fov, 1.0f);

                // calculate the uvs for the vertices
                float invzfov;
                for (int i = 0; i < verticescount; i++, VertexArray = (VxVector *)((CKBYTE *)VertexArray + Stride), NormalArray = (VxVector *)((CKBYTE *)NormalArray + nStride), uvarray = (Vx2DVector *)((CKBYTE *)uvarray + cStride))
                {

                    // Tranform Vertex Position from mesh coordinates system to projector coordinate system
                    Vx3DMultiplyMatrixVector(&vPos, mat2, VertexArray);

                    VtxFlags[i] = frustum.Classify(vPos);

                    invzfov = 0.5f / (tanfov * vPos.z);
                    uvarray->x = 0.5f + vPos.x * invzfov;
                    uvarray->y = 0.5f - vPos.y * invzfov;
                } // for
            }
            else
            {

                VxVector scale;
                ent->GetScale(&scale, FALSE);
                const Vx2DVector tiling = Vx2DVector(0.5f / retrievedMaxx, 1.0f / retrievedMaxy);

                // Add planar mapping information to the matrix
                mat2[0][0] *= tiling.x;
                mat2[0][1] *= -tiling.y;
                mat2[1][0] *= tiling.x;
                mat2[1][1] *= -tiling.y;
                mat2[2][0] *= tiling.x;
                mat2[2][1] *= -tiling.y;

                mat2[3][0] *= tiling.x;
                mat2[3][1] *= -tiling.y;
                mat2[3][0] += 0.5f;
                mat2[3][1] += 0.5f;

                // TODO : this is shitty
                // utiliser une matrice (comme dans le planar mapping
                // et ne pas faire de look at sur la camera, mais la deplacer...
                for (int i = 0; i < verticescount; i++, VertexArray = (VxVector *)((CKBYTE *)VertexArray + Stride), NormalArray = (VxVector *)((CKBYTE *)NormalArray + nStride), uvarray = (Vx2DVector *)((CKBYTE *)uvarray + cStride))
                {

                    // Tranform Vertex Position from mesh coordinates system to projector coordinate system
                    Vx3DMultiplyMatrixVector(&vPos, mat2, VertexArray);

                    uvarray->x = vPos.x;
                    uvarray->y = vPos.y;

                    VtxFlags[i] = 0;

                    if (vPos.z < barycenterZ)
                        VtxFlags[i] = VXCLIP_FRONT;
                    if (uvarray->x < 0)
                        VtxFlags[i] |= VXCLIP_LEFT;
                    if (uvarray->x > 1.0f)
                        VtxFlags[i] |= VXCLIP_RIGHT;
                    if (uvarray->y < 0)
                        VtxFlags[i] |= VXCLIP_TOP;
                    if (uvarray->y > 1.0f)
                        VtxFlags[i] |= VXCLIP_BOTTOM;

                } // for
            }
            mesh->UVChanged();

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            int FaceCount = mesh->GetFaceCount();
            CKWORD *Indices = mesh->GetFacesIndices();
            CKDWORD NormStride = 0;
            CKBYTE *FaceNormal = mesh->GetFaceNormalsPtr(&NormStride);

            // we now have to exclude the faces not concerned
            int ChannelMask = CKCHANNELMASK(channel);

            for (int i = 0; i < FaceCount; ++i, Indices += 3, FaceNormal += NormStride)
            {
                // Face is corretly oriented to be mapped , check if
                // all its vertices are inside the projection frustrum ( z >0 for the moment)
                if ((VtxFlags[Indices[0]] & VtxFlags[Indices[1]] & VtxFlags[Indices[2]]))
                {
                    mesh->ChangeFaceChannelMask(i, 0, ChannelMask);
                }
                else
                {
                    // test for DotProduct between projection axis
                    if (DotProduct(*(VxVector *)FaceNormal, camdir) < 0)
                    {
                        mesh->ChangeFaceChannelMask(i, ChannelMask, 0);
                    }
                    else
                    {
                        // Remove the face from the channel
                        mesh->ChangeFaceChannelMask(i, 0, ChannelMask);
                    }
                }
            }
#else
            int FaceCount = mesh->GetFaceCount();
            WORD *Indices = mesh->GetFacesIndices();
            CKDWORD NormStride = 0;
            BYTE *FaceNormal = mesh->GetFaceNormalsPtr(&NormStride);
            XArray<WORD> channelIndices;

            for (int i = 0; i < FaceCount; ++i, Indices += 3, FaceNormal += NormStride)
            {
                // Face is corretly oriented to be mapped , check if
                // all its vertices are inside the projection frustrum ( z >0 for the moment)
                if (!((VtxFlags[Indices[0]] & VtxFlags[Indices[1]] & VtxFlags[Indices[2]])) && (DotProduct(*(VxVector *)FaceNormal, camdir) < 0))
                {
                    channelIndices.PushBack(Indices[0]);
                    channelIndices.PushBack(Indices[1]);
                    channelIndices.PushBack(Indices[2]);
                }
            }
            if (!channelIndices.Size())
                mesh->ActivateChannel(channel, FALSE);
            else
            {
                mesh->UVChanged(channel);
                mesh->SetChannelFaceIndices(channel, channelIndices.Size(), channelIndices.Begin());
            }
#endif

            delete[] VtxFlags;
        }
        else
        {
            mesh->ActivateChannel(channel, FALSE);
        }
    } // for

    return CKBR_ACTIVATENEXTFRAME;
}

void ShadowCasterAttributeCallback(int AttribType, CKBOOL Set, CKBeObject *obj, void *arg)
{
    if (!Set)
    {
        CK3dObject *o = (CK3dObject *)obj;
        if (!o)
            return;

        CKMesh *mesh = o->GetCurrentMesh();
        if (!mesh)
            return;

        for (int i = 0; i < mesh->GetChannelCount();)
        {
            CKMaterial *mat = mesh->GetChannelMaterial(i);
            if (mat && !strcmp(mat->GetName(), materialname))
            {
                mesh->RemoveChannel(i);
            }
            else
                ++i;
        }
    }
}

//////////////////////////////////////////////////
//            CALLBACK
//////////////////////////////////////////////////
CKERROR ShadowCasterCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        // we register all the attributes types related to the Particles Systems

        // creation of a CkTexture
        CKCamera *cam = (CKCamera *)ctx->CreateObject(CKCID_TARGETCAMERA, "ShadowCaster Camera", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        cam->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        // creation of a CkTexture
        CKTexture *tex = (CKTexture *)ctx->CreateObject(CKCID_TEXTURE, "ShadowCaster Texture", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        tex->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);

        // creation of a CkMaterial
        CKMaterial *mat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, materialname, beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        mat->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        mat->SetDiffuse(VxColor(1.0f, 1.0f, 1.0f, 1.0f));
        mat->SetSpecular(VxColor(1.0f, 1.0f, 1.0f));
        mat->SetPower(0.0f);
        mat->SetTexture0(tex);
        mat->SetTextureBlendMode(VXTEXTUREBLEND_DECAL);
        mat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        mat->SetTextureMinMode(VXTEXTUREFILTER_LINEAR);
        mat->SetTextureMagMode(VXTEXTUREFILTER_LINEAR);
        mat->SetTextureBorderColor(0xFFFFFFFF);

        // save of the IDs
        ShadowCasterStruct *tss = new ShadowCasterStruct;

        CKAttributeManager *attman = ctx->GetAttributeManager();
        int att = attman->RegisterNewAttributeType("Shadow Caster Receiver", CKPGUID_NONE, CKCID_3DOBJECT);

        tss->m_Camera = cam;
        tss->m_ShadowTexture = tex;
        tss->m_Material = mat;
        tss->m_ReceiverAttribute = att;

        tss->m_Camera->SetAspectRatio(1, 1);

        beh->SetLocalParameterValue(0, tss, sizeof(ShadowCasterStruct));

        delete tss;
    }
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        ShadowCasterStruct *tss = (ShadowCasterStruct *)beh->GetLocalParameterWriteDataPtr(0);

        int size = 64;
        beh->GetLocalParameterValue(1, &size);

        size = (Near2Power(size));
        if (size != tss->m_ShadowTexture->GetWidth())
        { // Size changes
            tss->m_ShadowTexture->Create(size, size);
        }
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        ShadowCasterStruct *tss = (ShadowCasterStruct *)beh->GetLocalParameterWriteDataPtr(0);
        if (!tss)
            break;

        CKMaterial *mat = tss->m_Material;

        // now we remove the ShadowCaster channel
        CKAttributeManager *attman = ctx->GetAttributeManager();
        const XObjectPointerArray oArray = attman->GetAttributeListPtr(tss->m_ReceiverAttribute);

        for (CKObject **it = oArray.Begin(); it != oArray.End(); ++it)
        {
            CK3dEntity *o = (CK3dEntity *)*it;
            if (!o)
                continue;

            CKMesh *mesh = o->GetCurrentMesh();
            if (!mesh)
                continue;

            mesh->RemoveChannelByMaterial(mat);
        }

        CKDestroyObject(tss->m_ShadowTexture);
        CKDestroyObject(tss->m_Material);
        CKDestroyObject(tss->m_Camera);
    }
    break;
    case CKM_BEHAVIORPRESAVE:
    case CKM_BEHAVIORPAUSE:
    {
        ShadowCasterStruct *tss = (ShadowCasterStruct *)beh->GetLocalParameterWriteDataPtr(0);

        CKMaterial *mat = tss->m_Material;

        CKAttributeManager *attman = ctx->GetAttributeManager();
        const XObjectPointerArray oArray = attman->GetAttributeListPtr(tss->m_ReceiverAttribute);

        for (CKObject **it = oArray.Begin(); it != oArray.End(); ++it)
        {
            CK3dEntity *o = (CK3dEntity *)*it;
            if (!o)
                continue;

            CKMesh *mesh = o->GetCurrentMesh();
            if (!mesh)
                continue;

            mesh->RemoveChannelByMaterial(mat);
        }
    }
    break;
    }
    return CKBR_OK;
}
