/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Cylindrical Mapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCylindricalMappingDecl();
CKERROR CreateCylindricalMappingProto(CKBehaviorPrototype **);
int CylindricalMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCylindricalMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Cylindrical Mapping");
    od->SetDescription("Maps an Object according to the Cylindrical.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.<BR>
    <SPAN CLASS=pin>Mapping Referential: </SPAN>3D Entity defining the position of the mapping cylinder, and its orientation.<BR>
    <SPAN CLASS=pin>UV Tile: </SPAN>scaling of the mapping projection around the cylinder (U coordinate), and along the cylinder (V coordinate).<BR>
    <SPAN CLASS=pin>Wrap Correction: </SPAN>if TRUE, a wrapping correction will be calculate in order to avoid cyclic mapping compression artifact.
     Otherwise projection will be only proportionnal to the angle around the cylinder.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x770a5fc9, 0x7bbe2aed));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCylindricalMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreateCylindricalMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Cylindrical Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Mapping Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("UV Tile", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Wrap Correction", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CylindricalMapping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int CylindricalMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CK3dEntity *mapper = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!mapper)
        return CKBR_OWNERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_OWNERERROR;

    // Vertices
    CKDWORD vStride;
    VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vStride);
    int verticescount = mesh->GetVertexCount();
    if (!verticescount)
        return 0;

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

    VxDrawPrimitiveData *data = behcontext.CurrentRenderContext->GetDrawPrimitiveStructure(CKRST_DP_TR_VNT, verticescount);

    // Transfo Matrix creation
    VxMatrix mat2;
    Vx3DMultiplyMatrix(mat2, mapper->GetInverseWorldMatrix(), ent->GetWorldMatrix());

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
    tiling.y *= 0.5f;

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
            uvs->v = 0.5f - pos->y * tiling.y;
            pos->y = 0.0f;
            pos->Normalize();
            float angle = HALFPI - (float)atan2(pos->x, pos->z);
            float u = angle * tiling.x;
            uvs->u = u - (int)u;
        } // for
    }
    else
    {
        for (int i = 0; i < verticescount; ++i, pos = (VxVector *)((CKBYTE *)pos + pStride),
                 uvs = (VxUV *)((CKBYTE *)uvs + uStride))
        {
            uvs->v = 0.5f - pos->y * tiling.y;
            pos->y = 0.0f;
            pos->Normalize();
            float angle = HALFPI - (float)atan2(pos->x, pos->z);
            uvs->u = angle * tiling.x;
        } // for
    }

    mesh->UVChanged();

    return CKBR_OK;
}
