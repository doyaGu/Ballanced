/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CameraColorFilter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCameraColorFilterDecl();
CKERROR CreateCameraColorFilterProto(CKBehaviorPrototype **pproto);
int CameraColorFilter(const CKBehaviorContext &behcontext);
void CameraColorFilterRender(CKRenderContext *rc, void *arg);

CKObjectDeclaration *FillBehaviorCameraColorFilterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Camera Color Filter");
    od->SetDescription("Adds a color filter to the Camera.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the building block has been activated by "On".<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the building block has been activated by "Off".<BR>
    <BR>
    <SPAN CLASS=pin>Filter Color: </SPAN>color of the filter in RGBA.<BR>
    <SPAN CLASS=pin>Density: </SPAN>how dense the filter should be, in percent.<BR>
    <SPAN CLASS=pin>Hard Light: </SPAN>if FALSE, the filter color is added to the colors of the scene. If TRUE, the filter color is combined with the colors of the scene (resulting in greater opacity).<BR>
    <BR>
    This building block is useful to simulate infrared glasses or fade in/fade out effects.<BR>
    This building block does not need to be looped.<BR>
    */
    od->SetCategory("Cameras/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x00cd010b, 0x00dd010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCameraColorFilterProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateCameraColorFilterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Camera Color Filter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Filter Color", CKPGUID_COLOR, "255,0,0,0");
    proto->DeclareInParameter("Density", CKPGUID_PERCENTAGE, "20");
    proto->DeclareInParameter("Hard Light", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CameraColorFilter);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int CameraColorFilter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (!beh)
        return CKBR_BEHAVIORERROR;

    if (beh->IsInputActive(1))
    { // we enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    { // we enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    behcontext.CurrentRenderContext->AddPostRenderCallBack(CameraColorFilterRender, (void *)beh->GetID(), TRUE);
    // behcontext.CurrentRenderContext->AddPostRenderCallBack(CameraColorFilterRender,beh,TRUE);

    return CKBR_ACTIVATENEXTFRAME;
}

void CameraColorFilterRender(CKRenderContext *rc, void *arg)
{

    CKBehavior *beh = (CKBehavior *)CKGetObject(rc->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;
    //	CKBehavior* beh = (CKBehavior*)arg;

    if (rc->GetAttachedCamera() != beh->GetTarget())
        return;

    // we get the input parameters
    VxColor fcolor(255, 0, 0, 0);
    beh->GetInputParameterValue(0, &fcolor);

    // we get the density of the atmosphere
    float density = 20.0f;
    beh->GetInputParameterValue(1, &density);

    // we change the material according to the bool
    CKBOOL hardl = FALSE;
    beh->GetInputParameterValue(2, &hardl);

    if (hardl)
    {
        rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
        rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
        fcolor.a = density;
    }
    else
    {
        rc->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_ONE);
        rc->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_ONE);
        fcolor *= density;
        fcolor.a = 1.0f;
    }

    // SET STATES
    // we don't let write to the ZBuffer
    rc->SetTexture(NULL);
    rc->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    rc->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);

    VxDrawPrimitiveData *data = rc->GetDrawPrimitiveStructure(CKRST_DP_VCT, 4);

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

    // colors
#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    CKDWORD *colors = (CKDWORD *)data->ColorPtr;
    CKDWORD col = RGBAFTOCOLOR(&fcolor);
    VxFillStructure(4, colors, data->ColorStride, 4, &col);
#else
    CKDWORD *colors = (CKDWORD *)data->Colors.Ptr;
    CKDWORD col = RGBAFTOCOLOR(&fcolor);
    VxFillStructure(4, colors, data->Colors.Stride, 4, &col);
#endif

    // indices
    CKWORD *indices = rc->GetDrawPrimitiveIndices(4);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    rc->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, data);

    // we let write to the ZBuffer
    rc->SetState(VXRENDERSTATE_ZWRITEENABLE, TRUE);
    rc->SetState(VXRENDERSTATE_ZENABLE, TRUE);
}
