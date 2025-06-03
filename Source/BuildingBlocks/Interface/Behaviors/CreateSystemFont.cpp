/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CreateSystemFont
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorCreateSystemFontDecl();
CKERROR CreateCreateSystemFontProto(CKBehaviorPrototype **pproto);
int CreateSystemFont(const CKBehaviorContext &behcontext);
CKERROR CreateSystemFontCallback(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Create a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorCreateSystemFontDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create System Font");
    od->SetDescription("Create a font from a system font name");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Success: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Error: </SPAN>is activated when the font can't be created.<BR>
    <BR>
    <SPAN CLASS=pin>Font Name: </SPAN>name used to identify the font.<BR>
    <SPAN CLASS=pin>System Font Name: </SPAN>select the system font to use.<BR>
    <SPAN CLASS=pin>Font Weight: </SPAN>select the weight of the font to use.<BR>
    <SPAN CLASS=pin>Italic: </SPAN>check this to use the italic version of the selected font.<BR>
    <SPAN CLASS=pin>Underline: </SPAN>check this to use the underlined version of the selected font.<BR>
    <BR>
    <SPAN CLASS=pout>Font Created: </SPAN>a font identifier, give this to the related building blocks.<BR>
    <BR>
    <SPAN CLASS=setting>Texture Resolution: </SPAN>select the desired size of the generated texture, higher resolution give nicer font but use more video memory.<BR>
    <SPAN CLASS=setting>Full Charset: </SPAN>if checked, draw characters with ASCII code > 128, else we use a half sized texture<BR>
    <SPAN CLASS=setting>Render Control Characters: </SPAN>if checked we draw the control characters of ASCII table.<BR>
    <SPAN CLASS=setting>Dynamic Texture: </SPAN>if checked the texture created is dynamic.<BR>

    The font created with this building block can be used with the "2D Text" and the "3D Text" building blocks.

    <BR>
    See Also: 'Create Font', 'Delete Font', '2D Text', '3D Text' (in the Interface/Text category).<BR>
    */

    // Category in Virtools interface
    od->SetCategory("Interface/Fonts");

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x936334fc, 0xf243684f));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateCreateSystemFontProto);
    // Class ID of the objects to which the behavior can apply
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(FONT_MANAGER_GUID);

    return od;
}

//-------------------------------------------------
// CreateSystemFont behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateCreateSystemFontProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Create System Font");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");
    proto->DeclareOutput("Success");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Font Name", CKPGUID_STRING, "My Font");
    proto->DeclareInParameter("System Font Name", CKPGUID_FONTNAME, "Arial");
    proto->DeclareInParameter("Font Weight", CKPGUID_FONTWEIGHT, "NORMAL");
    proto->DeclareInParameter("Italic", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Underline", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("Font Created", CKPGUID_FONT);
    proto->DeclareOutParameter("Texture Created", CKPGUID_TEXTURE);

    proto->DeclareSetting("Texture Resolution", CKPGUID_FONTRESOLUTION, "512x512");
    proto->DeclareSetting("Full Charset", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Render Control Characters", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Dynamic Texture", CKPGUID_BOOL, "TRUE");

    proto->DeclareLocalParameter("Current Font", CKPGUID_STRING);
    proto->DeclareSetting("Font Size", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_NONE);

    // Function that will be called upon behavior execution
    proto->SetFunction(CreateSystemFont);
    proto->SetBehaviorCallbackFct(CreateSystemFontCallback);

    *pproto = proto;
    return CK_OK;
}

//-------------------------------------------------
// CreateSystemFont behavior execution function
//
//-------------------------------------------------
int CreateSystemFont(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    // Get the behavior parameters
    CKFontManager *font = (CKFontManager *)behcontext.Context->GetManagerByGuid(FONT_MANAGER_GUID);

    CKSTRING fontName = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!fontName)
        return CKBR_PARAMETERERROR;
    int systemFontIndex = -1;
    beh->GetInputParameterValue(1, &systemFontIndex);
    if (systemFontIndex == -1)
        return CKBR_PARAMETERERROR;

    int weight = 400;
    beh->GetInputParameterValue(2, &weight);
    CKBOOL italic = FALSE, underline = FALSE;
    beh->GetInputParameterValue(3, &italic);
    beh->GetInputParameterValue(4, &underline);

    int resolution = 4;
    beh->GetLocalParameterValue(0, &resolution);
    CKBOOL fullCharSet = TRUE, renderControls = FALSE, dynamic = TRUE;
    beh->GetLocalParameterValue(1, &fullCharSet);
    beh->GetLocalParameterValue(2, &renderControls);
    beh->GetLocalParameterValue(3, &dynamic);

    int fontSize = 0;
    beh->GetLocalParameterValue(5, &fontSize);

    CKTexture *texture = font->CreateTextureFromFont(systemFontIndex, resolution, fullCharSet, weight, italic, underline, renderControls, dynamic, fontSize);
    if (!texture)
        return CKBR_PARAMETERERROR;

    Vx2DVector charNumber;
    charNumber.Set(16.0f, (fullCharSet) ? (16.0f) : (8.0f));

    VxRect tZone;
    tZone.SetSize(Vx2DVector((float)texture->GetWidth(), (float)texture->GetHeight()));

    // we create the texture for the defined logical font
    int fontCreated = font->CreateTextureFont(fontName, texture, tZone, charNumber, FALSE, 0);
    beh->SetOutputParameterValue(0, &fontCreated);

    beh->SetOutputParameterObject(1, texture);

    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR CreateSystemFontCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    {
        CKParameterManager *pm = behcontext.Context->GetParameterManager();
        if (!pm)
            return CKBR_OK;
        CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));

        int fontIndex = -1;
        beh->GetInputParameterValue(1, &fontIndex);
        if ((fontIndex < 0) || (fontIndex > data->NbData))
            return CKBR_OK;
        beh->SetLocalParameterValue(4, data->Desc[fontIndex]);
    }
    break;
    case CKM_BEHAVIOREDITED:
    {
        CKParameterManager *pm = behcontext.Context->GetParameterManager();
        if (!pm)
            return CKBR_OK;
        CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));

        int fontIndex = -1;
        beh->GetInputParameterValue(1, &fontIndex);
        if ((fontIndex < 0) || (fontIndex > data->NbData))
            return CKBR_OK;
        beh->SetLocalParameterValue(4, data->Desc[fontIndex]);
    }
    break;
    case CKM_BEHAVIORLOAD:
    {
        CKParameterIn *pin = beh->GetInputParameter(1);
        CKParameter *pout = pin->GetRealSource();

        CKParameterManager *pm = behcontext.Context->GetParameterManager();
        if (!pm)
            return CKBR_OK;

        int i = 0;
        if (pin->GetGUID() == CKPGUID_STRING)
        {
            //------------ Old version => The Font Name was a string
            CKSTRING FontName = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
            pin->SetGUID(CKPGUID_FONTNAME, TRUE);

            CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));
            if (pout && FontName && strlen(FontName))
            {
                for (i = 0; i < data->NbData; i++)
                {
                    if (FontName)
                        if (!strcmp(FontName, data->Desc[i]))
                        {
                            pout->SetValue(&i);
                            beh->SetLocalParameterValue(4, data->Desc[i], strlen(data->Desc[i]) + 1);
                            return CKBR_OK;
                        }
                }
            }

            // No font was found
            behcontext.Context->OutputToConsoleEx("Warning : '%s' font not found on this system !", FontName);

            // we try to use Arial instead
            for (i = 0; i < data->NbData; i++)
            {
                if (!strcmp("Arial", data->Desc[i]))
                {
                    break;
                }
            }

            // if we have not found arial either, we use the first one !
            if (i == data->NbData)
                i = 0;

            pout->SetValue(&i);
        }
        else
        {
            //------------ New version => It's an enum
            CKSTRING currentFont = (CKSTRING)beh->GetLocalParameterReadDataPtr(4);

            CKEnumStruct *data = pm->GetEnumDescByType(pm->ParameterGuidToType(CKPGUID_FONTNAME));
            if (!currentFont || (strlen(currentFont) == 0))
            {
                // we try to use Arial instead
                for (i = 0; i < data->NbData; i++)
                {
                    if (!strcmp("Arial", data->Desc[i]))
                    {
                        break;
                    }
                }

                // if we have not found arial either, we use the first one !
                if (i == data->NbData)
                    i = 0;
                if (pout)
                    pout->SetValue(&i);
                return CKBR_OK;
            }

            for (i = 0; i < data->NbData; i++)
            {
                CKSTRING desc = data->Desc[i];
                if (desc && !strcmp(currentFont, desc))
                {
                    if (pout)
                        pout->SetValue(&i);
                    return CKBR_OK;
                }
            }

            // No font was found
            behcontext.Context->OutputToConsoleEx("Warning : '%s' font not found on this system !", currentFont);

            // we try to use Arial instead
            for (i = 0; i < data->NbData; i++)
            {
                if (!strcmp("Arial", data->Desc[i]))
                {
                    break;
                }
            }

            // if we have not found arial either, we use the first one !
            if (i == data->NbData)
                i = 0;

            pout->SetValue(&i);
        }
    }
    break;
    }
    return CKBR_OK;
}
