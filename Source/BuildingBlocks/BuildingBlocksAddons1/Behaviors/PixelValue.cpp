/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              PixelValue
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorPixelValueDecl();
CKERROR CreatePixelValueProto(CKBehaviorPrototype **);
int PixelValue(const CKBehaviorContext &behcontext);
CKERROR PixelValueCallback(const CKBehaviorContext &context);

/*****************************************/
/*             Declaration
/*****************************************/
CKObjectDeclaration *FillBehaviorPixelValueDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Pixel Value");
    od->SetDescription("Return the color of a given texture's pixel.");
    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x473167b6, 0x48f75a1d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePixelValueProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

/*****************************************/
/*             Prototype
/*****************************************/
CKERROR CreatePixelValueProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Pixel Value");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("In Range");
    proto->DeclareOutput("Out Range");

    proto->DeclareInParameter("Slot", CKPGUID_INT, "0");
    proto->DeclareInParameter("(U,V)", CKPGUID_2DVECTOR, "0,0");
    proto->DeclareInParameter("Address Mode", CKPGUID_ADDRESSMODE, "Wrap");

    proto->DeclareOutParameter("Pixel", CKPGUID_COLOR);

    proto->DeclareSetting("Use 2D Vector", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Homogeneous", CKPGUID_BOOL, "TRUE");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PixelValue);
    proto->SetBehaviorCallbackFct(PixelValueCallback, CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORLOAD | CKCB_BEHAVIORCREATE);

    *pproto = proto;
    return CK_OK;
}

/*****************************************/
/*             Function
/*****************************************/
int PixelValue(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    // Gets the texture
    CKTexture *texture = (CKTexture *)beh->GetTarget();
    if (!texture)
        return CKERR_OUTOFMEMORY;

    // Use 2d vector ?
    CKBOOL use2dvector = TRUE;
    beh->GetLocalParameterValue(0, &use2dvector);

    // Homogenous ?
    CKBOOL homogeneous = FALSE;
    beh->GetLocalParameterValue(1, &homogeneous);

    // Address Mode
    int addressmode = VXTEXTURE_ADDRESSWRAP;
    beh->GetInputParameterValue(2, &addressmode);

    // Slot
    int s = 0;
    beh->GetInputParameterValue(0, &s);

    Vx2DVector uv(0, 0);
    if (use2dvector)
    {
        beh->GetInputParameterValue(1, &uv);
    }
    else
    {
        beh->GetInputParameterValue(1, &uv.x);
        beh->GetInputParameterValue(2, &uv.y);
    }

    // put uv in homogeneous coords.
    if (!homogeneous)
    {
        uv.x /= texture->GetWidth();
        uv.y /= texture->GetHeight();
    }

    float &u = uv.x;
    float &v = uv.y;

    // outside ?
    if ((u < 0) || (v < 0) || (u >= 1.0f) || (v >= 1.0f))
    {
        beh->ActivateOutput(1);
    }

    switch (addressmode)
    {
    case VXTEXTURE_ADDRESSWRAP:
        u -= (int)u;
        if (u < 0.0f)
            u += 1.0f;
        v -= (int)v;
        if (v < 0.0f)
            v += 1.0f;
        break;
    case VXTEXTURE_ADDRESSCLAMP:
    case VXTEXTURE_ADDRESSBORDER:
        if (u < 0.0f)
            u = 0.0f;
        else if (u > 1.0f)
            u = 1.0f;
        if (v < 0.0f)
            v = 0.0f;
        else if (v > 1.0f)
            v = 1.0f;
        break;
    case VXTEXTURE_ADDRESSMIRROR:
        int tu = (int)u;
        u -= tu;
        if (u < 0.0f)
            u += 1.0f;
        if (tu & 1) // odd => invert
            u = 1.0f - u;

        int tv = (int)v;
        v -= tv;
        if (v < 0.0f)
            v += 1.0f;
        if (tv & 1) // odd => invert
            v = 1.0f - v;
        break;
    }

    int X = (int)(u * (float)(texture->GetWidth() - 1));
    int Y = (int)(v * (float)(texture->GetHeight() - 1));

    CKDWORD col = texture->GetPixel(X, Y, s);
    VxColor color(col);

    beh->SetOutputParameterValue(0, &color);

    beh->ActivateOutput(0);

    return CKBR_OK;
}

/*****************************************/
/*             Callback
/*****************************************/
CKERROR PixelValueCallback(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    CKBOOL use2dvector = TRUE;
    beh->GetLocalParameterValue(0, &use2dvector);

    CKParameterIn *pin = beh->GetInputParameter(1);

    if (use2dvector)
    {
        if (pin->GetGUID() == CKPGUID_FLOAT)
        {
            // Update 2nd input
            pin->SetGUID(CKPGUID_2DVECTOR, FALSE, "(U,V)");

            // Update 3th input
            pin = beh->GetInputParameter(2);
            pin->SetGUID(CKPGUID_ADDRESSMODE, FALSE, "Address Mode");

            CKDestroyObject(beh->RemoveInputParameter(3));
        }
    }
    else
    {
        if (pin->GetGUID() == CKPGUID_2DVECTOR)
        {
            // Update the second input
            pin->SetGUID(CKPGUID_FLOAT, FALSE, "U");

            CKDestroyObject(beh->RemoveInputParameter(2));

            // Add Inputs
            beh->CreateInputParameter("V", CKPGUID_FLOAT);
            beh->CreateInputParameter("Adress Mode", CKPGUID_ADDRESSMODE);
        }
    }

    return CKBR_OK;
}