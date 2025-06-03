/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Planar Mapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPlanarMappingDecl();
CKERROR CreatePlanarMappingProto(CKBehaviorPrototype **);
int PlanarMapping(const CKBehaviorContext &behcontext);

int GetSinglePassMaxTextureNumber(CKContext *ctx);

CKObjectDeclaration *FillBehaviorPlanarMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Planar Mapping");
    od->SetDescription("Maps an Object according to the Planar.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.<BR>
    <SPAN CLASS=pin>Mapping Referential: </SPAN>the referential conducting the projection, along its Z axis.
    NULL means a planar mapping along the Z world axis.<BR>
    <SPAN CLASS=pin>UV Tile: </SPAN>Tiling of the projected texture, determines also the size of the
    texture projected if the material is in clamp mode.<BR>
    <BR>
    <SPAN CLASS=setting>Exclude Faces: </SPAN>Check this option if you want faces not concerned by the projection
    to be excluded (to avoid for example projection taking place on the backfacing faces of a sphere on which you project).
    This option works only if the material is clamped and if you work on a channel, not on the
    primary mesh (because faces can't be excluded from the primary mesh.)
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2a6a3fa3, 0x3280407d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlanarMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreatePlanarMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Planar Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Mapping Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("UV Tile", CKPGUID_2DVECTOR, "1,1");

    proto->DeclareSetting("Exclude Faces", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PlanarMapping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int PlanarMapping(const CKBehaviorContext &behcontext)
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

    tiling *= Vx2DVector(0.5f, 0.5f);

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

    int FaceCount = mesh->GetFaceCount();
    CKWORD *Indices = mesh->GetFacesIndices();
    CKDWORD NormStride = 0;
    CKBYTE *FaceNormal = mesh->GetFaceNormalsPtr(&NormStride);

    // Get 3D Caps (mainly for single pass rendering)
    CKBOOL NoMultiPassSupport = GetSinglePassMaxTextureNumber(behcontext.Context) < 2;

    CKMaterial *mat = mesh->GetChannelMaterial(channel);

    CKBOOL excludefaces = FALSE;
    beh->GetLocalParameterValue(0, &excludefaces);

    if ((channel >= 0) &&
        (excludefaces) &&
        (mat && mat->GetTextureAddressMode() == VXTEXTURE_ADDRESSCLAMP))
    {
        //____________________________________
        // Clip not to be rendered faces

        CKDWORD *VtxFlags = new CKDWORD[verticescount];
        memset(VtxFlags, 0, verticescount * sizeof(CKDWORD));

        // Valid vertex must have positive Z and 0 < u&v <1
        int i;
        for (i = 0; i < verticescount; ++i, pos = (VxVector *)((CKBYTE *)pos + pStride),
            uvs = (VxUV *)((CKBYTE *)uvs + uStride))
        {
            *uvs = *(VxUV *)pos;

            if (pos->z < 0)
                VtxFlags[i] = VXCLIP_FRONT;
            if (uvs->u < 0)
                VtxFlags[i] |= VXCLIP_LEFT;
            if (uvs->u > 1.0f)
                VtxFlags[i] |= VXCLIP_RIGHT;
            if (uvs->v < 0)
                VtxFlags[i] |= VXCLIP_TOP;
            if (uvs->v > 1.0f)
                VtxFlags[i] |= VXCLIP_BOTTOM;
        }

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        int ChannelMask = CKCHANNELMASK(channel);
        pos = (VxVector *)data->NormalPtr;

        VxVector MapperZ = VxVector::axisZ();
        ent->InverseTransformVector(&MapperZ, &MapperZ, mapper);

        for (i = 0; i < FaceCount; ++i, Indices += 3, FaceNormal += NormStride)
        {
            // Face is correctly oriented to be mapped , check if
            // all its vertices are inside the projection frustum
            if ((VtxFlags[Indices[0]] & VtxFlags[Indices[1]] & VtxFlags[Indices[2]]))
            {
                mesh->ChangeFaceChannelMask(i, 0, ChannelMask);
            }
            else
            {
                // test for DotProduct between projection axis
                if (DotProduct(*(VxVector *)FaceNormal, MapperZ) < 0)
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
        unsigned short *Indices = mesh->GetFacesIndices();

        VxVector MapperZ = VxVector::axisZ();
        ent->InverseTransformVector(&MapperZ,&MapperZ,mapper);

        pos = (VxVector*)data->Normals.Ptr;
        XArray<WORD> ChannelIndices;
        for ( i=0; i < FaceCount; ++i, Indices+=3, FaceNormal+=NormStride) {
            // Face is correctly oriented to be mapped, check if
            // all its vertices are inside the projection frustrum
            if (!(VtxFlags[Indices[0]]&VtxFlags[Indices[1]]&VtxFlags[Indices[2]])
                && (DotProduct(*(VxVector *)FaceNormal,MapperZ)<0)) {
                ChannelIndices.PushBack(Indices[0]);
                ChannelIndices.PushBack(Indices[1]);
                ChannelIndices.PushBack(Indices[2]);
                }
        }
        mesh->SetChannelFaceIndices(channel,ChannelIndices.Size(), ChannelIndices.Begin());
#endif

        delete[] VtxFlags;
    }
    else
    {
        //_______________________________________
        // Don't clip to not be rendered faces

        VxCopyStructure(verticescount, uvs, uStride, sizeof(VxUV), pos, pStride);
    }

    mesh->UVChanged();

    return CKBR_OK;
}
