/////////////////////////////////////////////////////
//		            Combine Texture
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCombineTextureDecl();
CKERROR CreateCombineTextureProto(CKBehaviorPrototype **);
int CombineTexture(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCombineTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Combine Texture");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Color Texture: </SPAN>The texture to copy on the target texture.<BR>
    <SPAN CLASS=pin>Color Filter: </SPAN>The percentage of each colors to take in the Color Texture.<BR>
    <SPAN CLASS=pin>Alpha Texture: </SPAN>The texture to use as the alpha channel of the target texture.<BR>
    */
    /* warning:
    - The Color Texture and the Alpha Texture must be the same size as the target texture.<BR>
    - Use Full white for color filter to obtain pure copy (and more speed efficiency.).<BR>
    - Only the red channel of the alpha texture is used if this texture is specified.<BR>
    - This operation is destructive, you will lost your original texture unless you have setted
    initial conditions on it.<BR>
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCombineTextureProto);
    od->SetDescription("Combine two Textures into one");
    od->SetCategory("Materials-Textures/Texture");
    od->SetGuid(CKGUID(0x84BE2329, 0xAED66CE1));
    od->SetAuthorGuid(CKGUID(0x56495254, 0x4f4f4c53));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_TEXTURE);

    return od;
}

CKERROR CreateCombineTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Combine Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    //---	Inputs declaration
    proto->DeclareInput("In");

    //---	Outputs declaration
    proto->DeclareOutput("Out");

    //----- Input Parameters Declaration
    proto->DeclareInParameter("Color Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Color Filter", CKPGUID_COLOR, "255,255,255,255");
    proto->DeclareInParameter("Alpha Texture", CKPGUID_TEXTURE);

    //---	Output Parameters Declaration

    //----	Local Parameters Declaration

    //----	Settings Declaration

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(CombineTexture);

    *pproto = proto;
    return CK_OK;
}

int CombineTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!CKIsChildClassOf(tex, CKCID_TEXTURE))
        return CKBR_OWNERERROR;

    // WARNING only work for 32 bits textures

    int width = tex->GetWidth();
    int height = tex->GetHeight();

    // lock
    CKBYTE *data = tex->LockSurfacePtr();

    ///
    // Input Parameters
    CKTexture *coltex = (CKTexture *)beh->GetInputParameterObject(0);
    VxColor colf(1.0f, 1.0f, 1.0f, 1.0f);
    beh->GetInputParameterValue(1, &colf);
    VxColor invcolf(1.0f - colf.r, 1.0f - colf.g, 1.0f - colf.b, 1.0f - colf.a);

    // We copy the color texture
    if (coltex)
    {
        CKBYTE *coldata = coltex->LockSurfacePtr(coltex->GetCurrentSlot());

        if (coldata)
        {
            if ((coltex->GetWidth() == width) && (coltex->GetHeight() == height))
            {
                CKBYTE *tempdata = data;
                if ((colf.r == 1.0f) && (colf.g == 1.0f) && (colf.b == 1.0f) && (colf.a == 1.0f))
                {
                    memcpy(tempdata, coldata, width * height * 4);
                }
                else
                {
                    for (int i = 0; i < width * height; ++i)
                    {
                        *tempdata = (unsigned char)((*tempdata) * invcolf.b + (*coldata) * colf.b);
                        tempdata++;
                        coldata++;
                        *tempdata = (unsigned char)((*tempdata) * invcolf.g + (*coldata) * colf.g);
                        tempdata++;
                        coldata++;
                        *tempdata = (unsigned char)((*tempdata) * invcolf.r + (*coldata) * colf.r);
                        tempdata++;
                        coldata++;
                        *tempdata = (unsigned char)((*tempdata) * invcolf.a + (*coldata) * colf.a);
                        tempdata++;
                        coldata++;
                    }
                }
            }
        }
    }

    // Alpha component
    CKTexture *alphatex = (CKTexture *)beh->GetInputParameterObject(2);

    if (alphatex)
    {
        if ((alphatex->GetWidth() == width) && (alphatex->GetHeight() == height))
        {
            CKBYTE *coldata = alphatex->LockSurfacePtr();

            if (coldata)
            {
                if ((alphatex->GetWidth() == width) && (alphatex->GetHeight() == height))
                {
                    CKBYTE *tempdata = data;
                    for (int i = 0; i < width * height; ++i)
                    {
#if (defined(macintosh) && defined(__ppc__))
                        coldata += 3;
                        *tempdata = (*coldata);
                        tempdata += 4;
                        coldata++;
#else
                        tempdata += 3;
                        *tempdata = (*coldata);
                        tempdata++;
                        coldata += 4;
#endif
                    }
                }
            }
            alphatex->ReleaseSurfacePtr();
        }
    }

    // unlock
    tex->ReleaseSurfacePtr();

    return CKBR_OK;
}
