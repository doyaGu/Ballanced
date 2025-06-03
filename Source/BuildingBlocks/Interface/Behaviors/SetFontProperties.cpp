/////////////////////////////////////////////////////
//		            Set Font Properties
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorSetFontPropertiesDecl();
CKERROR CreateSetFontPropertiesProto(CKBehaviorPrototype **);
int SetFontProperties(const CKBehaviorContext &behcontext);
CKERROR SetFontPropertiesCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFontPropertiesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Font Properties");
    od->SetDescription("Sets the visual appearance of a font.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Font: </SPAN>font created with the building blocks "Create Font" or "Create System Font".<BR>
    <SPAN CLASS=pin>Space: </SPAN>space between characters (can be negative).<BR>
    <SPAN CLASS=pin>Scale: </SPAN>multiplicative scale of the characters.<BR>
    <SPAN CLASS=pin>Italic Offset: </SPAN>italic offset (positive is for right italic, negative is for left).<BR>
    <SPAN CLASS=pin>Color: </SPAN>color of the font (top color of the font if gradient is enabled).<BR>
    <SPAN CLASS=pin>End Color: </SPAN>bottom color of the font if gradient is enabled.<BR>
    <SPAN CLASS=pin>Shadow Color: </SPAN>color of the shadow, if the shadow is enabled.<BR>
    <SPAN CLASS=pin>Shadow Angle: </SPAN>angle of the projection of the shadow.<BR>
    <SPAN CLASS=pin>Shadow Distance: </SPAN>distance of the shadow from the main font.<BR>
    <SPAN CLASS=pin>Shadow Size: </SPAN>scale of the shadow.<BR>
    <SPAN CLASS=pin>Lit Material: </SPAN>material to use for the font if the lighting is enabled for the font.<BR>
    <BR>
    <SPAN CLASS=setting>Font Properties: </SPAN>properties of the font:<BR>
    <UL>
    <LI>Gradient: Enables a gradient rendering for the font.</LI>
    <LI>Shadow: Enables the shadowing of the font.</LI>
    <LI>Lighting: Enables the lighting of the font.</LI>
    <LI>Disable Filter: Enables the filtering of the texture.</LI>
    </UL>
    <BR>
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFontPropertiesProto);
    od->SetCategory("Interface/Fonts");
    od->SetGuid(CKGUID(0xDACFBD61, 0x7A6E65E7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_BEOBJECT);

    return od;
}

CKERROR CreateSetFontPropertiesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Font Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    //---	Inputs declaration
    proto->DeclareInput("In");

    //---	Outputs declaration
    proto->DeclareOutput("Out");

    //----- Input Parameters Declaration
    proto->DeclareInParameter("Font", CKPGUID_FONT);
    proto->DeclareInParameter("Space", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Scale", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Italic Offset", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("End Color", CKPGUID_COLOR, "0,0,0,255");
    proto->DeclareInParameter("Shadow Color", CKPGUID_COLOR, "0,0,0,128");
    proto->DeclareInParameter("Shadow Angle", CKPGUID_ANGLE, "0:120");
    proto->DeclareInParameter("Shadow Distance", CKPGUID_FLOAT, "4");
    proto->DeclareInParameter("Shadow Size", CKPGUID_2DVECTOR, "1,1");
    proto->DeclareInParameter("Lit Material", CKPGUID_MATERIAL);

    proto->DeclareSetting("Font Properties", CKPGUID_FONTPROPERTIES);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFontProperties);
    proto->SetBehaviorCallbackFct(SetFontPropertiesCallback);

    *pproto = proto;
    return CK_OK;
}

int SetFontProperties(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKFontManager *fm = (CKFontManager *)ctx->GetManagerByGuid(FONT_MANAGER_GUID);

    int fontIndex = 0;
    beh->GetInputParameterValue(0, &fontIndex);

    CKTextureFont *font = fm->GetFont(fontIndex);
    if (!font)
        return CKBR_OK;

    // We get the leading
    beh->GetInputParameterValue(1, &font->m_Leading);

    // We get the scale
    beh->GetInputParameterValue(2, &font->m_Scale);

    // We get the italic offset
    beh->GetInputParameterValue(3, &font->m_ItalicOffset);

    ///
    // colors
    VxColor col;

    // We get the start color
    if (!beh->GetInputParameterValue(4, &col))
    {
        font->m_StartColor = RGBAFTOCOLOR(&col);
    }

    // We get the end color
    if (!beh->GetInputParameterValue(5, &col))
    {
        font->m_EndColor = RGBAFTOCOLOR(&col);
    }

    // We get the shadow color
    if (!beh->GetInputParameterValue(6, &col))
    {
        font->m_ShadowColor = RGBAFTOCOLOR(&col);
    }

    float shadowAngle = 1.0f;
    if (!beh->GetInputParameterValue(7, &shadowAngle))
    {
        float shadowDistance = 4.0f;
        // beh->GetInputParameterValue(8, &shadowDistance);

        // Due to unknown reason, this parameter may be "Shadow Size",
        // The following workaround intends to prohibit illegal value overwrite (Write Vx2DVector value to float variable).
        CKParameterIn *pIn8 = beh->GetInputParameter(8);
        if (pIn8)
        {
            CKParameter *param8 = pIn8->GetRealSource();
            if (param8 && strncmp(param8->GetName(), "Shadow Distance", sizeof("Shadow Distance")) == 0 && param8->GetDataSize() == sizeof(float))
                param8->GetValue(&shadowDistance);
        }

        font->m_ShadowOffset.x = -shadowDistance * cosf(shadowAngle);
        font->m_ShadowOffset.y = shadowDistance * sinf(shadowAngle);
    }

    // We get the shadow scale
    beh->GetInputParameterValue(9, &font->m_ShadowScale);

    // We get the lighted material
    beh->GetInputParameterValue(10, &font->m_Material);

    // Text Properties
    beh->GetLocalParameterValue(0, &font->m_Properties);

    return CKBR_OK;
}

CKERROR SetFontPropertiesCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKDWORD flags = 0;
        beh->GetLocalParameterValue(0, &flags);

        // Gradient
        beh->EnableInputParameter(5, flags & FONT_GRADIENT);

        // Shadow
        beh->EnableInputParameter(6, flags & FONT_SHADOW);
        beh->EnableInputParameter(7, flags & FONT_SHADOW);
        beh->EnableInputParameter(8, flags & FONT_SHADOW);
        beh->EnableInputParameter(9, flags & FONT_SHADOW);

        // Lighted Material
        beh->EnableInputParameter(10, flags & FONT_LIGHTING);
        if (flags & FONT_LIGHTING)
        {
            beh->EnableInputParameter(4, !(flags & FONT_LIGHTING));
            beh->EnableInputParameter(5, !(flags & FONT_LIGHTING));
        }
        else
        {
            beh->EnableInputParameter(4, TRUE);
        }
    }
    break;
    }
    return CKBR_OK;
}
