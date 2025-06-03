/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TextDisplay
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define FONT_ITALIC    0x1
#define FONT_UNDERLINE 0x2
#define FONT_STRIKEOUT 0x4
#define FONT_BOLD      0x8

struct tdFontInfo
{
    char m_FaceName[56];
    int m_Weight;
    int m_Style;
};

CKObjectDeclaration *FillBehaviorTextDisplayDecl();
CKERROR CreateTextDisplayProto(CKBehaviorPrototype **pproto);
int TextDisplay(const CKBehaviorContext &behcontext);
CKERROR TextDisplayCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTextDisplayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Text Display");
    od->SetDescription("Displays a text on top of the 3D environment");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the behavior.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the behavior.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the behavior was triggered by 'On' input.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the behavior was triggered by 'Off' input.<BR>
    <BR>
    <SPAN CLASS=pin>Offset: </SPAN>X/Y offset for the text position on the screen (X/Y=0/0 is the top left corner of the screen).<BR>
    <SPAN CLASS=pin>Color: </SPAN>color expressed in RGBA.<BR>
    <SPAN CLASS=pin>Alignment: </SPAN>Chooses the alignment of the text inside the sprite text.<BR>
    <SPAN CLASS=pin>Size: </SPAN>vertical size of the text (expressed pixels).<BR>
    <SPAN CLASS=pin>Text: </SPAN>Text you want to display. You can change the type of this parameter to display something else.<BR>
    <BR>
    <SPAN CLASS=setting>Font: </SPAN>font to use (default = arial).<BR>
    <SPAN CLASS=setting>Sprite Size: </SPAN>size of the sprite in which you draw the text (the smaller, the faster).<BR>
    <BR>
    You must provide input parameters (e.g. : the string to be displayed )
    You can add any input parameters (of any type) as you want and they'll be printed, separated by a space.
    If you want to change the color, the size of the text, you need to reenter by the On input.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf22d010a, 0xf2cd010a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateTextDisplayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Interface/Text");
    return od;
}

CKERROR CreateTextDisplayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Text Display");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Offset", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Alignment", CKPGUID_SPRITETEXTALIGNMENT, "Left");
    proto->DeclareInParameter("Size", CKPGUID_INT, "10");
    proto->DeclareInParameter("Text", CKPGUID_STRING);

    proto->DeclareLocalParameter(NULL, CKPGUID_SPRITETEXT); // Spritetext
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF);	// Font Info (Mac OK)
    proto->DeclareLocalParameter(NULL, CKPGUID_NONE);		// Useless
    proto->DeclareLocalParameter(NULL, CKPGUID_NONE);		// Useless

    proto->DeclareSetting("Sprite Size", CKPGUID_2DVECTOR, "320,25");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TextDisplay);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_CUSTOMSETTINGSEDITDIALOG));

    proto->SetBehaviorCallbackFct(TextDisplayCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int TextDisplay(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSpriteText *tt = (CKSpriteText *)beh->GetLocalParameterObject(0);

    char *string = tt->GetText();

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        tt->Show(CKHIDE);
        return CKBR_OK;
    }
    else
    {
        if (beh->IsInputActive(0))
        {
            // Init to force redraw
            string = NULL;

            beh->ActivateInput(0, FALSE);
            beh->ActivateOutput(0);
            tt->Show(CKSHOW);
        }
    }

    CKParameterIn *pin;
    CKParameter *pout;

    XString buffer;

    // we construct the string
    int pinc = beh->GetInputParameterCount();
    for (int i = 4; i < pinc; i++)
    {
        pin = beh->GetInputParameter(i);
        pout = pin->GetRealSource();
        if (pout)
        {
            int paramsize = pout->GetStringValue(NULL);
            if (paramsize)
            {
                XAP<char> paramstring(new char[paramsize]);
                pout->GetStringValue(paramstring, FALSE);

                buffer << (char *)paramstring;
                buffer << " ";
            }
        }
    }

    Vx2DVector off;
    beh->GetInputParameterValue(0, &off);

    CKDWORD col;
    VxColor color;
    beh->GetInputParameterValue(1, &color);
    // warning inversion of the red and blue due to a BUG
    col = RGBAFTOCOLOR(color.r, color.g, color.b, color.a);

    // we get an int (input parameter)
    int align;
    beh->GetInputParameterValue(2, &align);

    // we get an int (input parameter)
    int size;
    beh->GetInputParameterValue(3, &size);

    tt->SetPosition(off);
    if (!string || strcmp(buffer.Str(), string))
    {
        tdFontInfo fi;
        beh->GetLocalParameterValue(1, &fi);

        tt->SetTextColor(col);
        tt->SetAlign((CKSPRITETEXT_ALIGNMENT)align);
        tt->SetFont(fi.m_FaceName, size, fi.m_Weight, fi.m_Style & FONT_ITALIC, fi.m_Style & FONT_UNDERLINE);
        tt->SetText(buffer.Str());
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR TextDisplayCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    {
        tdFontInfo fi;
        strcpy(fi.m_FaceName, "Arial");
        fi.m_Style = 0;
        fi.m_Weight = 400;
        beh->SetLocalParameterValue(1, &fi, sizeof(tdFontInfo));
    }
    case CKM_BEHAVIORLOAD:
    {
        // Patch for the older versions
        if (beh->GetVersion() < 0x00020000)
        {
            // we create a font info
            tdFontInfo fi;
            // font name
            CKSTRING font = (CKSTRING)beh->GetLocalParameterReadDataPtr(3);
            if (font)
                strcpy(fi.m_FaceName, font);
            else
                strcpy(fi.m_FaceName, "Arial");
            fi.m_Style = 0;
            fi.m_Weight = 400;
            CKParameterLocal *pl = beh->GetLocalParameter(1);
            if (pl)
                pl->SetGUID(CKPGUID_VOIDBUF);
            pl = beh->GetLocalParameter(2);
            if (pl)
                pl->SetGUID(CKPGUID_NONE);
            pl = beh->GetLocalParameter(3);
            if (pl)
                pl->SetGUID(CKPGUID_NONE);
            beh->SetLocalParameterValue(1, &fi, sizeof(tdFontInfo));
        }

        // creation of a CkSprite text
        CKSpriteText *tt = (CKSpriteText *)ctx->CreateObject(CKCID_SPRITETEXT, "TextDisplay Sprite", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        tt->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        tt->EnableClipToCamera(FALSE);

        // We get the size of the sprite to be created
        Vx2DVector spritesize(320.0f, 32.0f);
        beh->GetLocalParameterValue(4, &spritesize);
        tt->ReleaseAllSlots();
        tt->Create((int)spritesize.x, (int)spritesize.y, 32);

        // We add the sprite test to the level
        behcontext.CurrentLevel->AddObject(tt);

        // save the sprite
        beh->SetLocalParameterObject(0, tt);
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKSpriteText *tt = (CKSpriteText *)beh->GetLocalParameterObject(0);

        // WE get the size of the sprite to be created
        Vx2DVector spritesize(320.0f, 32.0f);
        beh->GetLocalParameterValue(4, &spritesize);
        tt->ReleaseAllSlots();
        tt->Create((int)spritesize.x, (int)spritesize.y, 32);

        // We get the font parameter
        CKInterfaceManager *UIMan = (CKInterfaceManager *)ctx->GetManagerByGuid(INTERFACE_MANAGER_GUID);
        if (UIMan)
            UIMan->CallBehaviorSettingsEditionFunction(beh, 0);
    }
    break;
    case CKM_BEHAVIORDELETE:
    {
        CKSpriteText *tex = (CKSpriteText *)beh->GetLocalParameterObject(0);
        ctx->DestroyObject(tex);
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
        CKScene *scene = ctx->GetCurrentScene();
        if (beh->IsParentScriptActiveInScene(scene))
        {
            if (beh->IsActive())
            {
                CKSpriteText *tex = (CKSpriteText *)beh->GetLocalParameterObject(0);
                tex->Show(CKSHOW);
            }
        }
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        CKSpriteText *tex = (CKSpriteText *)beh->GetLocalParameterObject(0);
        tex->Show(CKHIDE);
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        CKSpriteText *tex = (CKSpriteText *)beh->GetLocalParameterObject(0);
        CKBeObject *owner = beh->GetOwner();

        CKScene *scene = ctx->GetCurrentScene();
        if (beh->IsParentScriptActiveInScene(scene))
        {
            scene->AddObjectToScene(tex);
            if (beh->IsActive())
            {
                tex->Show(CKSHOW);
            }
        }
        else
        {
            if (owner->IsInScene(scene))
            {
                scene->AddObjectToScene(tex);
                tex->Show(CKHIDE);
            }
            else
            {
                scene->RemoveObjectFromScene(tex);
                tex->Show(CKHIDE);
            }
        }
    }
    break;
    }
    return CKBR_OK;
}
