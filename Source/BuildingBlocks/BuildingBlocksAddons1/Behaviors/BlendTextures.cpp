/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              BlendTextures
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "TextureProcessing.h"

CKObjectDeclaration *FillBehaviorBlendTexturesDecl();
CKERROR CreateBlendTexturesProto(CKBehaviorPrototype **);
int BlendTextures(const CKBehaviorContext &behcontext);
CKERROR BlendTexturesCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBlendTexturesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Blend Textures");
    od->SetDescription("Blends 2 textures.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    */
    od->SetCategory("Materials-Textures/Procedural Textures");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x53ee4292, 0x7d932bb1));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBlendTexturesProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateBlendTexturesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Blend Textures");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Texture 1", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Texture 2", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Blend Factor", CKPGUID_PERCENTAGE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(BlendTextures);
    proto->SetBehaviorCallbackFct(BlendTexturesCallBackObject);

    *pproto = proto;
    return CK_OK;
}

/************************************************/
/*                  CallBack                    */
/************************************************/
CKERROR BlendTexturesCallBackObject(const CKBehaviorContext &behcontext)
{
    /*
      switch(behcontext.CallbackMessage){

          case CKM_BEHAVIORPRESAVE:
              {
                // Set the texture to 0 when saving so compression reduce size of file
                CKBehavior* beh = behcontext.Behavior;
                CKTexture* tex=(CKTexture*)beh->GetTarget();
                if (tex) {
                    VxImageDescEx TextureDesc;
                    tex->GetSystemTextureDesc(TextureDesc);
                    DWORD* Image	= (DWORD*)tex->LockSurfacePtr(0);
                    if (Image) memset(Image,0,TextureDesc.Height*TextureDesc.BytesPerLine);

                }
              }
              break;

      }
    */
    return CKBR_OK;
}

int BlendTextures(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OK; // We must have a valid owner
    if (!(GetProcessorFeatures() & PROC_MMX))
        return CKBR_OK; // Need a MMX Cpu

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKTexture *tex1 = (CKTexture *)beh->GetInputParameterObject(0);
    CKTexture *tex2 = (CKTexture *)beh->GetInputParameterObject(1);

    float BlendFactor = 0.5f;
    beh->GetInputParameterValue(2, &BlendFactor);

    if (!(tex1 && tex2))
    {
        return CKBR_OK;
    }

    VxImageDescEx TexDesc1, TexDesc2, TexDesc;
    tex->GetSystemTextureDesc(TexDesc);
    tex1->GetSystemTextureDesc(TexDesc1);
    tex2->GetSystemTextureDesc(TexDesc2);

    // Only work in 32 bits per pixel
    if (TexDesc.BitsPerPixel != 32)
        return CKBR_OK;

    int Size = TexDesc.Width * TexDesc.Height;
    int Size1 = TexDesc1.Width * TexDesc1.Height;
    int Size2 = TexDesc2.Width * TexDesc2.Height;
    if (Size != Size1 || Size != Size2)
        return CKBR_OK;

    CKDWORD *TexData = (CKDWORD *)tex->LockSurfacePtr();
    CKDWORD *TexData1 = (CKDWORD *)tex1->LockSurfacePtr();
    CKDWORD *TexData2 = (CKDWORD *)tex2->LockSurfacePtr();

#if defined(WIN32) || (defined(macintosh) && defined(__i386__))
    // MMX asm code
    BlendDataMMX(TexData, TexData1, TexData2, Size, BlendFactor);
#elif (defined(macintosh) && defined(__ppc__))
#if G4 // sould be nerver defined because G3 processor don't have altivec instructions
    // BlendDataAltivec(TexData,TexData1,TexData2,Size,BlendFactor);
    BlendDataAltivec(TexData, TexData2, TexData1, Size, BlendFactor);
#else
    BlendDataC(TexData, TexData1, TexData2, Size, BlendFactor);
#endif
#endif

    // Force texture reload
    tex->ReleaseSurfacePtr();

    return CKBR_OK;
}
