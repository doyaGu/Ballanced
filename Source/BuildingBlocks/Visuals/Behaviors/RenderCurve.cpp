/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RenderCurve
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_CURVEMODE CKGUID(0x4d236301, 0x5be530bc)

CKObjectDeclaration *FillBehaviorRenderCurveDecl();
CKERROR CreateRenderCurveProto(CKBehaviorPrototype **pproto);
int RenderCurve(const CKBehaviorContext &behcontext);
int CurveRenderCallback(CKRenderContext *rc, CKRenderObject *ent, void *arg);

CKObjectDeclaration *FillBehaviorRenderCurveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Render Curve");
    od->SetDescription("Renders the curve with a specific material.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>Material to use to render.<BR>
    <SPAN CLASS=pin>Size: </SPAN>Size of the shape to render.<BR>
    <SPAN CLASS=pin>Texture Scale: </SPAN>Scale of the texture coordinates.<BR>
    <SPAN CLASS=pin>Texture Offset: </SPAN>Starting offset of the texture coordinates.<BR>
    <SPAN CLASS=pin>Up: </SPAN>Vector defining the up of the shape.<BR>
    <SPAN CLASS=pin>Tube Lighting: </SPAN>Should the tube be lighted?.<BR>
    <SPAN CLASS=pin>Tube Tesselation: </SPAN>Tesselation of the round shape of the tube.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x215c7794, 0x7ac40f5d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRenderCurveProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateRenderCurveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Render Curve");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Size", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Texture Scale", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Texture Offset", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Up", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Tube Lighting", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Tube Tesselation", CKPGUID_INT, "3");

    proto->DeclareSetting("Render Mode", CKPGUID_CURVEMODE, "Cross");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RenderCurve);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RenderCurve(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();

    if (beh->IsInputActive(1))
    { // We enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1, TRUE);

        if (!ent)
            return CKBR_OK;
        ent->RemovePostRenderCallBack(CurveRenderCallback, beh);

        return CKBR_OK;
    }

    if (!ent)
        return CKBR_OK;
    ent->AddPostRenderCallBack(CurveRenderCallback, (void *)beh->GetID(), TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

// Tube Rendering
void RenderTube(CKRenderContext *dev, CKCurve *curve, int rtc, VxVector &startup, Vx2DVector &size, Vx2DVector &texscale, CKDWORD col, float ustart, CKBOOL lighting)
{
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);

    CKMesh *mesh = curve->GetCurrentMesh();
    if (!mesh)
        curve->UpdateMesh();
    mesh = curve->GetCurrentMesh();

    int tc = mesh->GetVertexCount() - 1;

    int vcount = rtc * (tc + 1);

    VxDrawPrimitiveData *data;
    if (lighting)
    {
        data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VNT | CKRST_DP_VBUFFER), vcount);
    }
    else
    {
        data = dev->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_TR_CL_VCT | CKRST_DP_VBUFFER), vcount);

        /////////////////
        // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        CKDWORD *colors = (CKDWORD *)data->ColorPtr;
        VxFillStructure(vcount, colors, data->ColorStride, 4, &col);
#else
        CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
        VxFillStructure(vcount, colors, data->Colors.Stride, 4, &col);
#endif
    }

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    // Positions
    VxVector *positions = (VxVector *)data->PositionPtr;

    // Normals
    VxVector *normals = (VxVector *)data->NormalPtr;

    // UVs
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
#else
    // Positions
    VxVector *positions = (VxVector *)data->Positions.Ptr;

    // Normals
    VxVector *normals = (VxVector *)data->Normals.Ptr;

    // UVs
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
#endif

    float delta = 1.0f / tc;
    float a = 0.0f;

    VxVector pos, oldpos, dir;

    CKDWORD Stride;
    VxVector *curvepoints = (VxVector *)mesh->GetPositionsPtr(&Stride);

    float p = 0.0f;
    float pdelta = 2 * PI / rtc;
    float inv2pi = 1.0f / (2 * PI);

    curve->GetLocalPos(a + 0.01f, &pos);
    curve->GetLocalPos(a, &oldpos);
    VxVector offcurve = oldpos - Normalize(pos - oldpos);

    VxVector up, right, offset;

    // we calculate the positions and uvs
    int j;
    for (j = 0; j < rtc; ++j)
    {
        a = 0.0f;
        VxVector *cp = curvepoints;

        pos = offcurve;
        float psin = size.y * sinf(p);
        float pcos = size.x * cosf(p);
        for (int i = 0; i <= tc; ++i)
        {

            oldpos = pos;
            pos = *cp;
            dir = Normalize(pos - oldpos);

            up = startup;
            right = Normalize(CrossProduct(up, dir));
            up = Normalize(CrossProduct(dir, right));

            offset = pcos * right + psin * up;

            // Positions Up
            *positions = pos + offset;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
            positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif

            if (lighting)
            {
                // Normals
                *normals = Normalize(offset);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
                normals = (VxVector *)((CKBYTE *)normals + data->NormalStride);
#else
                normals = (VxVector *)((CKBYTE *)normals + data->Normals.Stride);
#endif
            }

            // UVs
            uvs->u = ustart + a * texscale.x;
            uvs->v = texscale.y * p * inv2pi;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

            a += delta;
            cp = (VxVector *)((CKBYTE *)cp + Stride);
        }

        p += pdelta;
    }

    // now the indices
    CKWORD *indices = dev->GetDrawPrimitiveIndices(rtc * 2 * (tc + 1));

    int ind = 0;
    CKBOOL forth = TRUE;

    for (j = 0; j < rtc; ++j)
    {
        int jmod = (j + 1) % rtc;
        if (forth)
        {
            for (int i = 0; i <= tc; ++i)
            {
                indices[ind++] = j * (tc + 1) + i;
                indices[ind++] = jmod * (tc + 1) + i;
            }
        }
        else
        {
            for (int i = 0; i <= tc; ++i)
            {
                indices[ind++] = j * (tc + 1) + (tc - i);
                indices[ind++] = jmod * (tc + 1) + (tc - i);
            }
        }
        forth = !forth;
    }

    // The Draw
    dev->DrawPrimitive(VX_TRIANGLESTRIP, indices, ind, data);
}

// Plane Rendering
void RenderPlane(CKRenderContext *dev, CKCurve *curve, VxVector &startup, Vx2DVector &size, Vx2DVector &texscale, CKDWORD col, float ustart)
{
    CKMesh *mesh = curve->GetCurrentMesh();
    if (!mesh)
        curve->UpdateMesh();
    mesh = curve->GetCurrentMesh();
    // curve->UpdateMesh();
    int tc = mesh->GetVertexCount() - 1;

    CKDWORD Stride;
    VxVector *curvepoints = (VxVector *)mesh->GetPositionsPtr(&Stride);

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 2 * (tc + 1));

    /////////////////
    // Colors

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
    VxFillStructure((tc + 1) * 2, colors, data->ColorStride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->PositionPtr;
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
#else
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
    VxFillStructure((tc + 1) * 2, colors, data->Colors.Stride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
#endif

    float delta = 1.0f / tc;
    float a = 0.0f;

    VxVector pos, oldpos, dir, right;
    curve->GetLocalPos(a + 0.01f, &pos);
    curve->GetLocalPos(a, &oldpos);
    pos = oldpos - Normalize(pos - oldpos);

    VxVector *cp = curvepoints;
    VxVector up = startup;

    for (int i = 0; i <= tc; ++i)
    {

        oldpos = pos;
        pos = *cp;
        dir = Normalize(pos - oldpos);

        VxVector right = size.x * Normalize(CrossProduct(up, dir));
        up = Normalize(CrossProduct(dir, right));

        // Positions
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        *positions = pos - right;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *positions = pos + right;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
#else
        *positions = pos - right;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
        *positions = pos + right;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
#endif

        // UVs
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif
        a += delta;
        cp = (VxVector *)((CKBYTE *)cp + Stride);
    }

    // the Drawing
    dev->DrawPrimitive(VX_TRIANGLESTRIP, NULL, 0, data);
}

// Cross Rendering
void RenderCross(CKRenderContext *dev, CKCurve *curve, VxVector &startup, Vx2DVector &size, Vx2DVector &texscale, CKDWORD col, float ustart)
{
    CKMesh *mesh = curve->GetCurrentMesh();
    if (!mesh)
        curve->UpdateMesh();
    mesh = curve->GetCurrentMesh();
    // curve->UpdateMesh();
    int tc = mesh->GetVertexCount() - 1;

    CKDWORD Stride;
    VxVector *curvepoints = (VxVector *)mesh->GetPositionsPtr(&Stride);

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * (tc + 1));

    /////////////////
    // Colors

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
    VxFillStructure((tc + 1) * 2, colors, data->ColorStride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->PositionPtr;
    VxVector *positions2 = (VxVector *)((CKBYTE *)data->PositionPtr + 2 * (tc + 1) * data->PositionStride);
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
#else
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
    VxFillStructure((tc + 1) * 2, colors, data->Colors.Stride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    VxVector *positions2 = (VxVector *)((CKBYTE *)data->Positions.Ptr + 2 * (tc + 1) * data->Positions.Stride);
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
#endif

    float delta = 1.0f / tc;
    float a = 0.0f;

    VxVector pos, oldpos, dir, right;
    curve->GetLocalPos(a + 0.01f, &pos);
    curve->GetLocalPos(a, &oldpos);
    pos = oldpos - Normalize(pos - oldpos);

    VxVector *cp = curvepoints;
    for (int i = 0; i <= tc; ++i)
    {

        oldpos = pos;
        pos = *cp;
        dir = Normalize(pos - oldpos);

        VxVector up = startup;
        VxVector right = Normalize(CrossProduct(up, dir));
        up = Normalize(CrossProduct(dir, right));

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        // Positions Up
        *positions = pos + up * size.x;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *positions = pos - up * size.x;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

        // Positions Right
        *positions2 = pos + right * size.y;
        positions2 = (VxVector *)((CKBYTE *)positions2 + data->PositionStride);
        *positions2 = pos - right * size.y;
        positions2 = (VxVector *)((CKBYTE *)positions2 + data->PositionStride);

        // UVs
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        // Positions Up
        *positions = pos + up * size.x;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
        *positions = pos - up * size.x;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);

        // Positions Right
        *positions2 = pos + right * size.y;
        positions2 = (VxVector *)((CKBYTE *)positions2 + data->Positions.Stride);
        *positions2 = pos - right * size.y;
        positions2 = (VxVector *)((CKBYTE *)positions2 + data->Positions.Stride);

        // UVs
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        a += delta;
        cp = (VxVector *)((CKBYTE *)cp + Stride);
    }

    // the Drawing
    data->VertexCount = 2 * (tc + 1);
    dev->DrawPrimitive(VX_TRIANGLESTRIP, NULL, 0, data);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    void *oldpp = data->PositionPtr;
    data->PositionPtr = (CKBYTE *)data->PositionPtr + data->VertexCount * data->PositionStride;
    dev->DrawPrimitive(VX_TRIANGLESTRIP, NULL, 0, data);

    // we restore the pointers
    data->VertexCount = 4 * (tc + 1);
    data->PositionPtr = oldpp;
#else
    void *oldpp = data->Positions.Ptr;
    data->Positions.Ptr = (CKBYTE *)data->Positions.Ptr + data->VertexCount * data->Positions.Stride;
    dev->DrawPrimitive(VX_TRIANGLESTRIP, NULL, 0, data);

    // we restore the pointers
    data->VertexCount = 4 * (tc + 1);
    data->Positions.Ptr = oldpp;
#endif
}

// Line Rendering
void RenderLine(CKRenderContext *dev, CKCurve *curve, CKDWORD col)
{

    CKMesh *mesh = curve->GetCurrentMesh();
    if (!mesh)
        curve->UpdateMesh();
    mesh = curve->GetCurrentMesh();

    // Use the material color
    CKDWORD firstvertexcolor;
    // Avoiding vertex color reassignment if already done

    firstvertexcolor = mesh->GetVertexColor(0);
    if (firstvertexcolor != col)
    {
        int nbvertex = mesh->GetVertexCount();
        for (int i = 0; i < nbvertex; i++)
            mesh->SetVertexColor(i, col);
    }
    mesh->Render(dev, curve);
}
// 2 Dimension Rendering
void RenderTwoDim(CKRenderContext *dev, CKCurve *curve, Vx2DVector &size, Vx2DVector &texscale, CKDWORD col, float ustart)
{
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);

    CKMesh *mesh = curve->GetCurrentMesh();
    if (!mesh)
        curve->UpdateMesh();
    mesh = curve->GetCurrentMesh();
    // curve->UpdateMesh();
    int tc = mesh->GetVertexCount() - 1;

    VxRect ViewRect;
    dev->GetViewRect(ViewRect);
    float dx = 1.0f / (float)ViewRect.GetWidth();
    float dy = 1.0f / (float)ViewRect.GetHeight();

    VxMatrix View = dev->GetViewTransformationMatrix();
    VxMatrix World = dev->GetWorldTransformationMatrix();
    dev->SetWorldTransformationMatrix(VxMatrix::Identity());
    dev->SetViewTransformationMatrix(VxMatrix::Identity());
    VxMatrix TransformMat = View * World;

    CKDWORD Stride;
    VxVector *curvepoints = (VxVector *)mesh->GetPositionsPtr(&Stride);

    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 2 * (tc + 1));
    VxStridedData SrcVertices(curvepoints, Stride);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxStridedData DstVertices(data->PositionPtr, 2 * data->PositionStride);
#else
    VxStridedData DstVertices(data->Positions.Ptr, 2 * data->Positions.Stride);
#endif
    Vx3DMultiplyMatrixVectorStrided(&DstVertices, &SrcVertices, TransformMat, tc + 1);

    /////////////////
    // Colors

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
    VxFillStructure((tc + 1) * 2, colors, data->ColorStride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->PositionPtr;
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
#else
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
    VxFillStructure((tc + 1) * 2, colors, data->Colors.Stride, 4, &col);

    // Positions
    VxVector *positions = (VxVector *)data->Positions.Ptr;
    // UVs
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
#endif

    float delta = 1.0f / tc;
    float a = 0.0f;

    VxVector pos, oldpos, dir, right;
    VxVector up(0, 0, 1.0f);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    memcpy(&pos, positions + 2 * data->PositionStride, sizeof(VxVector));
#else
    memcpy(&pos, positions + 2 * data->Positions.Stride, sizeof(VxVector));
#endif
    memcpy(&oldpos, positions, sizeof(VxVector));
    pos = oldpos - Normalize(pos - oldpos);

    for (int i = 0; i <= tc; ++i)
    {

        oldpos = pos;
        pos = *positions;
        dir = Normalize(pos - oldpos);

        dir.z = 0.0f;
        VxVector right = size.x * Normalize(CrossProduct(up, dir));
        right.z = 0.0f;
        right.x *= pos.z * dx;
        right.y *= pos.z * dy;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        // Positions
        *positions = pos - right;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);
        *positions = pos + right;
        positions = (VxVector *)((CKBYTE *)positions + data->PositionStride);

        // UVs
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
        // Positions
        *positions = pos - right;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);
        *positions = pos + right;
        positions = (VxVector *)((CKBYTE *)positions + data->Positions.Stride);

        // UVs
        uvs->u = ustart + a * texscale.x;
        uvs->v = texscale.y;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
        uvs->u = ustart + a * texscale.x;
        uvs->v = 0.0f;
        uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

        a += delta;
    }

    // the Drawing
    dev->DrawPrimitive(VX_TRIANGLESTRIP, NULL, 0, data);
    dev->SetWorldTransformationMatrix(World);
    dev->SetViewTransformationMatrix(View);
}
#define PLANE 0
#define CROSS 1
#define TUBE 2
#define TWODIM 3
#define LINE 4

/*******************************************************/
/****************** RENDER FUNCTION ********************/
int CurveRenderCallback(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{

    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return 0;

    CKCurve *curve = (CKCurve *)obj;

    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(0);
    if (mat)
        mat->SetAsCurrent(rc);

    VxColor color(1.0f, 1.0f, 1.0f, 1.0f);
    if (mat)
        color = mat->GetDiffuse();
    CKDWORD col = RGBAFTOCOLOR(&color);

    Vx2DVector size(1.0f, 1.0f);
    beh->GetInputParameterValue(1, &size);

    Vx2DVector texscale(1.0f, 1.0f);
    beh->GetInputParameterValue(2, &texscale);

    float offset = 0.0f;
    beh->GetInputParameterValue(3, &offset);

    VxVector up(0.0f, 1.0f, 0.0f);
    beh->GetInputParameterValue(4, &up);

    CKBOOL lighting = FALSE;
    beh->GetInputParameterValue(5, &lighting);

    int rtc = 3;
    beh->GetInputParameterValue(6, &rtc);
    if (rtc < 3)
        rtc = 3;

    int renderingmethod = 1;
    beh->GetLocalParameterValue(0, &renderingmethod);

    // unuseful
    // rc->SetWorldTransformationMatrix( curve->GetWorldMatrix() );

    switch (renderingmethod)
    {
    case PLANE:
        RenderPlane(rc, curve, up, size, texscale, col, offset);
        break;
    case CROSS:
        RenderCross(rc, curve, up, size, texscale, col, offset);
        break;
    case TUBE:
        RenderTube(rc, curve, rtc, up, size, texscale, col, offset, lighting);
        break;
    case TWODIM:
        RenderTwoDim(rc, curve, size, texscale, col, offset);
        break;
    case LINE:
        RenderLine(rc, curve, col);
        break;
    }

    return 1;
}