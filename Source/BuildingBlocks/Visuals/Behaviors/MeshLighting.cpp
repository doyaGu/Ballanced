/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MeshLighting
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMeshLightingDecl();
CKERROR CreateMeshLightingProto(CKBehaviorPrototype **pproto);
int MeshLighting(const CKBehaviorContext &behcontext);
CKERROR initcallback(const CKBehaviorContext &behcontext);
int render(CKRenderContext *rc, CKRenderObject *ent, void *arg);

CKObjectDeclaration *FillBehaviorMeshLightingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Vertex Render");
    od->SetDescription("Renders the object with a specific sprite at the vertices positions.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the building block.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the building block.<BR>
    <BR>
    <SPAN CLASS=out>Out (Loop): </SPAN>is activated when one behavior's process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Light Texture: </SPAN>name of the texture used as the sprite.<BR>
    <SPAN CLASS=pin>Light Size: </SPAN>float, 1.0 = actual size.<BR>
    <SPAN CLASS=pin>Enable Fog Influence: </SPAN>specify weither the rendering of each little light should be influenced by the fog values or not.<BR>
    <BR>
    If 'Light Size' is set to 1.0, it means the original size of the texture. 2.0 means double size ...etc.<BR>
    This behavior works best with small textures.<BR>
    Textures should always be square.<BR>
    */
    /* warning:
    - If you want to change either the 'Light Texture' or the 'Ligt Size' at each frame, you'll need to loop this behavior. Otherwise you needn't.<BR>
    - These building blocks replace the object's rendering function. Therefore if you'd like to stop this type of rendering, you'll have to activate the 'OFF' input.<BR>
    - In Fact the 'ON' and 'OFF' switch from both objet's rendering mode.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xcc030377, 0x897aaaaa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMeshLightingProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateMeshLightingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Vertex Render");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Out (Loop)");

    proto->DeclareInParameter("Light Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Light Size", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Enable Fog Influence", CKPGUID_BOOL, "TRUE");

    proto->DeclareLocalParameter("Light Sprite3D", CKPGUID_SPRITE3D);
    proto->DeclareLocalParameter("Light Material", CKPGUID_MATERIAL);
    proto->DeclareLocalParameter("Is Lighting", CKPGUID_BOOL);

    proto->DeclareSetting("Use Vertex Color", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Use Vertex Weight", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MeshLighting);

    proto->SetBehaviorCallbackFct(initcallback);

    *pproto = proto;
    return CK_OK;
}

int MeshLighting(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();

    CKBOOL is_lighting;
    beh->GetLocalParameterValue(2, &is_lighting);

    if (!is_lighting)
    {
        ent->SetRenderCallBack(render, beh);

        // we tell the render manager to render this object last
        CKMesh *mesh = ent->GetCurrentMesh();
        mesh->SetTransparent(TRUE);

        is_lighting = TRUE;
        beh->SetLocalParameterValue(2, &is_lighting);
    }

    if (beh->IsInputActive(0))
    { // We enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);

        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(0);
        CKMaterial *ckmat = (CKMaterial *)beh->GetLocalParameterObject(1);

        ckmat->SetTexture0(tex);
    }

    if (is_lighting)
    {
        if (beh->IsInputActive(1))
        { // We enter by 'OFF'
            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(0, TRUE);

            ent->RemoveRenderCallBack();

            CKMesh *mesh = ent->GetCurrentMesh();
            mesh->SetTransparent(FALSE);

            is_lighting = FALSE;
            beh->SetLocalParameterValue(2, &is_lighting);

            return CKBR_OK;
        }
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR initcallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKBOOL is_lighting = FALSE;
        beh->SetLocalParameterValue(2, &is_lighting);

        CKContext *ctx = behcontext.Context;
        // We create the Material of the sprite
        CKMaterial *ckmat = (CKMaterial *)ctx->CreateObject(CKCID_MATERIAL, "MLMaterial", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        if (!ckmat)
            return 0;
        beh->SetLocalParameterObject(1, ckmat);

        ckmat->EnableAlphaBlend();
        ckmat->SetSourceBlend(VXBLEND_ONE);
        ckmat->SetDestBlend(VXBLEND_ONE);
        ckmat->EnableZWrite(FALSE);
        ckmat->SetTextureAddressMode(VXTEXTURE_ADDRESSCLAMP);
        ckmat->SetTextureBlendMode(VXTEXTUREBLEND_MODULATEALPHA);

        // We apply the initial texture to the Material
        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(0);
        if (!tex)
            return 0;

        ckmat->SetTexture0(tex);
    }
    break;

    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
        CKBOOL is_lighting;
        if (beh->GetTarget())
        {
            beh->GetLocalParameterValue(2, &is_lighting);

            if (is_lighting)
            {
                is_lighting = FALSE;
                beh->SetLocalParameterValue(2, &is_lighting);
                CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
                if (!ent)
                    return 0;
                ent->RemoveRenderCallBack();

                CKMesh *mesh = ent->GetCurrentMesh();
                if (!mesh)
                    return 0;
                mesh->SetTransparent(FALSE);
            }
        }
    }
    break;

    case CKM_BEHAVIORNEWSCENE:
    {
        CKScene *scene = behcontext.CurrentScene;
        if (!(beh->IsParentScriptActiveInScene(scene)))
        {

            CKBOOL is_lighting;
            beh->GetLocalParameterValue(2, &is_lighting);

            if (is_lighting)
            {
                is_lighting = FALSE;
                beh->SetLocalParameterValue(2, &is_lighting);
                CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
                if (!ent)
                    return 0;
                ent->RemoveRenderCallBack();

                CKMesh *mesh = ent->GetCurrentMesh();
                if (!mesh)
                    return 0;
                mesh->SetTransparent(FALSE);
            }
        }
    }
    break;
    }
    return CKBR_OK;
}

/*******************************************************/
/****************** RENDER FUNCTION ********************/
int render(CKRenderContext *rc, CKRenderObject *obj, void *arg)
{
    CK3dEntity *ent = (CK3dEntity *)obj;
    CKMesh *ckmesh = ent->GetCurrentMesh();
    if (!ckmesh)
        return 0;

    CKBehavior *beh = (CKBehavior *)arg;
    if (!beh)
        return 0;

    CKBOOL useColor = FALSE;
    beh->GetLocalParameterValue(3, &useColor);

    CKBOOL useWeight = FALSE;
    beh->GetLocalParameterValue(4, &useWeight);

    CKBOOL enable_fog = TRUE;
    beh->GetInputParameterValue(2, &enable_fog);

    int count = ckmesh->GetVertexCount();
    if (!count)
        return 0;

    CKDWORD Stride;

    CKMaterial *material = (CKMaterial *)beh->GetLocalParameterObject(1);

    if (useColor)
    {
        material->SetTextureBlendMode(VXTEXTUREBLEND_MODULATEALPHA);
    }
    else
    {
        material->SetTextureBlendMode(VXTEXTUREBLEND_DECAL);
    }

    material->SetAsCurrent(rc, FALSE);
    rc->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);

    // Render States
    if (!enable_fog)
        rc->SetState(VXRENDERSTATE_FOGENABLE, FALSE);

    CK3dEntity *cam = rc->GetViewpoint();
    VxVector up, right;
    cam->GetOrientation(NULL, &up, &right, ent);

    float size = 1.0f;
    beh->GetInputParameterValue(1, &size);

    up *= 0.5f * size;
    right *= 0.5f * size;

    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_TR_CL_VCT, 4 * count);

    if (!useColor)
    {
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        data->ColorPtr = NULL;
        data->ColorStride = 0;
#else
        data->Colors.Ptr = NULL;
        data->Colors.Stride = 0;
#endif
    }

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    XPtrStrided<VxVector> positions(data->PositionPtr, data->PositionStride);
    XPtrStrided<VxUV> uvs(data->TexCoordPtr, data->TexCoordStride);
#else
    XPtrStrided<VxVector> positions(data->Positions.Ptr, data->Positions.Stride);
    XPtrStrided<VxUV> uvs(data->TexCoord.Ptr, data->TexCoord.Stride);
#endif

    VxVector *pos = (VxVector *)ckmesh->GetPositionsPtr(&Stride);

    VxVector offset1 = -right + up;
    VxVector offset2 = +right + up;
    VxVector offset3 = +right - up;
    VxVector offset4 = -right - up;

    if (useWeight && ckmesh->GetVertexWeightsPtr())
    {
        float *weights = ckmesh->GetVertexWeightsPtr();

        for (int a = 0; a < count; ++a)
        {

            // Positions
            *positions = *pos + weights[a] * offset1;
            ++positions;
            *positions = *pos + weights[a] * offset2;
            ++positions;
            *positions = *pos + weights[a] * offset3;
            ++positions;
            *positions = *pos + weights[a] * offset4;
            ++positions;

            // Uvs
            *uvs = VxUV(0.0f, 0.0f);
            ++uvs;
            *uvs = VxUV(1.0f, 0.0f);
            ++uvs;
            *uvs = VxUV(1.0f, 1.0f);
            ++uvs;
            *uvs = VxUV(0.0f, 1.0f);
            ++uvs;

            pos = (VxVector *)((CKBYTE *)pos + Stride);
        }
    }
    else
    {
        for (int a = 0; a < count; ++a)
        {

            // Positions
            *positions = *pos + offset1;
            ++positions;
            *positions = *pos + offset2;
            ++positions;
            *positions = *pos + offset3;
            ++positions;
            *positions = *pos + offset4;
            ++positions;

            // Uvs
            *uvs = VxUV(0.0f, 0.0f);
            ++uvs;
            *uvs = VxUV(1.0f, 0.0f);
            ++uvs;
            *uvs = VxUV(1.0f, 1.0f);
            ++uvs;
            *uvs = VxUV(0.0f, 1.0f);
            ++uvs;

            pos = (VxVector *)((CKBYTE *)pos + Stride);
        }
    }

    ////////////////////////////
    // Colors

    if (useColor)
    {
        CKDWORD cStride;
        CKDWORD *cptr = (CKDWORD *)ckmesh->GetColorsPtr(&cStride);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        VxCopyStructure(count, data->ColorPtr, 4 * data->ColorStride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->ColorPtr + 1, 4 * data->ColorStride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->ColorPtr + 2, 4 * data->ColorStride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->ColorPtr + 3, 4 * data->ColorStride, sizeof(CKDWORD), cptr, cStride);
#else
        VxCopyStructure(count, data->Colors.Ptr, 4 * data->Colors.Stride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->Colors.Ptr + 1, 4 * data->Colors.Stride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->Colors.Ptr + 2, 4 * data->Colors.Stride, sizeof(CKDWORD), cptr, cStride);
        VxCopyStructure(count, (CKDWORD *)data->Colors.Ptr + 3, 4 * data->Colors.Stride, sizeof(CKDWORD), cptr, cStride);
#endif
    }

    ////////////////////////////
    // Indices
    CKWORD *indices = rc->GetDrawPrimitiveIndices(6 * count);

    int ni = 0;
    int fi = 0;
    for (int i = 0; i < count; ++i)
    {
        indices[fi] = ni;
        indices[fi + 1] = ni + 1;
        indices[fi + 2] = ni + 2;
        indices[fi + 3] = ni;
        indices[fi + 4] = ni + 2;
        indices[fi + 5] = ni + 3;
        fi += 6;
        ni += 4;
    }

    // the Drawing
    rc->DrawPrimitive(VX_TRIANGLELIST, indices, fi, data);

    if (rc->GetFogMode() != VXFOG_NONE)
        rc->SetState(VXRENDERSTATE_FOGENABLE, TRUE);

    return 1;
}