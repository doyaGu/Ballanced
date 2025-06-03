/////////////////////////////////////////////////////
//		            ChangeVideoFormat
/////////////////////////////////////////////////////
#include "CKAll.h"

#define CKPGUID_TEXTUREVIDEOFORMAT CKGUID(0x2c1e5d83, 0x37b9284b)

VX_PIXELFORMAT CHTVFAllPixelFormats[14] = {_32_ARGB8888, _32_RGB888, _24_RGB888, _16_RGB565, _16_RGB555, _16_ARGB1555,
                                           _16_ARGB4444, _8_RGB332, _8_ARGB2222, _DXT1, _DXT3, _DXT5, _16_V8U8, _16_L6V5U5};

CKObjectDeclaration *FillBehaviorChangeVideoFormatDecl();
CKERROR CreateChangeVideoFormatProto(CKBehaviorPrototype **);
int ChangeVideoFormat(const CKBehaviorContext &BehContext);

CKObjectDeclaration *FillBehaviorChangeVideoFormatDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Change Texture Video Format");

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChangeVideoFormatProto);
    od->SetDescription("Changes the desired video memory format of the texture applied to");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Succes: </SPAN>is activated when the video memory format has been successfully changed (the current video card driver supports this format).<BR>
    <SPAN CLASS=out>Failed: </SPAN>is activated when the requested video memory format has not been successfully set. In this case the closest video memory format is used by the texture.<BR>
    <BR>
    <SPAN CLASS=pin>Texture Video Format: </SPAN>desired texture video memory format
    */
    od->SetCategory("Materials-Textures/Texture");
    od->SetGuid(CKGUID(0xF56D92EF, 0x41EB20C1));
    od->SetAuthorGuid(CKGUID(0x2893440a, 0x6c1f0ed7));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_TEXTURE);

    return od;
}

CKERROR CreateChangeVideoFormatProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Change Texture Video Format");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    //---	Inputs declaration
    proto->DeclareInput("In");

    //---	Outputs declaration
    proto->DeclareOutput("Success");
    proto->DeclareOutput("Failed");

    //----- Input Parameters Declaration
    proto->DeclareInParameter("Desired Video Format", CKPGUID_TEXTUREVIDEOFORMAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(ChangeVideoFormat);

    *pproto = proto;
    return CK_OK;
}

int ChangeVideoFormat(const CKBehaviorContext &BehContext)
{
    CKBehavior *beh = BehContext.Behavior;
    CKContext *ctx = BehContext.Context;
    CKRenderContext *rc = BehContext.CurrentRenderContext;
    CKRenderManager *rm = ctx->GetRenderManager();

    CKBOOL formatfound = FALSE;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        int desiredformat = 0;
        beh->GetInputParameterValue(0, &desiredformat);
        CKTexture *tex = (CKTexture *)beh->GetTarget();
        if (!tex)
            return CKBR_BEHAVIORERROR;
        // Checking it is not already set
        VX_PIXELFORMAT pixformat = tex->GetVideoPixelFormat();
        if (pixformat == CHTVFAllPixelFormats[desiredformat])
        {
            beh->ActivateOutput(0);
            return CKBR_OK;
        }

        VxDriverDesc *desc;
        if (rm && rc)
        {
//            tex->SetDesiredVideoFormat(CHTVFAllPixelFormats[desiredformat]);
            tex->SystemToVideoMemory(rc);
            desc = rm->GetRenderDriverDescription(rc->GetDriverIndex());
            for (int i = 0; i < desc->TextureFormats.Size(); i++)
            {
                if (VxImageDesc2PixelFormat(desc->TextureFormats[i]) == CHTVFAllPixelFormats[desiredformat])
                {
                    formatfound = TRUE;
                    break;
                }
            }
        }
    }

    if (formatfound)
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);
    return CKBR_OK;
}