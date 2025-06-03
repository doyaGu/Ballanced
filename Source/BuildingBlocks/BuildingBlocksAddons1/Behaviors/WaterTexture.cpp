/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              WaterTexture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "TextureProcessing.h"

CKObjectDeclaration *FillBehaviorWaterTextureDecl();
CKERROR CreateWaterTextureProto(CKBehaviorPrototype **);
int WaterTexture(const CKBehaviorContext &behcontext);
CKERROR WaterTextureCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWaterTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Water Texture");
    od->SetDescription("Apply a water matrix to the texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Damping: </SPAN>damping factor for attenuation (4 stands for 1/16,5 for 1/32 etc... ) Valid values range from 0 to 8.<BR>
    <SPAN CLASS=pin>Border Color: </SPAN>color used for the borders of the water. In bref, if your border color is [255,255,255] the wave will bounce on the texture bounds, and if you border color is [0,0,0] the waves will simply be absorbed by the texture bounds.
    Every intermediary value will be an interpolation of those two reactions.<BR>
    */
    /* warning:
    - A dynamic water texture can be obtain using behavior "Write In Texture" to set spots in the texture.<BR>
    */
    od->SetCategory("Materials-Textures/Procedural Textures");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3df93ddc, 0x9c89c141));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWaterTextureProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateWaterTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Water Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Damping", CKPGUID_INT, "8");
    proto->DeclareInParameter("Border Color", CKPGUID_COLOR, "0,0,0,0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WaterTexture);
    proto->SetBehaviorCallbackFct(WaterTextureCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR WaterTextureCallBackObject(const CKBehaviorContext &behcontext)
{
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORPRESAVE:
    {
        // Set the texture to 0 when saving so compression reduce size of file
        CKBehavior *beh = behcontext.Behavior;
        CKTexture *tex = (CKTexture *)beh->GetTarget();
        if (tex)
        {
            VxImageDescEx TextureDesc;
            tex->GetSystemTextureDesc(TextureDesc);
            CKDWORD *Image = (CKDWORD *)tex->LockSurfacePtr(0);
            if (Image)
                memset(Image, 0, TextureDesc.Height * TextureDesc.BytesPerLine);
            CKDWORD *Image2 = (CKDWORD *)tex->LockSurfacePtr(1);
            if (Image2)
                memset(Image2, 0, TextureDesc.Height * TextureDesc.BytesPerLine);
        }
    }
    break;

    case CKM_BEHAVIORATTACH:
    {
        CKBehavior *beh = behcontext.Behavior;
        CKTexture *tex = (CKTexture *)beh->GetTarget();
        if (tex)
        {
            if (tex->GetSlotCount() < 2)
            {
                tex->SetSlotCount(2);
                tex->Create(tex->GetWidth(), tex->GetHeight(), 32, 1);
            }
        }
    }
    break;
    }

    return CKBR_OK;
}

int WaterTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OK; // We must have a valid owner
    if (!(GetProcessorFeatures() & PROC_MMX))
        return CKBR_OK; // Need at least MMX instructions

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    int Damping = 8; // 1/16
    beh->GetInputParameterValue(0, &Damping);

    CKDWORD BorderColor = 0; // 1/16
    VxColor PColor;
    beh->GetInputParameterValue(1, &PColor);
    BorderColor = PColor.GetRGBA();

    VxImageDescEx TextureDesc;
    tex->GetSystemTextureDesc(TextureDesc);

    if (TextureDesc.BitsPerPixel != 32)
        return CKBR_OK;

    int CurrentSlot = tex->GetCurrentSlot();
    int PreviousSlot = (CurrentSlot + 1) & 1;

    CKDWORD *CurrImage = (CKDWORD *)tex->LockSurfacePtr(CurrentSlot);
    CKDWORD *PrevImage = (CKDWORD *)tex->LockSurfacePtr(PreviousSlot);

//------------- New pixel = 2*current pixel - neighborhood (old)
#ifdef WIN32
    // MMX asm code
    if (GetProcessorFeatures() & PROC_WNI)
    {
        WaterEffectWillamette(CurrImage, PrevImage, TextureDesc.Width, TextureDesc.Height, BorderColor, Damping);
    }
    else
    {
        WaterEffectMMX(CurrImage, PrevImage, TextureDesc.Width, TextureDesc.Height, BorderColor, Damping);
    }
#endif
#ifdef macintosh
#pragma message("Should implement Mac version of WaterEffect")
#endif

    tex->ReleaseSurfacePtr(PreviousSlot);
    tex->ReleaseSurfacePtr(CurrentSlot);

    tex->SetCurrentSlot(PreviousSlot);

    return CKBR_OK;
}
