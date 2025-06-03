/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetScrBlend
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetScrBlendDecl();
CKERROR CreateSetScrBlendProto(CKBehaviorPrototype **);
int SetScrBlend(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetScrBlendDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Src Blend");
    od->SetDescription("Sets the Source mode when Blending the Material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Blending Mode:</SPAN>
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
    od->SetGuid(CKGUID(0x40234ddc, 0x012da700));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetScrBlendProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetScrBlendProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Src Blend");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Blending Mode", CKPGUID_BLENDFACTOR, "One");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetScrBlend);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetScrBlend(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    int mode = VXBLEND_ONE;
    beh->GetInputParameterValue(0, &mode);

    mat->SetSourceBlend((VXBLEND_MODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
