/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Spherical Mapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSphericalMappingDecl();
CKERROR CreateSphericalMappingProto(CKBehaviorPrototype **);
int SphericalMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSphericalMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Spherical Mapping");
    od->SetDescription("Maps an Object according to the Spherical.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.<BR>
    <SPAN CLASS=pin>Mapping Referential: </SPAN>the referential conducting the projection.
    NULL means a spherical mapping around the world center.<BR>
    <SPAN CLASS=pin>Wrap Correction: </SPAN>if TRUE, a wrapping correction will be calculate in order to avoid cyclic mapping compression artifact.
    Otherwise projection will be only proportionnal to the angle around the cylinder.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5e40778e, 0x44f36cf3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSphericalMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreateSphericalMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Spherical Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Mapping Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("UV Tile", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Wrap Correction", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SphericalMapping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SphericalMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CK3dEntity *mapper = (CK3dEntity *)beh->GetInputParameterObject(1);

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    // Vertices
    CKDWORD vStride;
    VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vStride);
    int verticescount = mesh->GetVertexCount();

    // UVS
    int channel = -1;
    beh->GetInputParameterValue(0, &channel);
    CKDWORD uStride;
    VxUV *uvs = (VxUV *)mesh->GetTextureCoordinatesPtr(&uStride, channel);
    if (!uvs)
        return CKBR_OK;

    Vx2DVector tiling(1.0f, 1.0f);
    beh->GetInputParameterValue(2, &tiling);

    ///
    // Mapping Referential Transformation
    // Tranform Vertex Position from mesh coordinates system to mapper coordinate system

    // data point to a memory block allocated just once for all DrawPrimitive purpose
    VxDrawPrimitiveData *data = behcontext.CurrentRenderContext->GetDrawPrimitiveStructure(CKRST_DP_TR_VNT, verticescount);

    // Transfo Matrix creation
    VxMatrix mat2;
    if (mapper)
    {
        Vx3DMultiplyMatrix(mat2, mapper->GetInverseWorldMatrix(), ent->GetWorldMatrix());
    }
    else
    {
        mat2 = ent->GetWorldMatrix();
    }

    // The transfo itself
    VxStridedData dest, src;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    dest.Ptr = data->NormalPtr;
    dest.Stride = data->NormalStride;
    src.Ptr = vertices;
    src.Stride = vStride;
    Vx3DMultiplyMatrixVectorStrided(&dest, &src, mat2, verticescount);

    VxVector *pos = (VxVector *)data->NormalPtr;
    CKDWORD pStride = data->NormalStride;
#else
    dest.Ptr = data->Normals.Ptr;
    dest.Stride = data->Normals.Stride;
    src.Ptr = vertices;
    src.Stride = vStride;
    Vx3DMultiplyMatrixVectorStrided(&dest, &src, mat2, verticescount);

    VxVector *pos = (VxVector *)data->Normals.Ptr;
    CKDWORD pStride = data->Normals.Stride;
#endif

    float inv2pi = 0.5f / PI;
    tiling.x *= inv2pi;
    tiling.y *= 2.0f * inv2pi;

    // Wrap Correction
    CKBOOL wrap = TRUE;
    beh->GetInputParameterValue(3, &wrap);

    ///
    // Uvs Calculation
    if (wrap)
    {
        for (int i = 0; i < verticescount; ++i, pos = (VxVector *)((CKBYTE *)pos + pStride),
                 uvs = (VxUV *)((CKBYTE *)uvs + uStride))
        {
            pos->Normalize();
            if (pos->y >= 1.0f)
                pos->y = 1.0f;
            if (pos->y <= -1.0f)
                pos->y = -1.0f;

            float angle1 = HALFPI - asinf(pos->y);
            float angle2 = HALFPI - atan2f(pos->x, pos->z);
            float u = angle2 * tiling.x;
            uvs->u = u - int(u);
            uvs->v = angle1 * tiling.y;
        } // for
    }
    else
    {
        for (int i = 0; i < verticescount; ++i, pos = (VxVector *)((CKBYTE *)pos + pStride),
                 uvs = (VxUV *)((CKBYTE *)uvs + uStride))
        {
            pos->Normalize();
            if (pos->y >= 1.0f)
                pos->y = 1.0f;
            if (pos->y <= -1.0f)
                pos->y = -1.0f;

            float angle1 = HALFPI - asinf(pos->y);
            float angle2 = HALFPI - atan2f(pos->x, pos->z);

            uvs->u = angle2 * tiling.x;
            uvs->v = angle1 * tiling.y;
        } // for
    }

    mesh->UVChanged();

    return CKBR_OK;
}
