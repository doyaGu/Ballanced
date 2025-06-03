/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlanarShadow
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPlanarShadowDecl();
CKERROR CreatePlanarShadowProto(CKBehaviorPrototype **pproto);
int PlanarShadow(const CKBehaviorContext &behcontext);
int PlanarShadowPreRenderCallBack(CKRenderContext *rc, CKRenderObject *plane, void *arg);
int PlanarShadowPostRenderCallBack(CKRenderContext *rc, CKRenderObject *plane, void *arg);

CKObjectDeclaration *FillBehaviorPlanarShadowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Planar Shadow");
    od->SetDescription("Display planars shadows of selected objects projected from a light.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Shadow Group(s): </SPAN>Groups of objects effectively shadowed.<BR>
    <SPAN CLASS=pin>Light: </SPAN>Light from which the shadow are projected.<BR>
    <SPAN CLASS=pin>Color: </SPAN>Color of the shadows.<BR>
    <SPAN CLASS=pin>Plane Normal: </SPAN>Normal of the plane to cast the shadow to.<BR>
    <BR>
    The objects must have the "Planar Shadow Object" attributes to be taken into account
    by this behavior. The group will determine on which plane the objects will cast shadows
    and the mesh will be used for shadows rendering, if provided. Otherwise, it will "shadow"
    the entire hierarchy with their current meshes.<BR>
    The shadow is projected in perspective with point and spot lights and projected parralel
    with directionnal lights.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x731072fa, 0x56cf087f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlanarShadowProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/Shadows");
    return od;
}

CKERROR CreatePlanarShadowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Planar Shadow");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Shadowing Group(s)", CKPGUID_FILTER, "1");
    proto->DeclareInParameter("Light", CKPGUID_LIGHT);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "0, 0, 0, 255");
    proto->DeclareInParameter("Plane Normal", CKPGUID_VECTOR, "0,1,0");

    proto->DeclareSetting("Discard Off Plane Shadow", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(PlanarShadow);

    *pproto = proto;
    return CK_OK;
}

int PlanarShadow(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // We enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (ent)
    {
        CKContext *ctx = behcontext.Context;
        CKRenderContext *rc = behcontext.CurrentRenderContext;

        // Is Stencil desired ?
        CKBOOL maskOffPlane = TRUE;
        beh->GetLocalParameterValue(0, &maskOffPlane);

        if (maskOffPlane)
        {
            ///
            // We now determine the hardware present
            VxDriverDesc *driver = ctx->GetRenderManager()->GetRenderDriverDescription(rc->GetDriverIndex());
            int stencilBpp = 0;
            rc->GetPixelFormat(NULL, NULL, &stencilBpp);
            if (stencilBpp >= 1)
                ent->AddPreRenderCallBack(PlanarShadowPreRenderCallBack, NULL, TRUE);
        }

        // We add the post rendercallback
        ent->AddPostRenderCallBack(PlanarShadowPostRenderCallBack, (void *)beh->GetID(), TRUE);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/****************** RENDER CALLBACK ********************/

void RenderShadow(CKRenderContext *rc, CK3dEntity *ent, CKMesh *mesh, CKLight *light, CK3dEntity *plane, CKDWORD col, VxVector &localplanenormal)
{
    int facescount = mesh->GetFaceCount();
    int verticescount = mesh->GetVertexCount();
    if (!facescount)
        return;

    ///
    // the shadow data
    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, verticescount);
    CKWORD *indicesPtr = mesh->GetFacesIndices();

    // We tranform everything in the object referential
    VxVector zero(0.0f), lightpos, entpos, planepos, planenorm(0, 1.0f, 0);
    // Optimize this
    plane->InverseTransform(&lightpos, &zero, light);
    plane->InverseTransform(&planepos, &zero, plane);
    planenorm = Normalize(localplanenormal);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
#else
    VxVector4 *positions = (VxVector4 *)data->Positions.Ptr;
#endif
    CKDWORD vStride;
    VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vStride);

    VxMatrix shadowmatrix;
    if ((light->GetType() == VX_LIGHTDIREC))
    { // Parrallel projection
        VxVector lightdir, locallightdir(0.0f, 0.0f, 1.0f);
        plane->InverseTransformVector(&lightdir, &locallightdir, light);
        lightdir.Normalize();

        float invwn = 1.0f / DotProduct(lightdir, planenorm);
        float dp = DotProduct(planepos, planenorm);

        shadowmatrix[0][0] = 1.0f - planenorm.x * lightdir.x * invwn;
        shadowmatrix[0][1] = -planenorm.x * lightdir.y * invwn;
        shadowmatrix[0][2] = -planenorm.x * lightdir.z * invwn;
        shadowmatrix[0][3] = 0;

        shadowmatrix[1][0] = -planenorm.y * lightdir.x * invwn;
        shadowmatrix[1][1] = 1.0f - planenorm.y * lightdir.y * invwn;
        shadowmatrix[1][2] = -planenorm.y * lightdir.z * invwn;
        shadowmatrix[1][3] = 0;

        shadowmatrix[2][0] = -planenorm.z * lightdir.x * invwn;
        shadowmatrix[2][1] = -planenorm.z * lightdir.y * invwn;
        shadowmatrix[2][2] = 1.0f - planenorm.z * lightdir.z * invwn;
        shadowmatrix[2][3] = 0;

        shadowmatrix[3][0] = dp * invwn * lightdir.x;
        shadowmatrix[3][1] = dp * invwn * lightdir.y;
        shadowmatrix[3][2] = dp * invwn * lightdir.z;
        shadowmatrix[3][3] = 1.0f;

        VxMatrix resultMatrix;
        Vx3DMultiplyMatrix4(resultMatrix, plane->GetInverseWorldMatrix(), ent->GetWorldMatrix());
        Vx3DMultiplyMatrix4(resultMatrix, shadowmatrix, resultMatrix);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxStridedData dst(positions, data->PositionStride);
#else
        VxStridedData dst(positions, data->Positions.Stride);
#endif
        VxStridedData src(vertices, vStride);
        Vx3DMultiplyMatrixVectorStrided(&dst, &src, resultMatrix, verticescount);
    }
    else
    { // Perspective projection
        float dp = DotProduct(lightpos - planepos, planenorm);
        float qn = DotProduct(planepos, planenorm);
        shadowmatrix[0][0] = dp - planenorm.x * lightpos.x;
        shadowmatrix[0][1] = -planenorm.x * lightpos.y;
        shadowmatrix[0][2] = -planenorm.x * lightpos.z;
        shadowmatrix[0][3] = -planenorm.x;

        shadowmatrix[1][0] = -planenorm.y * lightpos.x;
        shadowmatrix[1][1] = dp - planenorm.y * lightpos.y;
        shadowmatrix[1][2] = -planenorm.y * lightpos.z;
        shadowmatrix[1][3] = -planenorm.y;

        shadowmatrix[2][0] = -planenorm.z * lightpos.x;
        shadowmatrix[2][1] = -planenorm.z * lightpos.y;
        shadowmatrix[2][2] = dp - planenorm.z * lightpos.z;
        shadowmatrix[2][3] = -planenorm.z;

        shadowmatrix[3][0] = qn * lightpos.x;
        shadowmatrix[3][1] = qn * lightpos.y;
        shadowmatrix[3][2] = qn * lightpos.z;
        shadowmatrix[3][3] = DotProduct(planenorm, lightpos);

        VxMatrix resultMatrix;
        Vx3DMultiplyMatrix4(resultMatrix, plane->GetInverseWorldMatrix(), ent->GetWorldMatrix());
        Vx3DMultiplyMatrix4(resultMatrix, shadowmatrix, resultMatrix);

        ///
        // Vertices
        for (int i = 0; i < verticescount; ++i)
        {
            Vx3DMultiplyMatrixVector4(positions, resultMatrix, vertices);
            *((VxVector *)positions) /= positions->w;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
#else
            positions = (VxVector4 *)((CKBYTE *)positions + data->Positions.Stride);
#endif
            vertices = (VxVector *)((CKBYTE *)vertices + vStride);
        }
    }

    ///
    // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxFillStructure(verticescount, data->ColorPtr, data->ColorStride, 4, &col);
#else
    VxFillStructure(verticescount, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

    // The draw
    //	rc->SetState(VXRENDERSTATE_ZFUNC, VXCMP_LESSEQUAL);
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
    rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    rc->SetTexture(NULL);
    rc->DrawPrimitive(VX_TRIANGLELIST, indicesPtr, facescount * 3, data);
}

int PlanarShadowPreRenderCallBack(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{

    CK3dEntity *plane = (CK3dEntity *)obj;
    CKContext *ctx = rc->GetCKContext();

    ///
    // We want the object to draw in the stencil buffer

    // We clear the stencil buffer TODO : only if necessary
    // rc->Clear(CK_RENDER_CLEARSTENCIL);

    CKBOOL stencilWasEnabled = rc->GetState(VXRENDERSTATE_STENCILENABLE);
    if (stencilWasEnabled)
    {
        plane->SetFlags(plane->GetFlags() | CK_3DENTITY_RESERVED0);
    }
    else
    {
        plane->SetFlags(plane->GetFlags() & ~CK_3DENTITY_RESERVED0);
    }

    rc->SetState(VXRENDERSTATE_STENCILENABLE, TRUE);

    int freeStencilBit = rc->GetFirstFreeStencilBits();
    if (freeStencilBit == -1)
    {
        rc->Clear(CK_RENDER_CLEARSTENCIL);
        freeStencilBit = 0;
    }
    rc->UsedStencilBits(1 << freeStencilBit);

    if (!stencilWasEnabled)
    {
        rc->SetState(VXRENDERSTATE_STENCILREF, 0xff);
        rc->SetState(VXRENDERSTATE_STENCILMASK, 0x0);
        rc->SetState(VXRENDERSTATE_STENCILWRITEMASK, 1 << freeStencilBit);
        rc->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_ALWAYS);
    }
    else
    {
        rc->SetState(VXRENDERSTATE_STENCILWRITEMASK, 1 << freeStencilBit);
    }

    rc->SetState(VXRENDERSTATE_STENCILFAIL, VXSTENCILOP_KEEP);
    rc->SetState(VXRENDERSTATE_STENCILZFAIL, VXSTENCILOP_KEEP);
    rc->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_REPLACE);

    return 1;
}

#define NONE 0
#define STENCIL 1
#define CLIPPLANES 2

int PlanarShadowPostRenderCallBack(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{
    struct Cleaner
    {
        Cleaner(CKRenderContext *rc, CK3dEntity *plane, int Mode)
        {
            m_RenderContext = rc;

            m_StencilReadMask = rc->GetState(VXRENDERSTATE_STENCILMASK);
            m_StencilWriteMask = rc->GetState(VXRENDERSTATE_STENCILWRITEMASK);

            m_StencilWasEnabled = plane->GetFlags() & CK_3DENTITY_RESERVED0;
            if (m_StencilWasEnabled)
                plane->SetFlags(plane->GetFlags() & ~CK_3DENTITY_RESERVED0);
            m_ClipPlanesWasEnabled = rc->GetState(VXRENDERSTATE_CLIPPLANEENABLE);

            m_Mode = Mode;
        }

        ~Cleaner()
        {
            // We restore the stencil states
            switch (m_Mode)
            {
            case STENCIL:
            {
                m_RenderContext->SetState(VXRENDERSTATE_STENCILMASK, m_StencilReadMask);
                if (!m_StencilWasEnabled)
                    m_RenderContext->SetState(VXRENDERSTATE_STENCILENABLE, FALSE);
            }
            break;
            case CLIPPLANES:
                if (!m_ClipPlanesWasEnabled)
                    m_RenderContext->SetState(VXRENDERSTATE_CLIPPLANEENABLE, 0);
                break;
            }
        }

        CKRenderContext *m_RenderContext;
        CKDWORD m_StencilReadMask;
        CKDWORD m_StencilWriteMask;
        int m_Mode;
        CKDWORD m_StencilWasEnabled;
        CKDWORD m_ClipPlanesWasEnabled;
    };

    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return 0;

    CK3dEntity *plane = (CK3dEntity *)obj;
    CKContext *ctx = rc->GetCKContext();

    int mode = NONE;

    // Is Masking desired ?
    CKBOOL maskOffPlane = TRUE;
    beh->GetLocalParameterValue(0, &maskOffPlane);

    if (maskOffPlane)
    {
        ///
        // We now determine the hardware present
        VxDriverDesc *driver = ctx->GetRenderManager()->GetRenderDriverDescription(rc->GetDriverIndex());
        int stencilBpp = 0;
        rc->GetPixelFormat(NULL, NULL, &stencilBpp);
        if (stencilBpp >= 1)
            mode = STENCIL;
        else if (driver->Caps3D.MaxClipPlanes >= 4)
            mode = CLIPPLANES;
    }

    Cleaner cleaner(rc, plane, mode);

    int planeflag = 1;
    beh->GetInputParameterValue(0, &planeflag);

    CKLight *light = (CKLight *)beh->GetInputParameterObject(1);
    if (!light)
        return 1;

    VxColor vcol;
    beh->GetInputParameterValue(2, &vcol);
    CKDWORD col = RGBAFTOCOLOR(&vcol);

    VxVector localplanenormal(0.0f, 1.0f, 0.0f);
    beh->GetInputParameterValue(3, &localplanenormal);

    // we do not render the plane if it is culled by the viewpoint
    VxVector viewposition;
    CK3dEntity *entity = rc->GetViewpoint();
    entity->GetPosition(&viewposition, plane);

    if (DotProduct(viewposition, localplanenormal) < 0)
        return 1;

    CKBOOL directional = (light->GetType() == VX_LIGHTDIREC);

    ///
    // we create the clip planes
    light->GetPosition(&viewposition);

    const VxBbox &box = plane->GetBoundingBox(TRUE);
    float min = localplanenormal.x;
    int x = 1;
    int y = 2;

    if (min < localplanenormal.y)
    {
        min = localplanenormal.y;
        x = 0;
        y = 2;
    }
    if (min < localplanenormal.z)
    {
        x = 0;
        y = 1;
    }

    // We Calculate the extrema of the box in the world
    VxVector worldPosition[4];
    worldPosition[0] = box.Max;
    worldPosition[0].v[x] = box.Min.v[x];
    worldPosition[0].v[y] = box.Min.v[y];
    worldPosition[1] = box.Max;
    worldPosition[1].v[x] = box.Min.v[x];
    worldPosition[2] = box.Max;
    worldPosition[3] = box.Max;
    worldPosition[3].v[y] = box.Min.v[y];
    Vx3DMultiplyMatrixVectorMany(worldPosition, plane->GetWorldMatrix(), worldPosition, 4, sizeof(VxVector));

    // we define the 4 clipping planes
    VxPlane left(worldPosition[1], worldPosition[0], viewposition);
    VxPlane right(worldPosition[2], worldPosition[1], viewposition);
    VxPlane top(worldPosition[3], worldPosition[2], viewposition);
    VxPlane down(worldPosition[0], worldPosition[3], viewposition);

    ///
    // We draw the plane in the stencil
    switch (mode)
    {
    case STENCIL:
    {
        // We now set rendering to only suceed if stencil >= 1
        rc->SetState(VXRENDERSTATE_STENCILENABLE, TRUE); // Bugfix : when there are two planar shadow...
        rc->SetState(VXRENDERSTATE_STENCILPASS, VXSTENCILOP_KEEP);
        rc->SetState(VXRENDERSTATE_STENCILREF, 0xff);
        rc->SetState(VXRENDERSTATE_STENCILMASK, cleaner.m_StencilWriteMask);
        rc->SetState(VXRENDERSTATE_STENCILWRITEMASK, cleaner.m_StencilWriteMask);
        rc->SetState(VXRENDERSTATE_STENCILFUNC, VXCMP_EQUAL); // GREATEREQUAL);
    }
    break;
    case CLIPPLANES:
    {
        // we set them for rendering
        rc->SetUserClipPlane(2, left);
        rc->SetUserClipPlane(0, right);
        rc->SetUserClipPlane(1, top);
        rc->SetUserClipPlane(3, down);
        rc->SetState(VXRENDERSTATE_CLIPPLANEENABLE, 0xf);
    }
    break;
    }

    // Get the attributes
    CKAttributeManager *attman = ctx->GetAttributeManager();
    int att = attman->GetAttributeTypeByName("Planar Shadow Object");

    // Projection Trick
    VxMatrix projmat = rc->GetProjectionTransformationMatrix();
    float oldproj = projmat[3].z;
    projmat[3].z = oldproj * 1.005f; // to disable Z buffer fighting
    rc->SetProjectionTransformationMatrix(projmat);

    const XObjectPointerArray &Array = attman->GetAttributeListPtr(att);
    for (CKObject **it = Array.Begin(); it != Array.End(); ++it)
    {
        // we get attribute owner
        CK3dEntity *ent = (CK3dEntity *)*it;
        if (!ent || !ent->IsVisible() || ent->IsHiddenByParent())
            continue;

        CKParameterOut *pout = ent->GetAttributeParameter(att);
        CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();

        pout = (CKParameterOut *)ctx->GetObject(paramids[0]);
        int objectflag = 1;
        pout->GetValue(&objectflag);

        // We check if the plane should reflect this object
        if (!(planeflag & objectflag))
            continue;

        pout = (CKParameterOut *)ctx->GetObject(paramids[1]);
        CKMesh *mesh = (CKMesh *)ctx->GetObject(*(CK_ID *)pout->GetReadDataPtr());
        CKBOOL hierarchy = TRUE;
        if (mesh)
            hierarchy = FALSE;

        if (hierarchy)
        {
            mesh = ent->GetCurrentMesh();
            if (mesh && mesh->IsVisible())
            {
                CKBOOL culled = FALSE;
                if (!directional)
                {
                    culled = ((left.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                              (right.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                              (top.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                              (down.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f))
                                 ? FALSE
                                 : TRUE;
                }
                if (!culled)
                {
                    RenderShadow(rc, ent, mesh, light, plane, col, localplanenormal);
                }
            }

            CK3dEntity *child = NULL;
            while (child = ent->HierarchyParser(child))
            {
                if (child->IsVisible() && !child->IsHiddenByParent())
                {
                    mesh = child->GetCurrentMesh();
                    if (mesh && mesh->IsVisible())
                    {
                        if (!directional)
                        {
                            if (left.Classify(child->GetBoundingBox(TRUE), child->GetWorldMatrix()) < 0.0f)
                                continue;
                            if (right.Classify(child->GetBoundingBox(TRUE), child->GetWorldMatrix()) < 0.0f)
                                continue;
                            if (top.Classify(child->GetBoundingBox(TRUE), child->GetWorldMatrix()) < 0.0f)
                                continue;
                            if (down.Classify(child->GetBoundingBox(TRUE), child->GetWorldMatrix()) < 0.0f)
                                continue;
                        }

                        // we now check if the object shadow will fall into the plane
                        RenderShadow(rc, child, mesh, light, plane, col, localplanenormal);
                    }
                }
            }
        }
        else
        {
            CKBOOL culled = FALSE;
            if (!directional)
            {
                culled = ((left.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                          (right.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                          (top.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f) &&
                          (down.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix()) >= 0.0f))
                             ? FALSE
                             : TRUE;
            }

            if (!culled)
            {
                RenderShadow(rc, ent, mesh, light, plane, col, localplanenormal);
            }
        }
    }

    projmat[3].z = oldproj; // restore projection matrix
    rc->SetProjectionTransformationMatrix(projmat);

    return 1;
}
