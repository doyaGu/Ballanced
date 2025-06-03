/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Blur
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBlurDecl();
CKERROR CreateBlurProto(CKBehaviorPrototype **pproto);
int Blur(const CKBehaviorContext &behcontext);
CKERROR BlurCallbackDisp(const CKBehaviorContext &behcontext);
void BlurRender(CKRenderContext *rc, void *arg);

CKObjectDeclaration *FillBehaviorBlurDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Global Blur");
    od->SetDescription("Apply a global blur on all the objects rendered.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>enables the global blurring.<BR>
    <SPAN CLASS=in>Off: </SPAN>disables it.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>Clearing Material.<BR>
    */
    /* warning:
    - This behaviors works well only if you do not have a background, but objects floating in space. In fact,
    all it does is prevent the screen clear at each frames, and draw a transparent frame on top using the given material,
    or the background material if none is given before the render of the next frame.<BR>
    - This behavior will work better in 32bits because of colors imprecision in 16bits.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x37f1163d, 0x35ad3e6b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBlurProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/FX");
    return od;
}

CKERROR CreateBlurProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Global Blur");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Blur);

    proto->SetBehaviorCallbackFct(BlurCallbackDisp);

    *pproto = proto;
    return CK_OK;
}

void BlurOn(const CKBehaviorContext &behcontext)
{
    behcontext.CurrentRenderContext->SetClearBackground(FALSE);
    behcontext.CurrentRenderContext->AddPreRenderCallBack(BlurRender, behcontext.Behavior);
}

void BlurOff(const CKBehaviorContext &behcontext)
{
    if (behcontext.CurrentRenderContext)
    {
        behcontext.CurrentRenderContext->SetClearBackground(TRUE);
        behcontext.CurrentRenderContext->RemovePreRenderCallBack(BlurRender, behcontext.Behavior);
    }
}

int Blur(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // we enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        BlurOff(behcontext);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // we enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        BlurOn(behcontext);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR BlurCallbackDisp(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORNEWSCENE:
    {
        if (behcontext.Context->IsPlaying() && beh->IsActive() && beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            BlurOn(behcontext);
        }
        else
        {
            BlurOff(behcontext);
        }
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        if (beh->GetOwner())
        {
            BlurOff(behcontext);
        }
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        if (beh->IsActive())
        {
            BlurOff(behcontext);
        }
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
        if (beh->IsActive() && beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            BlurOn(behcontext);
        }
    }
    break;
    }
    return CKBR_OK;
}

void BlurRender(CKRenderContext *rc, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;

    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(0);
    if (!mat)
    {
        mat = rc->GetBackgroundMaterial();
    }

    mat->SetAsCurrent(rc);
    rc->SetTextureStageState(CKRST_TSS_STAGEBLEND, 0, 1);

    // SET STATES
    // we don't let write to the ZBuffer
    rc->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATEALPHA);

    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
    rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);

    // DrawPrimitive
    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_VCT, 4);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxUV *uvs = (VxUV *)data->TexCoordPtr;
#else
    VxUV *uvs = (VxUV *)data->TexCoord.Ptr;
#endif

    /////////////////
    // UVs

    // Uvs
    uvs[0].u = 0.0f;
    uvs[0].v = 0.0f;
    uvs[1].u = 1.0f;
    uvs[1].v = 0.0f;
    uvs[2].u = 1.0f;
    uvs[2].v = 1.0f;
    uvs[3].u = 0.0f;
    uvs[3].v = 1.0f;

    VxRect rect;
    rc->GetViewRect(rect);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    // Positions
    ((VxVector4 *)data->PositionPtr)[0].x = rect.left;
    ((VxVector4 *)data->PositionPtr)[0].y = rect.top;

    ((VxVector4 *)data->PositionPtr)[1].x = rect.right;
    ((VxVector4 *)data->PositionPtr)[1].y = rect.top;

    ((VxVector4 *)data->PositionPtr)[2].x = rect.right;
    ((VxVector4 *)data->PositionPtr)[2].y = rect.bottom;

    ((VxVector4 *)data->PositionPtr)[3].x = rect.left;
    ((VxVector4 *)data->PositionPtr)[3].y = rect.bottom;

    for (int i = 0; i < 4; ++i)
    {
        ((VxVector4 *)data->PositionPtr)[i].z = 0;
        ((VxVector4 *)data->PositionPtr)[i].w = 1.0f;
    }
    // Colors
    VxColor fcolor = mat->GetDiffuse();
    if (fcolor.a == 1.0f)
        fcolor.a = 0.9f;
    fcolor.a = 0.01f + (float)(rand() & 1023) * 0.0002f;
    CKDWORD col = RGBAFTOCOLOR(&fcolor);
    VxFillStructure(4, data->ColorPtr, data->ColorStride, 4, &col);
#else
    // Positions
    ((VxVector4 *)data->Positions.Ptr)[0].x = rect.left;
    ((VxVector4 *)data->Positions.Ptr)[0].y = rect.top;

    ((VxVector4 *)data->Positions.Ptr)[1].x = rect.right;
    ((VxVector4 *)data->Positions.Ptr)[1].y = rect.top;

    ((VxVector4 *)data->Positions.Ptr)[2].x = rect.right;
    ((VxVector4 *)data->Positions.Ptr)[2].y = rect.bottom;

    ((VxVector4 *)data->Positions.Ptr)[3].x = rect.left;
    ((VxVector4 *)data->Positions.Ptr)[3].y = rect.bottom;

    for (int i = 0; i < 4; ++i)
    {
        ((VxVector4 *)data->Positions.Ptr)[i].z = 0;
        ((VxVector4 *)data->Positions.Ptr)[i].w = 1.0f;
    }
    // Colors
    VxColor fcolor = mat->GetDiffuse();
    if (fcolor.a == 1.0f)
        fcolor.a = 0.9f;
    fcolor.a = 0.01f + (float)(rand() & 1023) * 0.0002f;
    CKDWORD col = RGBAFTOCOLOR(&fcolor);
    VxFillStructure(4, data->Colors.Ptr, data->Colors.Stride, 4, &col);
#endif

    CKWORD indices[4] = {0, 1, 2, 3};
    rc->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, data);
}
