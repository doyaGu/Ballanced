/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Screen Mapping 2
//
// To work on devices that does not support non-perspective correct
// textures , we need to transform vertices on screen,
// and render them setting their w component to 1 to
// avoid perspective correction...
//
// NOTE : For the moment this only work for single material
//    meshes and these meshes are automatically rendered without
//    lighting (full white blended with the material texture)
//
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorScreenMapping2Decl();
CKERROR CreateScreenMapping2Proto(CKBehaviorPrototype **);
int ScreenMapping2(const CKBehaviorContext &behcontext);
int ScreenMapping2RenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument);
CKERROR ScreenMapping2CallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorScreenMapping2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Screen Mapping2");
    od->SetDescription("Maps an Object according to the screen.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Screen UVs: </SPAN>the texture coordinates coresponding to the screen region.<BR>
    <SPAN CLASS=pin>Rotation: </SPAN>angle to which rotate the projection.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ab440ea, 0x49831b03));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateScreenMapping2Proto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Materials-Textures/Mapping");
    return od;
}

CKERROR CreateScreenMapping2Proto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Screen Mapping2");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Screen UVs", CKPGUID_RECT, "(0.0,0.0),(1.0,1.0)");
    proto->DeclareInParameter("Rotation", CKPGUID_ANGLE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ScreenMapping2);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    proto->SetBehaviorCallbackFct(ScreenMapping2CallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR ScreenMapping2CallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
        if (beh->GetTarget())
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            ent->RemoveRenderCallBack();
        }
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        CKScene *scene = behcontext.CurrentScene;
        if (!(beh->IsParentScriptActiveInScene(scene)))
        {
            CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
            if (!ent)
                return 0;
            ent->RemoveRenderCallBack();
        }
    }
    break;
    }
    return CKBR_OK;
}

int ScreenMapping2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    ent->SetRenderCallBack(ScreenMapping2RenderCallBack, beh);
    return CKBR_OK;
}

/****************************************************************************
Replace standard rendering, by a callback that transform
every vertex to screen before rendering ,
the w coordinates of these vertex which is normally used for
texture perspective correction is forced to 1  to avoid perspective correction
******************************************************************************/
int ScreenMapping2RenderCallBack(CKRenderContext *Dev, CKRenderObject *ent, void *Argument)
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
    beh->GetInputParameterValue(0, &Rect);

    //------------- Rotation Angle
    float Angle = 0.0f;
    beh->GetInputParameterValue(1, &Angle);

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
            uvs->v = ((Rect.bottom - positions->y) * invheight) - 0.5f;
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
