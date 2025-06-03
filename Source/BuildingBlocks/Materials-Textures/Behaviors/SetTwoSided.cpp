/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Two Sided
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetTwoSidedDecl();
CKERROR CreateSetTwoSidedProto(CKBehaviorPrototype **);
int SetTwoSided(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTwoSidedDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Both Sided");
    od->SetDescription("Sets the Material to be rendered even if the face normals doesn't faces the camera.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Both Sided: </SPAN>if TRUE, the material will be two-sided (i.e. rendered even if the faces aren't turn toward the camera).<BR>
    <BR>
    Use this building block to render walls or doors (or everything else) that have been modelized with no thickness.<BR>
    */
    od->SetCategory("Materials-Textures/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x76319845, 0x48593399));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetTwoSidedProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    return od;
}

CKERROR CreateSetTwoSidedProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Both Sided");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Both sided", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetTwoSided);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetTwoSided(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    // Two sided ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(0, &k);

    mat->SetTwoSided(k);

    return CKBR_OK;
}
