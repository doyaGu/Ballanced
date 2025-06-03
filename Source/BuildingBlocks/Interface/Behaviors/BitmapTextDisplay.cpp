/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BitmapTextDisplay
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKBitmapFont.h"

#define LEFT 1
#define RIGHT 2
#define CENTER 3

CKObjectDeclaration *FillBehaviorBitmapTextDisplayDecl();
CKERROR CreateBitmapTextDisplayProto(CKBehaviorPrototype **pproto);
int BitmapTextDisplay(const CKBehaviorContext &behcontext);
CKERROR BitmapTextDisplayCallBackObject(const CKBehaviorContext &behcontext);
// void BitmapTextRender(CKRenderContext *rc, void *arg);
void BitmapTextRender(CKRenderContext *context, void *arg);

CKObjectDeclaration *FillBehaviorBitmapTextDisplayDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("BitmapText Display");
    od->SetDescription("Displays a Bitmap Text on top of the 3D environment");
    /* rem:
    <SPAN CLASS=in>On/Reset : </SPAN>activates the behavior, updates the displayed text and reset the inserted lines.<BR>
    <SPAN CLASS=in>Off : </SPAN>deactivates the behavior.<BR>
    <SPAN CLASS=in>Carriage Return : </SPAN>Add a new line to the text to be displayed
    <BR>
    <SPAN CLASS=out>Exit On : </SPAN>is activated when the behavior was triggered by 'On' input.<BR>
    <SPAN CLASS=out>Exit Off : </SPAN>is activated when the behavior was triggered by 'Off' input.<BR>
    <SPAN CLASS=out>Line Added : </SPAN>is activated when a new line is added.<BR>
    <BR>
    <SPAN CLASS=pin>Position : </SPAN>x,y position for the text on the screen, in pixels. It represents the top margin in Y and the left margin in X
    for the align left mode, the right margin in X for the right aligned mode and the center of the text for center align mode.<BR>
    <SPAN CLASS=pin>Character Spacing : </SPAN>Choose the number of pixel between each character. Default is 1.<BR>
    <SPAN CLASS=pin>Scale : </SPAN>Choose the horizontal and vertical scaling of the font. Warning values greater than 2 and smaller than 0.5 lead to disgracious visual effects (Moreover, values < 1 may crash some machines...).<BR>
    <SPAN CLASS=pin>Alignment : </SPAN> Choose the justification of the text : either left justified, centered on the position or right justified.<BR>
    <SPAN CLASS=pin>Vertical Spacing : </SPAN>When printing several lines with the carriage return input, you can choose here the space (in pixels) to leave between each line.<BR>
    <SPAN CLASS=pin>Text : </SPAN>Text you want to display. You can change the type of this parameter to display something else.<BR>
    <BR>
    <SPAN CLASS=setting>Font : </SPAN>The chosen font to be used for displaying the string. The fonc is a 2DSprite with
    a fixed set of characters drawn in it, each character spared by vertical lines. The bitmap must start by a vertical line of a
    color, followed by some spaces (representing the typical pixels to let for 'space' character) and another vertical
    lines + all the other characters.
    The characters used are :<BR>
    <FONT SIZE=2><pre>!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~</pre></FONT>
    Look at the 2DSprite/ArialFont.bmp in the "General_Resources" for an example of font designing....<BR>
    Enter the character "|" if you want a carriage return in your string.<BR>
    If you want to change dynamically the parameters printed in the string, you'll need to loop
    the behavior in the "On" input. <BR>
    You must provide input parameters (e.g. : the string to be displayed )<BR>
    You can add any input parameters (of any type) as you want and they'll be printed, separated by a space.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x49d487b, 0x57565345));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBitmapTextDisplayProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Interface/Text");
    return od;
}

CKERROR CreateBitmapTextDisplayProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("BitmapText Display");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On/Reset");
    proto->DeclareInput("Off");
    proto->DeclareInput("Carriage Return");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Line Added");

    proto->DeclareInParameter("Position", CKPGUID_2DVECTOR);
    proto->DeclareInParameter("Character Spacing", CKPGUID_INT, "1");
    proto->DeclareInParameter("Scale", CKPGUID_2DVECTOR, "1.0,1.0");
    proto->DeclareInParameter("Alignment", CKGUID(0x11223faf, 0x1a9315f9), "Left");
    proto->DeclareInParameter("Vertical Spacing", CKPGUID_INT, "20");
    proto->DeclareInParameter("Text", CKPGUID_STRING);

    proto->DeclareSetting("Font", CKPGUID_SPRITE);
    proto->DeclareLocalParameter(NULL, CKPGUID_STRING);  // "String"
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // "BitmapFont" (MAC ok)

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BitmapTextDisplay);
    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEPARAMETERINPUTS);

    proto->SetBehaviorCallbackFct(BitmapTextDisplayCallBackObject);

    *pproto = proto;
    return CK_OK;
}

int BitmapTextDisplay(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL update = FALSE;
    if (beh->IsInputActive(1)) //________/ OFF
    {
        behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(BitmapTextRender, beh);

        beh->ActivateInput(1, FALSE);
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0)) //________/ ON
    {
        update = TRUE;
        behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(BitmapTextRender, beh);

        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        char buffer = (char)0;
        beh->SetLocalParameterValue(1, &buffer, sizeof(buffer));
    }

    XString buffer;

    CKParameterIn *pin;
    CKParameter *pout;
    CKGUID guid;
    if (!beh->GetInput(2)) // old version
    {
        // we construct the string
        int pinc = beh->GetInputParameterCount();
        int i;
        for (i = 4; i < pinc; i++)
        {
            pin = beh->GetInputParameter(i);
            pout = pin->GetRealSource();
            if (!pout)
                continue;

            int paramSize = pout->GetStringValue(NULL);
            if (paramSize)
            {
                XAP<char> paramString(new char[paramSize]);
                pout->GetStringValue(paramString, FALSE);

                if (i != 4)
                    buffer << " ";
                buffer << (char *)paramString;
            }
        }
    }
    else
    {
        if (beh->IsInputActive(2) || update) // we need to read the input
        {
            // we construct the string
            int pinc = beh->GetInputParameterCount();
            int i;
            for (i = 5; i < pinc; i++)
            {
                pin = beh->GetInputParameter(i);
                pout = pin->GetRealSource();
                if (!pout)
                    continue;

                int paramSize = pout->GetStringValue(NULL);
                if (paramSize)
                {
                    XAP<char> paramString(new char[paramSize]);
                    pout->GetStringValue(paramString, FALSE);

                    char *foundcr = paramString;
                    foundcr = strchr(foundcr, '|');
                    while (foundcr)
                    {
                        *foundcr = '\n';
                        foundcr = strchr(foundcr, '|');
                    }

                    if (i != 5)
                        buffer << " ";
                    buffer << (char *)paramString;
                }
            }
        }
    }

    char *finalString = buffer.Str();
    if (beh->IsInputActive(2)) // we enter by concatenate
    {
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(2);

        char *oldString = (char *)beh->GetLocalParameterReadDataPtr(1);
        if (oldString)
        {
            finalString = new char[buffer.Length() + strlen(oldString) + 2];
            sprintf(finalString, "%s\n%s", oldString, buffer.Str());
        }
        update = TRUE;
    }

    if (update)
        beh->SetLocalParameterValue(1, finalString, strlen(finalString) + 1);
    if (buffer != finalString)
        delete[] finalString;

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR BitmapTextDisplayCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    {
        CKBitmapFont *bf = new CKBitmapFont;
        beh->SetLocalParameterValue(2, bf, sizeof(CKBitmapFont));
        delete bf;
    }
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
        CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(0);
        if (sprite)
        {
            sprite->Show(CKHIDE);
            if (bf->GetFont(behcontext.Context) != sprite)
            {
                bf->CreateFont(CKOBJID(sprite), ctx);
            }
        }
    }
    break;
    case CKM_BEHAVIORLOAD:
    {
        CKSprite *sprite = (CKSprite *)beh->GetLocalParameterObject(0);
        CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
        CKParameterLocal *p = beh->GetLocalParameter(2);
        if (p->GetDataSize() != sizeof(CKBitmapFont))
        {
            CKBitmapFont temp = *bf;
            temp.m_SeparationColor = 0xFFFF00FF;
            temp.m_FillColor = 0xFF000000;
            p->SetValue(&temp, sizeof(CKBitmapFont));
            bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
        }
        if (!bf)
        {
            CKBitmapFont temp;
            beh->SetLocalParameterValue(2, &temp, sizeof(CKBitmapFont));
            CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
            if (sprite)
                bf->CreateFont(CKOBJID(sprite), ctx);
        }

        if (sprite)
        {
            sprite->Show(CKHIDE);
            bf->SetFont(CKOBJID(sprite));
        }
        else
            bf->m_FontBitmap = 0;
    }
    break;
    case CKM_BEHAVIORDETACH:
    {
        if (behcontext.CurrentRenderContext)
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(BitmapTextRender, beh);
    }
    break;
    case CKM_BEHAVIORDEACTIVATESCRIPT:
    case CKM_BEHAVIORPAUSE:
    case CKM_BEHAVIORRESET:
    {
        CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
        if (bf)
            bf->MaskSeparationLines(FALSE, ctx);

        if (beh->IsActive())
        {
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(BitmapTextRender, beh);
        }
    }
    break;
    case CKM_BEHAVIORACTIVATESCRIPT:
    case CKM_BEHAVIORRESUME:
    {
        CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
        if (bf)
            bf->MaskSeparationLines(TRUE, ctx);

        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            if (beh->IsActive())
            {
                behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(BitmapTextRender, beh);
            }
        }
    }
    break;
    case CKM_BEHAVIORNEWSCENE:
    {
        CKBeObject *owner = beh->GetOwner();
        CKScene *scene = behcontext.CurrentScene;
        if (beh->IsParentScriptActiveInScene(scene))
        {
            if (beh->IsInputActive(0))
            {
                behcontext.CurrentRenderContext->AddPostSpriteRenderCallBack(BitmapTextRender, beh);
            }
        }
        else
        {
            behcontext.CurrentRenderContext->RemovePostSpriteRenderCallBack(BitmapTextRender, beh);
        }
        break;
    }
    }
    return CKBR_OK;
}

/*
void BitmapTextRender(CKRenderContext *rc, void *arg)
{
    CKRenderContext *context=VxDeviceToCKRenderContext(Vxdev);

    CKBehavior* beh = (CKBehavior*)arg;
    char *string = (char*) beh->GetLocalParameterReadDataPtr(1);

    CKBitmapFont* bf = (CKBitmapFont*)beh->GetLocalParameterReadDataPtr(2);
    if (!bf) return;
    if (!bf->HasFont()) return;

    Vx2DVector off;
    beh->GetInputParameterValue(0, &off);
    int spacing;
    beh->GetInputParameterValue(1, &spacing);
    Vx2DVector scale(1.0f,1.0f);
    beh->GetInputParameterValue(2, &scale);
    int align=LEFT;
    beh->GetInputParameterValue(3, &align);
    int vspacing=20;
    if(beh->GetOutput(2)) {
        beh->GetInputParameterValue(4, &vspacing);
    }

    switch(align) {
    case LEFT:
        {
            string = strdup(string);
            char* substring = string;
            char* newline;
            while(substring) {
                newline = strchr(substring,'\n');
                if(newline) {
                    *newline = '\0';
                    newline++;
                }
                bf->DrawString(context,substring,(int)off.x,(int)off.y,spacing,scale);
                off.y += vspacing;
                if(newline) {
                    if(*newline!='\0') substring = newline;
                    else substring = NULL;
                } else substring = NULL;
            }
            free(string);
        }
        break;
    case CENTER:
        {
            string = strdup(string);
            char* substring = string;
            char* newline;
            while(substring) {
                newline = strchr(substring,'\n');
                if(newline) {
                    *newline = '\0';
                    newline++;
                }
                int sw = bf->StringWidth(substring,spacing,scale);
                bf->DrawString(context,substring,(int)off.x-(sw>>1),(int)off.y,spacing,scale);
                off.y += vspacing;
                if(newline) {
                    if(*newline!='\0') substring = newline;
                    else substring = NULL;
                } else substring = NULL;
            }
            free(string);
        }
        break;
    case RIGHT:
        {
            string = strdup(string);
            char* substring = string;
            char* newline;
            while(substring) {
                newline = strchr(substring,'\n');
                if(newline) {
                    *newline = '\0';
                    newline++;
                }
                int sw = bf->StringWidth(substring,spacing,scale);
                bf->DrawString(context,substring,(int)off.x-sw,(int)off.y,spacing,scale);
                off.y += vspacing;
                if(newline) {
                    if(*newline!='\0') substring = newline;
                    else substring = NULL;
                } else substring = NULL;
            }
            free(string);
        }
        break;
    }

}
*/

void BitmapTextRender(CKRenderContext *context, void *arg)
{
    CKBehavior *beh = (CKBehavior *)arg;
    CKContext *ctx = beh->GetCKContext();

    char *string = (char *)beh->GetLocalParameterReadDataPtr(1);

    CKBitmapFont *bf = (CKBitmapFont *)beh->GetLocalParameterReadDataPtr(2);
    if (!bf)
        return;
    if (!bf->HasFont(ctx))
        return;

    Vx2DVector off;
    beh->GetInputParameterValue(0, &off);
    int spacing;
    beh->GetInputParameterValue(1, &spacing);
    Vx2DVector scale(1.0f, 1.0f);
    beh->GetInputParameterValue(2, &scale);
    int align = LEFT;
    beh->GetInputParameterValue(3, &align);
    int vspacing = 20;
    if (beh->GetOutput(2))
    {
        beh->GetInputParameterValue(4, &vspacing);
    }

    char *substring = CKStrdup(string);
    char *newline = NULL;

    switch (align)
    {
    case LEFT:
    {
        while (substring)
        {
            newline = strchr(substring, '\n');
            if (newline)
            {
                *newline = '\0';
                newline++;
            }
            bf->DrawString(context, substring, (int)off.x, (int)off.y, spacing, scale);
            off.y += vspacing;
            if (newline)
            {
                if (*newline != '\0')
                    substring = newline;
                else
                    substring = NULL;
            }
            else
                substring = NULL;
        }
    }
    break;
    case CENTER:
    {
        while (substring)
        {
            newline = strchr(substring, '\n');
            if (newline)
            {
                *newline = '\0';
                newline++;
            }
            int sw = bf->StringWidth(substring, spacing, scale);
            bf->DrawString(context, substring, (int)off.x - (sw >> 1), (int)off.y, spacing, scale);
            off.y += vspacing;
            if (newline)
            {
                if (*newline != '\0')
                    substring = newline;
                else
                    substring = NULL;
            }
            else
                substring = NULL;
        }
    }
    break;
    case RIGHT:
    {
        while (substring)
        {
            newline = strchr(substring, '\n');
            if (newline)
            {
                *newline = '\0';
                newline++;
            }
            int sw = bf->StringWidth(substring, spacing, scale);
            bf->DrawString(context, substring, (int)off.x - sw, (int)off.y, spacing, scale);
            off.y += vspacing;
            if (newline)
            {
                if (*newline != '\0')
                    substring = newline;
                else
                    substring = NULL;
            }
            else
                substring = NULL;
        }
    }
    break;
    default:
        break;
    }

    CKDeletePointer(substring);
}
