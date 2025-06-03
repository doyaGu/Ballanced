/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              TextureRender
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTextureRenderDecl();
CKERROR CreateTextureRenderProto(CKBehaviorPrototype **pproto);
int TextureRender(const CKBehaviorContext &behcontext);
CKERROR TextureRenderCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextureRenderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Texture Render");
    od->SetDescription("Render an image in the texture of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Reset: </SPAN>You need to reset this behavior when you have new objects entering your scene and you want them
    to be seen in the texture renderer. (Anyway you must reset this behavior at least one time.)<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Camera: </SPAN>camera from which the image should be rendered.<BR>
    <SPAN CLASS=pin>Background Color: </SPAN>background color of the rendering in RGBA.<BR>
    <SPAN CLASS=pin>Background Texture: </SPAN>name of the texture to use for the background of the rendering.<BR>
    <BR>
    <SPAN CLASS=setting>Render Options: </SPAN>render flags...<BR>
    <U>Background Sprites:</U> background 2D Sprites are to be rendered.<BR>
    <U>Foreground Sprites:</U> foreground 2D Sprites are to be rendered.<BR>
    <U>Update Extents:</U> Update the extents of the objects rendered (to allow future picking).<BR>
    <U>Use Camera Ratio:</U> render context size is automatically set using current camera aspect ratio and Window Rectangle (Horizontal Size has priority).<BR>
    <U>Clear ZBuffer:</U> clear ZBuffer.<BR>
    <U>Clear Back Buffer:</U> clear Back Buffer.<BR>
    <U>Clear Stencil Buffer:</U> clear Stencil Buffer (stencil buffer has not been yet implemented).<BR>
    <U>Back To Front:</U> cnable Buffers Swapping.<BR>
    <U>Clear Only Viewport:</U> do not clear entire device, only viewport (device=window, viewport=subpart in window where 3d objects are rendered [for instance think about 16/9 rendering]).<BR>
    <U>Wait VBL:</U> wait for Vertical Blank before blitting.<BR>
    <U>Disable 3D:</U> do not draw 3D scene (only 2d sprites).<BR>
    */
    /* warning:
    - You must apply this behavior on a textured object.<BR>
    */
    od->SetCategory("Visuals/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x67fc7084, 0x141154f7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateTextureRenderProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateTextureRenderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Texture Render");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Camera", CKPGUID_CAMERA);
    proto->DeclareInParameter("Background Color", CKPGUID_COLOR, "128,128,128,255");
    proto->DeclareInParameter("Background Texture", CKPGUID_TEXTURE);

    // "render context" (remains here for compatibility with previous version)
    proto->DeclareLocalParameter(NULL, CKPGUID_NONE);
    proto->DeclareSetting(NULL, CKPGUID_NONE); // old width
    proto->DeclareSetting(NULL, CKPGUID_NONE); // old height
    proto->DeclareSetting("Render Options", CKPGUID_RENDEROPTIONS, "Use Camera Ratio,Clear ZBuffer,Clear Back Buffer,Clear Stencil,Clear Only Viewport");
    proto->DeclareSetting("(CubeMap) Render Face by Face", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(TextureRender);
    proto->SetBehaviorCallbackFct(TextureRenderCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int TextureRender(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // We get the created render context
    CKTexture *target_tex = (CKTexture *)beh->GetTarget();
    if (!target_tex)
    {
        beh->GetCKContext()->OutputToConsole("No texture Provided.");
        return CKBR_OWNERERROR;
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKRenderContext *rc = behcontext.CurrentRenderContext;
    CKCamera *oldcam = rc->GetAttachedCamera();

    // we get the desired camera
    CKCamera *cam = (CKCamera *)beh->GetInputParameterObject(0);
    if (cam)
        rc->AttachViewpointToCamera(cam);

    // We set the background
    CKMaterial *mat = rc->GetBackgroundMaterial();

    // Get Background color
    VxColor col, OldCol;
    beh->GetInputParameterValue(1, &col);
    OldCol = mat->GetDiffuse();
    mat->SetDiffuse(col);

    // Get Background color
    CKTexture *tex, *oldtex = mat->GetTexture();
    tex = (CKTexture *)beh->GetInputParameterObject(2);
    mat->SetTexture0(tex);

    if (!target_tex->IsInVideoMemory())
        target_tex->SystemToVideoMemory(rc);

    if (target_tex->IsCubeMap())
    {
        CKBOOL RenderFaceByFace = FALSE;
        beh->GetLocalParameterValue(4, &RenderFaceByFace);

        float oldfov = cam->GetFov();
        int asx, asy;
        cam->GetAspectRatio(asx, asy);
        cam->SetFov(PI / 2.0f);
        cam->SetAspectRatio(1, 1);

        // we select render options
        CKDWORD ro = rc->GetCurrentRenderOptions();
        beh->GetLocalParameterValue(3, &ro);
        ro &= ~CK_RENDER_DOBACKTOFRONT;

        VxMatrix OldCamMat = cam->GetWorldMatrix();
        VxVector Pos = OldCamMat[3];

        VxMatrix Rotation[6] = {VxMatrix::Identity(), VxMatrix::Identity(), VxMatrix::Identity(), VxMatrix::Identity(), VxMatrix::Identity(), VxMatrix::Identity()};
        //----- Referential directed to X axis
        Rotation[0][0].Set(0.0f, 0.0f, -1.0f, 0.0f);
        Rotation[0][2].Set(1.0f, 0.0f, 0.0f, 0.0f);
        //----- Referential directed to -X axis
        Rotation[1][0].Set(0.0f, 0.0f, 1.0f, 0.0f);
        Rotation[1][2].Set(-1.0f, 0.0f, 0.0f, 0.0f);
        //----- Referential directed to Y axis
        Rotation[2][1].Set(0.0f, 0.0f, -1.0f, 0.0f);
        Rotation[2][2].Set(0.0f, 1.0f, 0.0f, 0.0f);
        //----- Referential directed to -Y axis
        Rotation[3][1].Set(0.0f, 0.0f, 1.0f, 0.0f);
        Rotation[3][2].Set(0.0f, -1.0f, 0.0f, 0.0f);
        //----- Referential directed to Z axis
        //----- Referential directed to -Z axis
        Rotation[5][0].Set(-1.0f, 0.0f, 0.0f, 0.0f);
        Rotation[5][2].Set(0.0f, 0.0f, -1.0f, 0.0f);

        if (RenderFaceByFace)
        {
            int Current = target_tex->GetCurrentSlot();

            Rotation[Current][3] = Pos;
            cam->SetWorldMatrix(Rotation[Current]);

            if (rc->SetRenderTarget(target_tex, Current))
            {
                // We can draw directly to texture
                rc->Render((CK_RENDER_FLAGS)ro);
                // And Restore rendering to default
                rc->SetRenderTarget(NULL);
            }
            target_tex->SetCurrentSlot((Current + 1) % 6);
        }
        else
        {
            for (int i = 0; i < 6; ++i)
            {
                // Set the correct direction for the camera
                Rotation[i][3] = Pos;
                cam->SetWorldMatrix(Rotation[i]);

                if (rc->SetRenderTarget(target_tex, i))
                {
                    // We can draw directly to texture
                    rc->Render((CK_RENDER_FLAGS)ro);
                    // And Restore rendering to default
                    rc->SetRenderTarget(NULL);
                }
            }
        }

        cam->SetWorldMatrix(OldCamMat);
        cam->SetFov(oldfov);
        cam->SetAspectRatio(asx, asy);
    }
    else
    {
        if (rc->SetRenderTarget(target_tex))
        {
            // we select render options
            CKDWORD ro = rc->GetCurrentRenderOptions();
            beh->GetLocalParameterValue(3, &ro);

            ro &= ~CK_RENDER_DOBACKTOFRONT;

            // We can draw directly to texture
            rc->Render((CK_RENDER_FLAGS)ro);

            // And Restore rendering to default
            rc->SetRenderTarget(NULL);
        }
    }

    if (cam)
        rc->AttachViewpointToCamera(oldcam);

    // restoring background attributes
    mat->SetTexture0(oldtex);
    mat->SetDiffuse(OldCol);

    return CKBR_OK;
}

// OLD CODE

int TextureRenderV1(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // We get the created render context
    CKTexture *target_tex = (CKTexture *)beh->GetOutputParameterObject(0);
    if (!target_tex)
    {
        beh->GetCKContext()->OutputToConsole("No texture Provided.");
        return CKBR_OWNERERROR;
    }

    beh->ActivateInput(0, FALSE);
    beh->ActivateInput(1, FALSE);
    beh->ActivateOutput(0);

    CKRenderContext *rc = behcontext.CurrentRenderContext;
    CKCamera *oldcam = rc->GetAttachedCamera();

    // we get the desired camera
    CKCamera *cam = (CKCamera *)beh->GetInputParameterObject(0);
    if (cam)
        rc->AttachViewpointToCamera(cam);

    // We set the background
    CKMaterial *mat = rc->GetBackgroundMaterial();

    // Get Background color
    VxColor col, OldCol;
    beh->GetInputParameterValue(1, &col);
    OldCol = mat->GetDiffuse();
    mat->SetDiffuse(col);

    // Get Background texture
    CKTexture *tex, *oldtex = mat->GetTexture();
    tex = (CKTexture *)beh->GetInputParameterObject(2);
    mat->SetTexture0(tex);

    if (rc->SetRenderTarget(target_tex))
    {
        // we select render options
        CKDWORD ro = rc->GetCurrentRenderOptions();
        beh->GetLocalParameterValue(3, &ro);

        ro &= ~CK_RENDER_DOBACKTOFRONT;

        // We can draw directly to texture
        rc->Render((CK_RENDER_FLAGS)ro);

        // And Restore rendering to default
        rc->SetRenderTarget(NULL);
    }

    if (cam)
        rc->AttachViewpointToCamera(oldcam);

    // restoring background attributes
    mat->SetTexture0(oldtex);
    mat->SetDiffuse(OldCol);

    return CKBR_OK;
}

CKERROR
TextureRenderCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->GetVersion() >= 0x00020000)
        return CKBR_OK;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        beh->SetFunction(TextureRenderV1);
    }
    case CKM_BEHAVIORCREATE:
    {
        // we erase the texture parameter
        beh->SetOutputParameterObject(0, NULL);
    }
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // we get the settings
        int width = 64;
        beh->GetLocalParameterValue(1, &width);
        int height = 64;
        beh->GetLocalParameterValue(2, &height);

        int i, b;
        int p2 = 0;
        for (i = 0, b = 1; i < 32; ++i, (b <<= 1))
        {
            if (width == b)
                p2++;
        }
        for (i = 0, b = 1; i < 32; ++i, (b <<= 1))
        {
            if (height == b)
                p2++;
        }
        // the texture size are not power of 2
        if (p2 != 2)
            return CKBR_OK;

        // we get the old texture
        CKTexture *tex = (CKTexture *)beh->GetOutputParameterObject(0);
        CKDestroyObject(tex);

        // creation of a new CkTexture
        tex = (CKTexture *)behcontext.Context->CreateObject(CKCID_TEXTURE, "RenderTexture", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        tex->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        // TODO : get the current BPP

        // we write the texture
        beh->SetOutputParameterObject(0, tex);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        // we restore the old texture
        CKTexture *tex = (CKTexture *)beh->GetOutputParameterObject(0);
        CKDestroyObject(tex);
    }
    break;
    }

    return CKBR_OK;
}
