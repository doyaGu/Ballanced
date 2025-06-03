/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            MouseCursor
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorMouseCursorDecl();
CKERROR CreateMouseCursorProto(CKBehaviorPrototype **);
int MouseCursor(const CKBehaviorContext &behcontext);
CKERROR MouseCursorCallbackDisp(const CKBehaviorContext &behcontext);
void MouseCursorRender(CKRenderContext *dev, void *arg);

CKObjectDeclaration *FillBehaviorMouseCursorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Mouse Cursor");
    od->SetDescription("Display a texture instead of the standard cursor.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates it.<BR>
    <SPAN CLASS=in>Off: </SPAN>disables it.<BR>
    <BR>
    <SPAN CLASS=in>Cursor Texture: </SPAN>texture to use to represent the mouse cursor.<BR>
    <SPAN CLASS=in>Cursor Size: </SPAN>size in screen coordinates of the mouse cursor.<BR>
    <SPAN CLASS=in>Cursor Color: </SPAN>color and transparency of the cursor.<BR>
    <SPAN CLASS=in>Shadow: </SPAN>either to draw a shadow under the cursor.<BR>
    <SPAN CLASS=in>Shadow Distance: </SPAN>distance of the shadow in screen coordinates.<BR>
    <SPAN CLASS=in>Shadow Angle: </SPAN>angle of projection of the shadow.<BR>
    <SPAN CLASS=in>Shadow Intensity: </SPAN>intensity of the shadow.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x140c3eac, 0x6e502a81));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateMouseCursorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Interface/Screen");
    return od;
}

CKERROR CreateMouseCursorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Mouse Cursor");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareInParameter("Cursor Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Cursor Size", CKPGUID_2DVECTOR, "32,32");
    proto->DeclareInParameter("Cursor Color", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Shadow", CKPGUID_BOOL);
    proto->DeclareInParameter("Shadow Distance", CKPGUID_FLOAT, "4.0");
    proto->DeclareInParameter("Shadow Angle", CKPGUID_ANGLE, "0:120");
    proto->DeclareInParameter("Shadow Intensity", CKPGUID_PERCENTAGE, "30");
    // TODO : center - left - right
    // TODO : center - top - bottom
    // TODO : offset
    // TODO : setting = screen relative

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(MouseCursor);

    proto->SetBehaviorCallbackFct(MouseCursorCallbackDisp);

    *pproto = proto;
    return CK_OK;
}

int MouseCursor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1)) // we enter by 'OFF'
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
        if (im)
        {
            im->ShowCursor(TRUE);
        }

        return CKBR_OK;
    }

    if (beh->IsInputActive(0)) // we enter by 'ON'
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
        if (im) im->ShowCursor(FALSE);
    }
    behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(MouseCursorRender, (void *)behcontext.Behavior->GetID(), TRUE);
    return CKBR_ACTIVATENEXTFRAME;
}

/*******************************************************/
/******************** INIT CALLBACK ********************/
CKERROR MouseCursorCallbackDisp(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    {
        CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
        if (im)
            im->ShowCursor(TRUE);
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    case CKM_BEHAVIORRESUME:
    case CKM_BEHAVIORACTIVATESCRIPT:
    {
        // TODO : foire dans le load !!!!!!!!!
        if (beh->IsParentScriptActiveInScene(behcontext.CurrentScene))
        {
            if (beh->IsActive())
            {
                CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
                if (im)
                    im->ShowCursor(FALSE);
            }
        }
    }
    break;
    }
    return CKBR_OK;
}

void MouseCursorRender(CKRenderContext *dev, void *arg)
{
    CKBehavior *beh = (CKBehavior *)CKGetObject(dev->GetCKContext(), (CK_ID)arg);
    if (!beh)
        return;
    CKContext *ctx = beh->GetCKContext();

    CKTexture *cursortex = (CKTexture *)beh->GetInputParameterObject(0);

    // SET STATES
    // we don't let write to the ZBuffer
    dev->SetTexture(cursortex);
    dev->SetTextureStageState(CKRST_TSS_TEXTUREMAPBLEND, VXTEXTUREBLEND_MODULATEALPHA);
    dev->SetState(VXRENDERSTATE_CULLMODE, VXCULL_NONE);
    dev->SetState(VXRENDERSTATE_SRCBLEND, VXBLEND_SRCALPHA);
    dev->SetState(VXRENDERSTATE_DESTBLEND, VXBLEND_INVSRCALPHA);
    dev->SetState(VXRENDERSTATE_ALPHABLENDENABLE, TRUE);
    dev->SetState(VXRENDERSTATE_ZENABLE, FALSE);
    dev->SetState(VXRENDERSTATE_ZWRITEENABLE, FALSE);

    CKWORD indices[4] = {0, 1, 2, 3};
    float vertices[16] = {0.0f, 0.0f, 0.5f, 1.0f, 640.0f, 0.0f, 0.5f, 1.0f, 0.0f, 480.0f, 0.5f, 1.0f, 640.0f, 480.0f, 0.5f, 1.0f};
    CKDWORD colors[4];
    float uv[8] = {0, 0, 1, 0, 1, 1, 0, 1};

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    VxDrawPrimitiveDataSimple data = {
        4,
        CKRST_DP_STAGES0,
        vertices,          // A pointer to position data (VxVector if transformation should be applied, VxVector4(xs,ys,zs,rhw) if screen coordinates)
        sizeof(VxVector4), // Amount in bytes between two positions in the PositionPtr bufffer
        NULL,              // A pointer to normals data
        0,                 // ...
        colors,            // A pointer to colors data
        sizeof(CKDWORD),     // ...
        NULL,              // A pointer to specular colors data
        0,                 // ...
        uv,                // A pointer to texture coordinates  data
        sizeof(float) * 2, // ...
    };
#else
    VxDrawPrimitiveDataSimple data;
    data.VertexCount = 4;
    data.Flags = CKRST_DP_STAGES0;
    data.Positions.Set(vertices, sizeof(VxVector4));
    data.Normals.Set(NULL, 0);
    data.Colors.Set(colors, sizeof(CKDWORD));
    data.SpecularColors.Set(NULL, 0);
    data.TexCoord.Set(uv, sizeof(float) * 2);
#endif

    // we get the input parameters

    Vx2DVector cursorsize;
    beh->GetInputParameterValue(1, &cursorsize);

    VxColor cursorcolor;
    beh->GetInputParameterValue(2, &cursorcolor);

    /////////////////////////////
    // Mouse position

    Vx2DVector mousePos;
    CKInputManager *input = (CKInputManager *)ctx->GetManagerByGuid(INPUT_MANAGER_GUID);
    input->GetMousePosition(mousePos, FALSE);

    VxRect rect;
    rect.left = mousePos.x;
    rect.top = mousePos.y;
    rect.right = rect.left + cursorsize.x;
    rect.bottom = rect.top + cursorsize.y;

    ///////////////////////////////
    // Shadow

    CKBOOL shadow = FALSE;
    beh->GetInputParameterValue(3, &shadow);
    if (shadow)
    {
        float shadowdistance = 4.0f;
        beh->GetInputParameterValue(4, &shadowdistance);

        float shadowangle = 1.0f;
        beh->GetInputParameterValue(5, &shadowangle);

        float shadowintensity = 0.8f;
        beh->GetInputParameterValue(6, &shadowintensity);

        float offsetx = 4.0f;
        float offsety = 4.0f;

        offsetx = -shadowdistance * cosf(shadowangle);
        offsety = shadowdistance * sinf(shadowangle);

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
        ((VxVector4 *)data.PositionPtr)[0].x = rect.left + offsetx;
        ((VxVector4 *)data.PositionPtr)[0].y = rect.top + offsety;
        ((VxVector4 *)data.PositionPtr)[1].x = rect.right + offsetx;
        ((VxVector4 *)data.PositionPtr)[1].y = rect.top + offsety;
        ((VxVector4 *)data.PositionPtr)[2].x = rect.right + offsetx;
        ((VxVector4 *)data.PositionPtr)[2].y = rect.bottom + offsety;
        ((VxVector4 *)data.PositionPtr)[3].x = rect.left + offsetx;
        ((VxVector4 *)data.PositionPtr)[3].y = rect.bottom + offsety;
#else
        ((VxVector4 *)data.Positions.Ptr)[0].x = rect.left + offsetx;
        ((VxVector4 *)data.Positions.Ptr)[0].y = rect.top + offsety;
        ((VxVector4 *)data.Positions.Ptr)[1].x = rect.right + offsetx;
        ((VxVector4 *)data.Positions.Ptr)[1].y = rect.top + offsety;
        ((VxVector4 *)data.Positions.Ptr)[2].x = rect.right + offsetx;
        ((VxVector4 *)data.Positions.Ptr)[2].y = rect.bottom + offsety;
        ((VxVector4 *)data.Positions.Ptr)[3].x = rect.left + offsetx;
        ((VxVector4 *)data.Positions.Ptr)[3].y = rect.bottom + offsety;
#endif

        for (int i = 0; i < 4; i++)
        {
            colors[i] = RGBAFTOCOLOR(0.0f, 0.0f, 0.0f, shadowintensity);
        }
        dev->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, (VxDrawPrimitiveData *)&data);
    }

    ////////////////
    // Cursor

#if CKVERSION == 0x13022002 || CKVERSION == 0x05082002
    ((VxVector4 *)data.PositionPtr)[0].x = rect.left;
    ((VxVector4 *)data.PositionPtr)[0].y = rect.top;
    ((VxVector4 *)data.PositionPtr)[1].x = rect.right;
    ((VxVector4 *)data.PositionPtr)[1].y = rect.top;
    ((VxVector4 *)data.PositionPtr)[2].x = rect.right;
    ((VxVector4 *)data.PositionPtr)[2].y = rect.bottom;
    ((VxVector4 *)data.PositionPtr)[3].x = rect.left;
    ((VxVector4 *)data.PositionPtr)[3].y = rect.bottom;
#else
    ((VxVector4 *)data.Positions.Ptr)[0].x = rect.left;
    ((VxVector4 *)data.Positions.Ptr)[0].y = rect.top;
    ((VxVector4 *)data.Positions.Ptr)[1].x = rect.right;
    ((VxVector4 *)data.Positions.Ptr)[1].y = rect.top;
    ((VxVector4 *)data.Positions.Ptr)[2].x = rect.right;
    ((VxVector4 *)data.Positions.Ptr)[2].y = rect.bottom;
    ((VxVector4 *)data.Positions.Ptr)[3].x = rect.left;
    ((VxVector4 *)data.Positions.Ptr)[3].y = rect.bottom;
#endif

    CKDWORD col = RGBAFTOCOLOR(&cursorcolor);

    for (int i = 0; i < 4; i++)
    {
        colors[i] = col;
    }

    dev->DrawPrimitive(VX_TRIANGLEFAN, indices, 4, (VxDrawPrimitiveData *)&data);
}
