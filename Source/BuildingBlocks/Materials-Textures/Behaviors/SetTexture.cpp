/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Texture
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetTextureDecl();
CKERROR CreateSetTextureProto(CKBehaviorPrototype **);
int SetTexture(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTextureDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Texture");
    od->SetDescription("Sets the Texture of a Material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Texture: </SPAN>the texture to be set.<BR>
    <SPAN CLASS=pin>Perspective Correct: </SPAN>if TRUE, the mapping will be draw with a perspective correction. If FALSE, the mapping will be applied linearly without perspective correction.<BR>
    <SPAN CLASS=pin>Address Mode: </SPAN><BR>
    WRAP = Default mesh wrap mode is used (see VXTEXTURE_WRAPMODE).<BR>
    MIRROR = Texture coordinates outside the range [0..1] are flipped evenly.<BR>
    CLAMP = Texture coordinates greater than 1.0 are set to 1.0, and values less than 0.0 are set to 0.0.<BR>
    BORDER = When texture coordinates are greater than 1.0 or less than 0.0 , texture is set to a color defined in CKMaterial::SetTextureBorderColor.<BR>
    */
    /* warning:
    - If the object wasn't textured before, it is possible that no UV coords. were applied to its vertices. Therefore, you won't be able to see the texture properly.
    To do so, you have to assign some mapping method to the object. Whether with Virtools (for the moment, the only way to do something like that is throw the EnvMapping behavior), or by applying some texture mapping with a 3d graphic software specialized in it.<BR>
    */
    od->SetCategory("Materials-Textures/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xeb123eb5, 0x5be321be));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetTextureProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    return od;
}

CKERROR CreateSetTextureProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Texture");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Texture", CKPGUID_TEXTURE);
    proto->DeclareInParameter("Perspective Correct", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Address Mode", CKPGUID_ADDRESSMODE, "Wrap");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetTexture);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetTexture(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    // Get texture
    if (beh->GetInputParameter(0)->GetRealSource())
    {
        CKTexture *tex = (CKTexture *)beh->GetInputParameterObject(0);
        mat->SetTexture0(tex);
    }

    CKBOOL perscorrect = TRUE;
    if (!beh->GetInputParameterValue(1, &perscorrect))
    {
        mat->EnablePerspectiveCorrection(perscorrect);
    }

    VXTEXTURE_ADDRESSMODE addmode = VXTEXTURE_ADDRESSWRAP;
    if (!beh->GetInputParameterValue(2, &addmode))
    {
        mat->SetTextureAddressMode(addmode);
    }

    return CKBR_OK;
}
