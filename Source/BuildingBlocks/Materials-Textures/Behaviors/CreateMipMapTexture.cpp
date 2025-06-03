/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              CreateMipMapTexture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCreateMipMapTextureDecl();
CKERROR CreateCreateMipMapTextureProto(CKBehaviorPrototype **);
int CreateMipMapTexture(const CKBehaviorContext &behcontext);
CKERROR CreateMipMapTextureCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateMipMapTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set MipMap Texture");
    od->SetDescription("Set the 'Full Size Texture' to contain the given mipmap textures.");

    od->SetCategory("Materials-Textures/Texture");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6e407fcf, 0xeff06a5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateMipMapTextureProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCreateMipMapTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set MipMap Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Full Size Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Size/2 Texture", CKPGUID_TEXTURE);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_VARIABLEPARAMETERINPUTS));
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CreateMipMapTexture);
    proto->SetBehaviorCallbackFct(CreateMipMapTextureCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR CreateMipMapTextureCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {

    case CKM_BEHAVIOREDITED:
    {
        int c_pin = beh->GetInputParameterCount();

        char pin_str[16];

        CKParameterIn *pin;
        for (int a = 1, b = 2; a < c_pin; ++a, b *= 2)
        {
            sprintf(pin_str, "Size/%d", b);
            pin = beh->GetInputParameter(a);
            pin->SetGUID(CKPGUID_TEXTURE, TRUE, pin_str);
        }
    }
    break;
    }

    return CKBR_OK;
}

/******************************************************/
/*   Bbehavior Function
/******************************************************/
int CreateMipMapTexture(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // get full size texture
    CKTexture *finalTex = (CKTexture *)beh->GetInputParameterObject(0);
    if (!finalTex)
        return CKBR_OWNERERROR;

    // automatically generate mipmap levels (so if some texture is missing,
    // the lower levels will be correct)
    // finalTex->UseMipmap(TRUE);

    // set the texture as being a user mipmap texture
    finalTex->SetUserMipMapMode(TRUE);

    // number of generated levels
    int levelCount = finalTex->GetMipmapCount();

    VxImageDescEx mipDesc, texDesc;

    int widthFT = finalTex->GetWidth();
    int heightFT = finalTex->GetHeight();

    int w = widthFT, h = heightFT;
    int pinCount = beh->GetInputParameterCount();

    for (int a = 1; a < pinCount; ++a)
    {
        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(a);
        if (!tex)
            return CKBR_OK;

        w >>= 1;
        h >>= 1;

        if (tex->GetWidth() != w || tex->GetHeight() != h)
        {
            behcontext.Context->OutputToConsoleEx("Wrong texture size for pIn: \"%s\", must be [%d,%d]", beh->GetInputParameter(a)->GetName(), w, h);
            return CKBR_OWNERERROR;
        }

        // copy texture image into corresponding mipmap level
        CKDWORD *texPtr = (CKDWORD *)tex->LockSurfacePtr();

        finalTex->GetUserMipMapLevel(a - 1, mipDesc);
        memcpy(mipDesc.Image, texPtr, mipDesc.Width * mipDesc.Height * 4);

        tex->ReleaseSurfacePtr();
    }

    finalTex->FreeVideoMemory();

    return CKBR_OK;
}
