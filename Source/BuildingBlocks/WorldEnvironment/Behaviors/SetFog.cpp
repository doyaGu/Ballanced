/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetFog
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetFogDecl();
CKERROR CreateSetFogProto(CKBehaviorPrototype **);
int SetFog(const CKBehaviorContext &behcontext);
int SetFogV2(const CKBehaviorContext &behcontext);
CKERROR SetFogCB(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFogDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Fog");
    od->SetDescription("Sets the Atmospheric Fog.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Fog Mode : </SPAN>the fog calculation of gradation (None, Exponential, Exponential Squared, Linear).<BR>
    <SPAN CLASS=pin>Fog Start : </SPAN>distance from the viewpoint at which the fog effect starts.<BR>
    <SPAN CLASS=pin>Fog End : </SPAN>distance from the viewpoint at which the fog effect ends.<BR>
    <SPAN CLASS=pin>Fog Density : </SPAN>density of the fog.<BR>
    <SPAN CLASS=pin>Fog Color : </SPAN>fog color in RGBA.<BR>
    <BR>
    This behavior can be applied to any object in the enviroment.<BR>
    Trick : If you put the 'Fog End' to 30.0 (for example) you'll then be able to save rendering time by reducing the Far Clipping Plane of the camera to 30.0 (because none of the object behind this limit will be visible).
    */
    od->SetCategory("World Environments/Global");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x151aaaaa, 0xffd5bdbc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateSetFogProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetFogProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Fog");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Fog Mode", CKPGUID_FOGMODE, "Linear");
    proto->DeclareInParameter("Fog Start", CKPGUID_FLOAT, "30.0");
    proto->DeclareInParameter("Fog End", CKPGUID_FLOAT, "100.0");
    proto->DeclareInParameter("Fog Density", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Fog Color", CKPGUID_COLOR, "128,128,128,255");

    // Only used to detect old fog mode
    proto->DeclareLocalParameter("NewVersion", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFog);
    proto->SetBehaviorCallbackFct(SetFogCB);

    *pproto = proto;
    return CK_OK;
}

CKERROR SetFogCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        if (beh->GetVersion() == 0x00020000)
        {
            beh->SetFunction(SetFogV2);
        }
        else
        {
            beh->SetFunction(SetFog);
        }
    }
    break;
    }

    return CKBR_OK;
}

int SetFog(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Check for an old version of fog settings ( non-compliant projection matrix was used ...)
    CKBOOL OldMode = TRUE;
    if (beh->GetLocalParameterCount() > 0)
        OldMode = FALSE;

    // Get mode
    int mode = 0;
    beh->GetInputParameterValue(0, &mode);

    // Get start
    float fog_start = 0.0f;
    beh->GetInputParameterValue(1, &fog_start);
    if (OldMode)
        fog_start *= 2.8f; // more or less equal to 1/sin(fov)

    // Get end
    float fog_end = 10.0f;
    beh->GetInputParameterValue(2, &fog_end);
    if (OldMode)
        fog_end *= 2.8f; // more or less equal to 1/sin(fov)

    // Get density
    float fog_dens = 1.0f;
    beh->GetInputParameterValue(3, &fog_dens);

    // Get color
    VxColor fog_color;
    beh->GetInputParameterValue(4, &fog_color);

    CKRenderContext *rcontext = behcontext.CurrentRenderContext;

    rcontext->SetFogMode(mode ? VXFOG_LINEAR : VXFOG_NONE);
    rcontext->SetFogStart(fog_start);
    rcontext->SetFogEnd(fog_end);
    rcontext->SetFogDensity(fog_dens);
    rcontext->SetFogColor(RGBAFTOCOLOR(fog_color.r, fog_color.g, fog_color.b, 1.0f));

    return CKBR_OK;
}

int SetFogV2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get mode
    int mode = 0;
    beh->GetInputParameterValue(0, &mode);

    // Get start
    float fog_start = 0.0f;
    beh->GetInputParameterValue(1, &fog_start);

    // Get end
    float fog_end = 10.0f;
    beh->GetInputParameterValue(2, &fog_end);

    // Get density
    float fog_dens = 1.0f;
    beh->GetInputParameterValue(3, &fog_dens);

    // Get color
    VxColor fog_color;
    beh->GetInputParameterValue(4, &fog_color);

    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    rcontext->SetFogMode((VXFOG_MODE)mode);
    rcontext->SetFogStart(fog_start);
    rcontext->SetFogEnd(fog_end);
    rcontext->SetFogDensity(fog_dens);
    rcontext->SetFogColor(RGBAFTOCOLOR(fog_color.r, fog_color.g, fog_color.b, 1.0f));

    return CKBR_OK;
}
