/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetRenderOptions
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetRenderOptionsDecl();
CKERROR CreateSetRenderOptionsProto(CKBehaviorPrototype **pproto);
int SetRenderOptions(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRenderOptionsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Render Options");
    od->SetDescription("Set the flags of the render context.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Render Options: </SPAN>render flags...<BR>
    <U>Background Sprites:</U> background 2D Sprites are to be rendered.<BR>
    <U>Foreground Sprites:</U> foreground 2D Sprites are to be rendered.<BR>
    <U>Use Camera Ratio:</U> render context size is automatically set using current camera aspect ratio and Window Rectangle (Horizontal Size has priority).<BR>
    <U>Clear ZBuffer:</U> clear ZBuffer.<BR>
    <U>Clear Back Buffer:</U> clear Back Buffer.<BR>
    <U>Clear Stencil Buffer:</U> clear Stencil Buffer (stencil buffer has not been yet implemented).<BR>
    <U>Back To Front:</U> cnable Buffers Swapping.<BR>
    <U>Clear Only Viewport:</U> do not clear entire device, only viewport (device=window, viewport=subpart in window where 3d objects are rendered [for instance think about 16/9 rendering]).<BR>
    <U>Wait VBL:</U> wait for Vertical Blank before blitting.<BR>
    <U>Disable 3D:</U> do not draw 3D scene (only 2d sprites).<BR>
    <BR>
    <SPAN CLASS=pin>Global Shade Mode: </SPAN> the shade mode, that applies to every 3d objects drawn in the surrent render context.<BR>
    <U>Flat:</U> 3d objects are drawn using flat shading.<BR>
    <U>Gouraud:</U> 3d objects are drawn using gouraud shading.<BR>
    <U>Phong:</U> 3d objects are drawn using phong shading (not implemented).<BR>
    <U>Materials Default:</U> 3d objects are drawn using their own materials shading mode.<BR>
    <BR>
    <SPAN CLASS=pin>Use Textures: </SPAN> if TRUE, textures are rendered, otherwise, no texture rendering is perform.<BR>
    <SPAN CLASS=pin>Add Wireframe Layer: </SPAN> if TRUE, a wireframe layer will be added to all objects rendering.<BR>
    <BR>
    This building block sets the flags of the current render context.<BR>
    You can use it to save a time while rendering your scene by setting the Clear Back Buffer=FALSE, if you're sure your 3d environments covers all your rendering view
    (it clear the back buffer's viewport if the "Clear Viewport" is set to TRUE, and the all device if not).<BR>
      */
    od->SetCategory("World Environments/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4d586c55, 0x5250236a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00000005);
    od->SetCreationFunction(CreateSetRenderOptionsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetRenderOptionsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Render Options");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Render Options", CKPGUID_RENDEROPTIONS, "Background Sprites,Foreground Sprites,Use Camera Ratio,Clear ZBuffer,Clear Back Buffer,Clear Stencil Buffer,Buffer Swapping,Clear Only Viewport");
    proto->DeclareInParameter("Global Shade Mode", CKPGUID_GLOBALEXMODE, "Material Default");
    proto->DeclareInParameter("Use Textures", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Add Wireframe Layer", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetRenderOptions);

    *pproto = proto;
    return CK_OK;
}

int SetRenderOptions(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Render Options
    CKDWORD inputFlags = CK_RENDER_DEFAULTSETTINGS;
    beh->GetInputParameterValue(0, &inputFlags);

    // Global Shade Mode
    VxShadeType shading = MaterialDefault;
    beh->GetInputParameterValue(1, &shading);

    // Use Textures
    CKBOOL useTextures = TRUE;
    beh->GetInputParameterValue(2, &useTextures);

    // Add Wireframe Layer
    CKBOOL addWireframe = FALSE;
    beh->GetInputParameterValue(3, &addWireframe);

    CKRenderContext *rc = behcontext.CurrentRenderContext;

    // memorize the old flag (use camera ratio)
    CKDWORD oldRenderFlags = rc->GetCurrentRenderOptions();
    CKBOOL wasUsingCameraRatio = oldRenderFlags & CK_RENDER_USECAMERARATIO;

    rc->SetCurrentRenderOptions(inputFlags);
    rc->SetGlobalRenderMode(shading, useTextures, addWireframe);

    // resize viewport if we are no more using the camera ratio
    if (wasUsingCameraRatio && !(inputFlags & CK_RENDER_USECAMERARATIO))
    {
        VxRect rect(0, 0, (float)rc->GetWidth(), (float)rc->GetHeight());
        rc->SetViewRect(rect);

        // added by agoth
        rc->ForceCameraSettingsUpdate();
    }
    else
    { // force the camera to be updated if we now want to match its ratio

        if (!wasUsingCameraRatio && (inputFlags & CK_RENDER_USECAMERARATIO))
        {
            rc->ForceCameraSettingsUpdate();
        }
    }

    return CKBR_OK;
}
