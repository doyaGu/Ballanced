/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Screen Mapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorScreenMappingDecl();
CKERROR CreateScreenMappingProto(CKBehaviorPrototype **);
int ScreenMappingPreRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument);
int ScreenMappingPostRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument);
int ScreenMappingRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument);
int ScreenMapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorScreenMappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Screen Mapping");
    od->SetDescription("Maps an Object according to the screen.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Channel: </SPAN>if set to -1, the mapping is calculated only for the default material.
    Otherwise it is processed on the specified channel.<BR>
    <SPAN CLASS=pin>Screen UVs: </SPAN>the texture coordinates coresponding to the screen region.<BR>
    <SPAN CLASS=pin>Rotation: </SPAN>angle to which rotate the projection.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x58ef12ca, 0x26805940));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateScreenMappingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreateScreenMappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Screen Mapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Channel", CKPGUID_INT, "-1");
    proto->DeclareInParameter("Screen UVs", CKPGUID_RECT, "(0.0,0.0),(1.0,1.0)");
    proto->DeclareInParameter("Rotation", CKPGUID_ANGLE);
    proto->DeclareSetting("Disable Perspective Correction", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ScreenMapping);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ScreenMapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_OK;

    CKBOOL forcenocorrect = FALSE;
    beh->GetLocalParameterValue(0, &forcenocorrect);

    if (forcenocorrect)
    {
        if (mesh->GetMaterialCount() != 1)
        {
            behcontext.Context->OutputToConsole("There must be only one material for disabling perspective correction");
            return CKBR_GENERICERROR;
        }
        // add render callbacks
        ent->AddPreRenderCallBack(ScreenMappingPreRenderCallBack, beh, TRUE);
        ent->AddPostRenderCallBack(ScreenMappingPostRenderCallBack, NULL, TRUE);
        return CKBR_OK;
    }

    // Vertices
    CKDWORD vStride;
    VxVector *vertices = (VxVector *)mesh->GetPositionsPtr(&vStride);
    int verticescount = mesh->GetVertexCount();

    // Faces
    int facescount = mesh->GetFaceCount();

    ///
    // UV Calculation

    CKRenderContext *dev = behcontext.CurrentRenderContext;
    VxDrawPrimitiveData *data = dev->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VNT, verticescount);

    VxTransformData tdata;
    tdata.InVertices = vertices;
    tdata.InStride = vStride;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    tdata.OutVertices = data->NormalPtr;
    tdata.OutStride = data->NormalStride;
    tdata.ScreenVertices = data->PositionPtr;
    tdata.ScreenStride = data->PositionStride;
#else
    tdata.OutVertices = data->Normals.Ptr;
    tdata.OutStride = data->Normals.Stride;
    tdata.ScreenVertices = data->Positions.Ptr;
    tdata.ScreenStride = data->Positions.Stride;
#endif
    tdata.ClipFlags = NULL;

    dev->TransformVertices(verticescount, &tdata, ent);

    VxRect rect(0.0, 0.0, 1.0f, 1.0f);
    beh->GetInputParameterValue(1, &rect);

    float angle = 0.0f;
    beh->GetInputParameterValue(2, &angle);

    float invwidth = rect.GetWidth() * 0.5f;
    float invheight = rect.GetHeight() * 0.5f;

    CKCamera *camera = dev->GetAttachedCamera();
    CKBOOL perspective = TRUE;
    if (camera->GetProjectionType() == CK_ORTHOGRAPHICPROJECTION)
        perspective = FALSE;

    int channel = -1;
    beh->GetInputParameterValue(0, &channel);
    CKDWORD uStride;
    VxUV *uvs = (VxUV *)mesh->GetTextureCoordinatesPtr(&uStride, channel);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxVector *positions = (VxVector *)data->NormalPtr;
#else
    VxVector *positions = (VxVector *)data->Normals.Ptr;
#endif

    if (XAbs(angle) > EPSILON) // Rotation desired
    {
        // UVS
        float cosa = cosf(angle);
        float sina = sinf(angle);

        Vx2DVector vhx, vhy;

        float invz = 1.0f;
        for (int i = 0; i < verticescount; ++i)
        {

            if (perspective)
                invz = (float)fabs(1.0f / positions->z);
            uvs->u = rect.left + positions->x * invwidth * invz;
            uvs->v = rect.top - positions->y * invheight * invz;
            vhx.x = cosa * uvs->u;
            vhx.y = -sina * uvs->u;
            vhy.x = sina * uvs->v;
            vhy.y = cosa * uvs->v;
            uvs->u = -0.5f + vhx.x + vhy.x;
            uvs->v = -0.5f + vhx.y + vhy.y;

            uvs = (VxUV *)((CKBYTE *)uvs + uStride);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            positions = (VxVector *)((CKBYTE *)positions + data->NormalStride);
#else
            positions = (VxVector *)((CKBYTE *)positions + data->Normals.Stride);
#endif
        }
    }
    else // No rotation
    {
        float invz = 1.0f;
        for (int i = 0; i < verticescount; ++i)
        {

            if (perspective)
                invz = (float)fabs(1.0f / positions->z);
            uvs->u = -0.5f + rect.left + positions->x * invwidth * invz;
            uvs->v = -0.5f + rect.top - positions->y * invheight * invz;

            uvs = (VxUV *)((CKBYTE *)uvs + uStride);
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            positions = (VxVector *)((CKBYTE *)positions + data->NormalStride);
#else
            positions = (VxVector *)((CKBYTE *)positions + data->Normals.Stride);
#endif
        }
    }

    mesh->UVChanged();

    return CKBR_OK;
}

/****************************************************************************
This callback sets the render callback of the entity
****************************************************************************/
int ScreenMappingPreRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument)
{
    CKBehavior *beh = (CKBehavior *)Argument;
    ent->SetRenderCallBack(ScreenMappingRenderCallBack, beh);

    return 1;
}

/****************************************************************************
This callback removes the render callback of the entity
****************************************************************************/
int ScreenMappingPostRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument)
{
    ent->RemoveRenderCallBack();

    return 1;
}

/****************************************************************************
Replace standard rendering, by a callback that transform
every vertex to screen before rendering ,
the w coordinates of these vertex which is normally used for
texture perspective correction is forced to 1  to avoid perspective correction
******************************************************************************/
int ScreenMappingRenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument)
{
    CK3dEntity *Ent = (CK3dEntity *)ent;
    CKMesh *mesh = Ent->GetCurrentMesh();
    CKBehavior *beh = (CKBehavior *)Argument;
    if (!mesh)
        return 0;

    //--- Get Mesh Vertices
    CKDWORD vStride;
    VxVector *Vertices = (VxVector *)mesh->GetPositionsPtr(&vStride);
    int VertexCount = mesh->GetVertexCount();

    // Faces
    int Facescount = mesh->GetFaceCount();
    CKWORD *Indices = mesh->GetFacesIndices();

    VxDrawPrimitiveData *data = Dev->GetDrawPrimitiveStructure(CKRST_DP_VCT, VertexCount);

    VxTransformData tdata;
    tdata.InVertices = Vertices;
    tdata.InStride = vStride;
    tdata.OutVertices = NULL;
    tdata.OutStride = 0;
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    tdata.ScreenVertices = data->PositionPtr;
    tdata.ScreenStride = data->PositionStride;
#else
    tdata.ScreenVertices = data->Positions.Ptr;
    tdata.ScreenStride = data->Positions.Stride;
#endif
    tdata.ClipFlags = NULL;

    Dev->TransformVertices(VertexCount, &tdata, Ent);

    //------------- Mapping rectangle
    VxRect Rect(0.0, 0.0, 1.0f, 1.0f);
    beh->GetInputParameterValue(1, &Rect);

    //------------- Rotation Angle
    float Angle = 0.0f;
    beh->GetInputParameterValue(2, &Angle);

    //------ Viewport Rect
    VxRect ViewRect(0.0, 0.0, 1.0f, 1.0f);
    Dev->GetViewRect(ViewRect);
    Rect.TransformFromHomogeneous(ViewRect);

    float invwidth = 1.0f / Rect.GetWidth();
    float invheight = 1.0f / Rect.GetHeight();

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    VxVector4 *positions = (VxVector4 *)data->PositionPtr;
    CKDWORD *Colors = (CKDWORD *)data->ColorPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    VxVector4 *positions = (VxVector4 *)data->Positions.Ptr;
    CKDWORD *Colors = (CKDWORD *)data->Colors.Ptr;
#endif

    if (XAbs(Angle) > EPSILON) // Rotation desired
    {
        // UVS
        float cosa = cosf(Angle);
        float sina = sinf(Angle);

        Vx2DVector vhx, vhy;

        float invz = 1.0f;
        for (int i = 0; i < VertexCount; ++i)
        {

            uvs->u = ((positions->x - Rect.left) * invwidth) - 0.5f;
            uvs->v = ((positions->y - Rect.bottom) * invheight) - 0.5f;
            vhx.x = cosa * uvs->u;
            vhx.y = -sina * uvs->u;
            vhy.x = sina * uvs->v;
            vhy.y = cosa * uvs->v;
            uvs->u = 0.5f + vhx.x + vhy.x;
            uvs->v = 0.5f + vhx.y + vhy.y;
            positions->w = 1.0f;
            *Colors = 0xFFFFFFFF;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
            positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
            Colors = (CKDWORD *)((CKBYTE *)Colors + data->ColorStride);
#else
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
            positions = (VxVector4 *)((CKBYTE *)positions + data->Positions.Stride);
            Colors = (CKDWORD *)((CKBYTE *)Colors + data->Colors.Stride);
#endif
        }
    }
    else // No rotation
    {
        for (int i = 0; i < VertexCount; ++i)
        {

            uvs->u = ((positions->x - Rect.left) * invwidth);
            uvs->v = ((positions->y - Rect.top) * invheight);
            positions->w = 1.0f;
            *Colors = 0xFFFFFFFF;

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
            positions = (VxVector4 *)((CKBYTE *)positions + data->PositionStride);
            Colors = (CKDWORD *)((CKBYTE *)Colors + data->ColorStride);
#else
            uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
            positions = (VxVector4 *)((CKBYTE *)positions + data->Positions.Stride);
            Colors = (CKDWORD *)((CKBYTE *)Colors + data->Colors.Stride);
#endif
        }
    }

    CKMaterial *Mat = mesh->GetMaterial(0);

    Mat->SetAsCurrent(Dev, FALSE);
    Dev->DrawPrimitive(VX_TRIANGLELIST, Indices, Facescount * 3, data);

    return 1;
}
