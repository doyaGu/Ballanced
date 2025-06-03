/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetBlendModes
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetBlendModesDecl();
CKERROR CreateSetBlendModesProto(CKBehaviorPrototype **);
int SetBlendModes(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBlendModesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Blend Modes");
    od->SetDescription("Sets the blend modes of a Material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Blend: </SPAN>tells whether the blending is enable or not.<BR>
    <SPAN CLASS=pin>Source Blend: </SPAN>the blending mode of the source.<BR>
    <SPAN CLASS=pin>Destination Blend: </SPAN>the blending mode of the destination.<BR>
    <SPAN CLASS=pin>Texture Blend: </SPAN>how the texture will affect the material.<BR>
    <BR><BR>
    BLEND_ZERO<BR>
    Blend factor is (0, 0, 0, 0).<BR><BR>

    BLEND_ONE<BR>
    Blend factor is (1, 1, 1, 1).<BR><BR>

    BLEND_SRCCOLOR<BR>
    Blend factor is (Rs, Gs, Bs, As).<BR><BR>

    BLEND_INVSRCCOLOR<BR>
    Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).<BR><BR>

    BLEND_SRCALPHA<BR>
    Blend factor is (As, As, As, As).<BR><BR>

    BLEND_INVSRCALPHA<BR>
    Blend factor is (1-As, 1-As, 1-As, 1-As).<BR><BR>

    BLEND_DESTALPHA<BR>
    Blend factor is (Ad, Ad, Ad, Ad).<BR><BR>

    BLEND_INVDESTALPHA<BR>
    Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).<BR><BR>

    BLEND_DESTCOLOR<BR>
    Blend factor is (Rd, Gd, Bd, Ad).<BR><BR>

    BLEND_INVDESTCOLOR<BR>
    Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).<BR><BR>

    BLEND_SRCALPHASAT<BR>
    Blend factor is (f, f, f, 1); f = min(As, 1-Ad).<BR><BR>

    BLEND_BOTHSRCALPHA<BR>
    Source blend factor is (As, As, As, As), and destination blend factor is (1-As, 1-As, 1-As, 1-As); the destination blend selection is overridden.<BR><BR>

    BLEND_BOTHINVSRCALPHA<BR>
    Source blend factor is (1-As, 1-As, 1-As, 1-As), and destination blend factor is (As, As, As, As); the destination blend selection is overridden.<BR><BR>

    Resulting Color in the Destination Frame Buffer will be<BR>

    FrameBufferColor = DestColor*DestBlendMode + SourceColor*SrcBlenMode<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2f572a66, 0x6a9a0088));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBlendModesProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetBlendModesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Blend Modes");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Blend", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Source Blend", CKPGUID_BLENDFACTOR, "Source Alpha");
    proto->DeclareInParameter("Destination Blend", CKPGUID_BLENDFACTOR, "Inverse Source Alpha");
    proto->DeclareInParameter("Texture Blend", CKPGUID_BLENDMODE, "Modulate");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetBlendModes);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetBlendModes(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    CKBOOL enable = TRUE;
    if (beh->GetInputParameterValue(0, &enable) == CK_OK)
        mat->EnableAlphaBlend(enable);

    VXBLEND_MODE srcmode = VXBLEND_SRCALPHA;
    if (beh->GetInputParameterValue(1, &srcmode) == CK_OK)
        mat->SetSourceBlend(srcmode);

    VXBLEND_MODE destmode = VXBLEND_INVSRCALPHA;
    if (beh->GetInputParameterValue(2, &destmode) == CK_OK)
        mat->SetDestBlend(destmode);

    VXTEXTURE_BLENDMODE texmode = VXTEXTUREBLEND_MODULATE;
    if (beh->GetInputParameterValue(3, &texmode) == CK_OK)
        mat->SetTextureBlendMode(texmode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
