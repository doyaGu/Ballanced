/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ShowBoundingBox
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorShowBoundingBoxDecl();
CKERROR CreateShowBoundingBoxProto(CKBehaviorPrototype **pproto);
int ShowBoundingBox(const CKBehaviorContext &behcontext);
CKBOOL ShowBoundingBoxRenderCallBack(CKRenderContext *rc, CKRenderObject *obj, void *arg);

CKObjectDeclaration *FillBehaviorShowBoundingBoxDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Show Object Information");
    od->SetDescription("Shows the bounding box and the normals of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Show Bounding Box: </SPAN>shows or hides the bounding box.<BR>
    <SPAN CLASS=pin>Bounding Box Color: </SPAN>color in RGBA.<BR>
    <SPAN CLASS=pin>Local: </SPAN>specifies the local box or the world box.<BR>
    <SPAN CLASS=pin>Hierarchical Box: </SPAN>specifies if hierarchical box is to be shown.<BR>
    <SPAN CLASS=pin>Show Vertex Normals: </SPAN>shows or hides the vertices normals.<BR>
    <SPAN CLASS=pin>Vertices Normals Colors: </SPAN>color in RGBA.<BR>
    <SPAN CLASS=pin>Vertices Normals Size: </SPAN>size of the normals drawn, in world coordinates.<BR>
    <SPAN CLASS=pin>Show Face Normals: </SPAN>shows or hides the faces normals.<BR>
    <SPAN CLASS=pin>Face Normals Colors: </SPAN>color in RGBA.<BR>
    <SPAN CLASS=pin>Face Normals Size: </SPAN>size of the normals drawn, in world coordinates.<BR>
    <BR>
    */
    /*	warning:
    - You have to loop this behavior if you want the object information to be draw for several frames.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x17cb4c57, 0xf525fb));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShowBoundingBoxProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateShowBoundingBoxProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Show Object Information");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Show Bounding Box", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Bounding Box color", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Local", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Hierarchical Box", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Show Vertex Normals", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Vertex Normals Color", CKPGUID_COLOR, "0,0,255,255");
    proto->DeclareInParameter("Vertex Normals Size", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Show Face Normals", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Face Normals Color", CKPGUID_COLOR, "0,255,255,255");
    proto->DeclareInParameter("Face Normals Size", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Show Bounding Sphere", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Bounding Sphere color", CKPGUID_COLOR, "255,0,0,255");

    proto->DeclareSetting("Vertex Normal Material", CKPGUID_MATERIAL);
    proto->DeclareSetting("Face Normal Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(ShowBoundingBox);

    *pproto = proto;
    return CK_OK;
}

int ShowBoundingBox(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (ent)
    {
        ent->AddPostRenderCallBack(ShowBoundingBoxRenderCallBack, beh, TRUE);
        // CKRenderContext* rc = behcontext.CurrentRenderContext;
        // rc->AddPostRenderCallBack(ShowBoundingBoxRenderCallBack,(void *)beh->GetID(),TRUE);
    }
    return CKBR_OK;
}

CKBOOL
ShowBoundingBoxRenderCallBack(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;
    CK3dEntity *ent = (CK3dEntity *)obj;
    if (!beh || !ent)
        return FALSE;

    // Projection Trick
    VxMatrix &projmat = (VxMatrix &)rc->GetProjectionTransformationMatrix();
    float oldproj = projmat[3].z;
    projmat[3].z = oldproj * 1.008f; // to disable Z buffer fighting
    rc->SetProjectionTransformationMatrix(projmat);

    CKBOOL DrawBBox;
    beh->GetInputParameterValue(0, &DrawBBox);

    CKBOOL Local;
    CKBOOL Hierachical;
    beh->GetInputParameterValue(2, &Local);
    beh->GetInputParameterValue(3, &Hierachical);

    // Bounding Box
    if (DrawBBox)
    {
        VxColor color;
        beh->GetInputParameterValue(1, &color);

        const VxBbox &Box = Hierachical ? ent->GetHierarchicalBox(Local) : ent->GetBoundingBox(Local);

        rc->SetTexture(NULL);

        if (Local)
            rc->SetWorldTransformationMatrix(ent->GetWorldMatrix());
        else
            rc->SetWorldTransformationMatrix(VxMatrix::Identity());

        // Data
        VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, 10);

        // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        CKDWORD *colors = (CKDWORD *)data->ColorPtr;
        CKDWORD col = RGBAFTOCOLOR(&color);
        VxFillStructure(10, colors, data->ColorStride, 4, &col);

        // Positions
        VxVector Min = Box.Min, Max = Box.Max;
        ((VxVector4 *)data->PositionPtr)[0] = Min;
        ((VxVector4 *)data->PositionPtr)[1].Set(Max.x, Min.y, Min.z, 1.0f);
        ((VxVector4 *)data->PositionPtr)[2].Set(Max.x, Max.y, Min.z, 1.0f);
        ((VxVector4 *)data->PositionPtr)[3].Set(Min.x, Max.y, Min.z, 1.0f);

        ((VxVector4 *)data->PositionPtr)[4].Set(Min.x, Min.y, Max.z, 1.0f);
        ((VxVector4 *)data->PositionPtr)[5].Set(Max.x, Min.y, Max.z, 1.0f);
        ((VxVector4 *)data->PositionPtr)[6] = Max;
        ((VxVector4 *)data->PositionPtr)[7].Set(Min.x, Max.y, Max.z, 1.0f);
#else
        CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
        CKDWORD col = RGBAFTOCOLOR(&color);
        VxFillStructure(10, colors, data->Colors.Stride, 4, &col);

        // Positions
        VxVector Min = Box.Min, Max = Box.Max;
        ((VxVector4 *)data->Positions.Ptr)[0] = Min;
        ((VxVector4 *)data->Positions.Ptr)[1].Set(Max.x, Min.y, Min.z, 1.0f);
        ((VxVector4 *)data->Positions.Ptr)[2].Set(Max.x, Max.y, Min.z, 1.0f);
        ((VxVector4 *)data->Positions.Ptr)[3].Set(Min.x, Max.y, Min.z, 1.0f);

        ((VxVector4 *)data->Positions.Ptr)[4].Set(Min.x, Min.y, Max.z, 1.0f);
        ((VxVector4 *)data->Positions.Ptr)[5].Set(Max.x, Min.y, Max.z, 1.0f);
        ((VxVector4 *)data->Positions.Ptr)[6] = Max;
        ((VxVector4 *)data->Positions.Ptr)[7].Set(Min.x, Max.y, Max.z, 1.0f);
#endif
        // Indices
        CKWORD indices[24] = {0, 1, 0, 3, 2, 1, 2, 3, 2, 6, 6, 7, 6, 5, 5, 1, 5, 4, 4, 7, 4, 0, 7, 3};
        rc->DrawPrimitive(VX_LINELIST, indices, 24, data);
    }

    CKBOOL vNormal = FALSE;
    beh->GetInputParameterValue(4, &vNormal);

    CKMesh *mesh = ent->GetCurrentMesh();

    // Vertices Normals
    if (vNormal && mesh)
    {
        VxColor color;
        beh->GetInputParameterValue(5, &color);

        float size = 1.0f;
        beh->GetInputParameterValue(6, &size);

        CKMaterial *mat = (CKMaterial *)beh->GetLocalParameterObject(0);

        int vcount = mesh->GetVertexCount();

        rc->SetTexture(NULL);

        rc->SetWorldTransformationMatrix(ent->GetWorldMatrix());

        CKDWORD flags = CKRST_DP_TR_CL_VC;
        if (mat)
            flags = CKRST_DP_TR_CL_VNT & ~CKRST_DP_STAGES0;

        // Data
        VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure((CKRST_DPFLAGS)flags, vcount * 2);

        CKDWORD pS;
        CKBYTE *positions = (CKBYTE *)mesh->GetPositionsPtr(&pS);
        CKDWORD nS;
        CKBYTE *normals = (CKBYTE *)mesh->GetNormalsPtr(&nS);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        XPtrStrided<VxVector> dpp(data->PositionPtr, data->PositionStride);
        XPtrStrided<VxVector> dpn(data->NormalPtr, data->NormalStride);
#else
        XPtrStrided<VxVector> dpp(data->Positions.Ptr, data->Positions.Stride);
        XPtrStrided<VxVector> dpn(data->Normals.Ptr, data->Normals.Stride);
#endif
        for (int i = 0; i < vcount; ++i, positions += pS, normals += nS)
        {
            VxVector *v = (VxVector *)positions;
            VxVector *n = (VxVector *)normals;
            dpp[i * 2] = *v;
            dpp[i * 2 + 1] = *v + size * (*n);
            if (mat)
            {
                dpn[i * 2] = Normalize(*n);
                dpn[i * 2 + 1] = dpn[i * 2];
            }
        }

        if (!mat)
        {
            // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            CKDWORD *colors = (CKDWORD *)data->ColorPtr;
            CKDWORD col = RGBAFTOCOLOR(&color);
            VxFillStructure(vcount * 2, colors, data->ColorStride, 4, &col);
#else
            CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
            CKDWORD col = RGBAFTOCOLOR(&color);
            VxFillStructure(vcount * 2, colors, data->Colors.Stride, 4, &col);
#endif
        }

        if (vcount)
        {
            if (mat)
                mat->SetAsCurrent(rc);

            rc->DrawPrimitive(VX_LINELIST, NULL, vcount * 2, data);
        }
    }

    CKBOOL fNormal = FALSE;
    beh->GetInputParameterValue(7, &fNormal);

    // Faces Normals
    if (fNormal && mesh)
    {
        VxColor color;
        beh->GetInputParameterValue(8, &color);

        float size = 1.0f;
        beh->GetInputParameterValue(9, &size);

        CKMaterial *mat = (CKMaterial *)beh->GetLocalParameterObject(1);

        int fcount = mesh->GetFaceCount();
        int vcount = mesh->GetVertexCount();

        rc->SetTexture(NULL);

        rc->SetWorldTransformationMatrix(ent->GetWorldMatrix());

        CKDWORD flags = CKRST_DP_TR_CL_VC;
        if (mat)
            flags = CKRST_DP_TR_CL_VNT & ~CKRST_DP_STAGES0;

        // Data
        VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure((CKRST_DPFLAGS)flags, fcount * 2);

        CKDWORD pS;
        CKBYTE *positions = (CKBYTE *)mesh->GetPositionsPtr(&pS);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        XPtrStrided<VxVector> dpp(data->PositionPtr, data->PositionStride);
        XPtrStrided<VxVector> dpn(data->NormalPtr, data->NormalStride);
#else
        XPtrStrided<VxVector> dpp(data->Positions.Ptr, data->Positions.Stride);
        XPtrStrided<VxVector> dpn(data->Normals.Ptr, data->Normals.Stride);
#endif

        for (int i = 0; i < fcount; ++i)
        {
            VxVector n = mesh->GetFaceNormal(i);
            VxVector v = (mesh->GetFaceVertex(i, 0) + mesh->GetFaceVertex(i, 1) + mesh->GetFaceVertex(i, 2)) * 0.33333f;
            dpp[i * 2] = v;
            dpp[i * 2 + 1] = v + size * n;

            if (mat)
            {
                dpn[i * 2] = v;
                dpn[i * 2 + 1] = v;
            }
        }

        if (!mat)
        {
            // Colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            CKDWORD *colors = (CKDWORD *)data->ColorPtr;
            CKDWORD col = RGBAFTOCOLOR(&color);
            VxFillStructure(fcount * 2, colors, data->ColorStride, 4, &col);
#else
            CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
            CKDWORD col = RGBAFTOCOLOR(&color);
            VxFillStructure(fcount * 2, colors, data->Colors.Stride, 4, &col);
#endif
        }

        if (fcount)
        {
            if (mat)
                mat->SetAsCurrent(rc);

            rc->DrawPrimitive(VX_LINELIST, NULL, fcount * 2, data);
        }
    }

    CKBOOL showSphere = FALSE;
    beh->GetInputParameterValue(10, &showSphere);

    if (showSphere && mesh)
    {

        rc->SetTexture(NULL);

        rc->SetWorldTransformationMatrix(ent->GetWorldMatrix());

        VxColor color;
        beh->GetInputParameterValue(11, &color);

        int count = 32;

        // Data
        VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VC, count);

        VxVector center;
        mesh->GetBaryCenter(&center);

        float radius = mesh->GetRadius();

        int axis0 = 0;
        int axis1 = 1;
        for (int i = 0; i < 3; ++i)
        {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
#else
            XPtrStrided<VxVector> positions(data->Positions.Ptr, data->Positions.Stride);
#endif
            // Positions
            float delta = PI * 2.0f / (count - 1);
            float t = 0.0f;
            for (int j = 0; j < count; ++j, t += delta)
            {
                positions[j] = center;
                positions[j][axis0] += radius * cosf(t);
                positions[j][axis1] += radius * sinf(t);
            }

            // Colors
            CKDWORD col = RGBAFTOCOLOR(&color);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            VxFillStructure(count, data->ColorPtr, data->ColorStride, 4, &col);
#else
            VxFillStructure(count, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

            // Drawing
            rc->DrawPrimitive(VX_LINESTRIP, NULL, count, data);

            ++axis0;
            ++axis1;
            if (axis1 == 3)
                axis1 = 0;
        }
    }

    projmat[3].z = oldproj; // restore projection matrix
    rc->SetProjectionTransformationMatrix(projmat);

    return TRUE;
}
