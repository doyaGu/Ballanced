/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Texture Min
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetTextureMinDecl();
CKERROR CreateSetTextureMinProto(CKBehaviorPrototype **);
int SetTextureMin(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTextureMinDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Texture Min");
    od->SetDescription("Sets the Material Filtering Mode when it is minimized.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Mode: </SPAN><BR>
    NEAREST: No Filter<BR>
    LINEAR: Bilinear Interpolation<BR>
    MIPNEAREST: Mip mapping<BR>
    MIPLINEAR: Mip Mapping with Bilinear interpolation<BR>
    LINEARMIPNEAREST: Trilinear Filtering<BR>
    LINEARMIPLINEAR: Anisotropic Filtering<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12589321, 0x32587123));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetTextureMinProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetTextureMinProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Texture Min");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Mode", CKPGUID_FILTERMODE, "Linear");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetTextureMin);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetTextureMin(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    int mode = VXTEXTUREFILTER_LINEAR;
    beh->GetInputParameterValue(0, &mode);

    mat->SetTextureMinMode((VXTEXTURE_FILTERMODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
