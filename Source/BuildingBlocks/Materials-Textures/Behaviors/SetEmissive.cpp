/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Emissive
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetEmissiveDecl();
CKERROR CreateSetEmissiveProto(CKBehaviorPrototype **);
int SetEmissive(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetEmissiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Emissive");
    od->SetDescription("Sets the Emissive Color of a Material");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Emissive Color: </SPAN>the material's self-illumination color.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd1d1d1d1, 0x30303030));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetEmissiveProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetEmissiveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Emissive");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Emissive Color", CKPGUID_COLOR, "0,0,0,255");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetEmissive);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetEmissive(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    VxColor amb(0, 0, 0);
    beh->GetInputParameterValue(0, &amb);

    mat->SetEmissive(amb);

    return CKBR_OK;
}
