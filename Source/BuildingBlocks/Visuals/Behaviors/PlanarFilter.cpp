/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            PlanarFilter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPlanarFilterDecl();
CKERROR CreatePlanarFilterProto(CKBehaviorPrototype **pproto);
int PlanarFilter(const CKBehaviorContext &behcontext);
CKERROR PlanarFilterCallback(const CKBehaviorContext &behcontext);
void PlanarFilterRender(CKRenderContext *rc, void *arg);

CKObjectDeclaration *FillBehaviorPlanarFilterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Planar Filter");
    od->SetDescription("Sets an filter color and an additionnal color to recover all the scene.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Filtering Color: </SPAN>color by which the texture colors are multiplied... e.g : to see red components of the texture, set filtering color to (255, 0, 0, 128).<BR>
    Alpha component is the fourth member of the filtering color.<BR>
    <BR>
    <SPAN CLASS=pin>Additional Color: </SPAN>This is the color which is added to the texture color.<BR>
    <SPAN CLASS=pin>Texture: </SPAN>texture that will be used in the filter
    <SPAN CLASS=pin>Source Blend: </SPAN>This is the mode for source blending. (see 'SetSrcBlend' for more details on blending mode)<BR>
    <SPAN CLASS=pin>Dest Blend: </SPAN>This is the mode for destination blending. (see 'SetDestBlend' for more details on blending mode)<BR>
    <SPAN CLASS=pin>Texture Mag Mode: </SPAN>Texture Filtering Mode when it is magnified<BR>
    <SPAN CLASS=pin>Texture Min Mode: </SPAN>Texture Filtering Mode when it is minimized<BR>
    <BR>
    Useful to simulate infrared glasses or fade in/fade out effects.<BR>
    This Behavior doesn't need loop.<BR>
    */
    od->SetCategory("Visuals/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00cd320b, 0x32ed010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlanarFilterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreatePlanarFilterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Planar Filter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Filtering Color", CKPGUID_COLOR, "255, 255, 255, 128");
    proto->DeclareInParameter("Additional Color", CKPGUID_COLOR, "0, 0, 0, 0");
    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "Source Alpha");
    proto->DeclareInParameter("Dest Blend", CKPGUID_BLENDFACTOR, "Inverse Source Alpha");
    proto->DeclareInParameter("Texture Mag Mode", CKPGUID_FILTERMODE, "Linear");
    proto->DeclareInParameter("Texture Min Mode", CKPGUID_FILTERMODE, "Linear");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PlanarFilter);

    proto->SetBehaviorCallbackFct(PlanarFilterCallback);

    *pproto = proto;
    return CK_OK;
}

int PlanarFilter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (!beh)
        return CKBR_BEHAVIORERROR;

    if (beh->IsInputActive(1))
    { // we enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(PlanarFilterRender, beh);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // we enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(PlanarFilterRender, beh);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR PlanarFilterCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORNEWSCENE:
    {
        CKLevel *level = behcontext.CurrentLevel;
        if (ctx->IsPlaying() && beh->IsActive() && beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(PlanarFilterRender, beh);
        }
        else
        {
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(PlanarFilterRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        if (beh->GetOwner())
        {
            //----- Remove Sort Function
            if (behcontext.CurrentRenderContext)
                behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(PlanarFilterRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        CKLevel *level = behcontext.CurrentLevel;
        if (beh->IsActive())
        {
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(PlanarFilterRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
        CKLevel *level = behcontext.CurrentLevel;
        if (beh->IsActive() && beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(PlanarFilterRender, beh);
        }
    }
    break;
    }
    return CKBR_OK;
}

void PlanarFilterRender(CKRenderContext *rc, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;

    // we get the input parameters
    VxColor fcolor;
    beh->GetInputParameterValue(0, &fcolor);

    VxColor acolor;
    beh->GetInputParameterValue(1, &acolor);

    CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(2);

    VXBLEND_MODE srcblend;
    beh->GetInputParameterValue(3, &srcblend);

    VXBLEND_MODE destblend;
    beh->GetInputParameterValue(4, &destblend);

    VXTEXTURE_FILTERMODE txmag;
    beh->GetInputParameterValue(5, &txmag);

    VXTEXTURE_FILTERMODE txmin;
    beh->GetInputParameterValue(6, &txmin);

    // SET STATES
    // we don't let write to the ZBuffer
    rc->SetTexture(tex);
    rc->SetState(VXRENDERSTATE_FOGENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_WRAP0, 0);
    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_SRCBLEND, srcblend);
    rc->SetState(VXRENDERSTATE_DESTBLEND, destblend);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);
    rc->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATEALPHA);
    rc->SetTextureStageState(CKRST_TSS_MINFILTER, txmin);
    rc->SetTextureStageState(CKRST_TSS_MAGFILTER, txmag);
    rc->SetState(VXRENDERSTATE_SPECULARENABLE, TRUE);
    rc->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);
    rc->SetTextureStageState(CKRST_TSS_TEXTURETRANSFORMFLAGS, 0);
    rc->SetTextureStageState(CKRST_TSS_TEXCOORDINDEX, 0);

    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure((CKRST_DPFLAGS)(CKRST_DP_VCST), 4);

    // Positions
    VxRect rect;
    rc->GetViewRect(rect);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    ((VxVector4 *)data->PositionPtr)[0].x = rect.left;
    ((VxVector4 *)data->PositionPtr)[0].y = rect.top;
    ((VxVector4 *)data->PositionPtr)[0].z = 0.0f;
    ((VxVector4 *)data->PositionPtr)[0].w = 1.0f;
    ((VxVector4 *)data->PositionPtr)[1].x = rect.right;
    ((VxVector4 *)data->PositionPtr)[1].y = rect.top;
    ((VxVector4 *)data->PositionPtr)[1].z = 0.0f;
    ((VxVector4 *)data->PositionPtr)[1].w = 1.0f;
    ((VxVector4 *)data->PositionPtr)[2].x = rect.right;
    ((VxVector4 *)data->PositionPtr)[2].y = rect.bottom;
    ((VxVector4 *)data->PositionPtr)[2].z = 0.0f;
    ((VxVector4 *)data->PositionPtr)[2].w = 1.0f;
    ((VxVector4 *)data->PositionPtr)[3].x = rect.left;
    ((VxVector4 *)data->PositionPtr)[3].y = rect.bottom;
    ((VxVector4 *)data->PositionPtr)[3].z = 0.0f;
    ((VxVector4 *)data->PositionPtr)[3].w = 1.0f;
#else
    ((VxVector4 *)data->Positions.Ptr)[0].x = rect.left;
    ((VxVector4 *)data->Positions.Ptr)[0].y = rect.top;
    ((VxVector4 *)data->Positions.Ptr)[0].z = 0.0f;
    ((VxVector4 *)data->Positions.Ptr)[0].w = 1.0f;
    ((VxVector4 *)data->Positions.Ptr)[1].x = rect.right;
    ((VxVector4 *)data->Positions.Ptr)[1].y = rect.top;
    ((VxVector4 *)data->Positions.Ptr)[1].z = 0.0f;
    ((VxVector4 *)data->Positions.Ptr)[1].w = 1.0f;
    ((VxVector4 *)data->Positions.Ptr)[2].x = rect.right;
    ((VxVector4 *)data->Positions.Ptr)[2].y = rect.bottom;
    ((VxVector4 *)data->Positions.Ptr)[2].z = 0.0f;
    ((VxVector4 *)data->Positions.Ptr)[2].w = 1.0f;
    ((VxVector4 *)data->Positions.Ptr)[3].x = rect.left;
    ((VxVector4 *)data->Positions.Ptr)[3].y = rect.bottom;
    ((VxVector4 *)data->Positions.Ptr)[3].z = 0.0f;
    ((VxVector4 *)data->Positions.Ptr)[3].w = 1.0f;
#endif

    // Colors
    CKDWORD col = RGBAFTOCOLOR(&fcolor);
    CKDWORD cols = RGBAFTOCOLOR(&acolor);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxFillStructure(4, data->ColorPtr, data->ColorStride, 4, &col);
    VxFillStructure(4, data->SpecularColorPtr, data->SpecularColorStride, 4, &cols);

    // UVs
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
    uvs->u = 0.0f;
    uvs->v = 0.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
    uvs->u = 1.0f;
    uvs->v = 0.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
    uvs->u = 1.0f;
    uvs->v = 1.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
    uvs->u = 0.0f;
    uvs->v = 1.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoordStride);
#else
    VxFillStructure(4, data->Colors.Ptr, data->Colors.Stride, 4, &col);
    VxFillStructure(4, data->SpecularColors.Ptr, data->SpecularColors.Stride, 4, &cols);

    // UVs
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
    uvs->u = 0.0f;
    uvs->v = 0.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
    uvs->u = 1.0f;
    uvs->v = 0.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
    uvs->u = 1.0f;
    uvs->v = 1.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
    uvs->u = 0.0f;
    uvs->v = 1.0f;
    uvs = (VxUV *)((CKBYTE *)uvs + data->TexCoord.Stride);
#endif

    // indices
    CKWORD *indices = rc->GetDrawPrimitiveIndices(4);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    // the drawing itself
    rc->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, data);

    // we let write to the ZBuffer
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_ZENABLE, TRUE);
    // Reenable Fog
    if (rc->GetFogMode() != VXFOG_NONE)
        rc->SetState(VXRENDERSTATE_FOGENABLE, TRUE);
}
