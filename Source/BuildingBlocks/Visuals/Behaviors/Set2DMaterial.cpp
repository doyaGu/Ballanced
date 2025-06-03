/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set2DMaterial
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSet2DMaterialDecl();
CKERROR CreateSet2DMaterialProto(CKBehaviorPrototype **pproto);
int Set2DMaterial(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet2DMaterialDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set 2D Material");
    od->SetDescription("Sets the material of a 2D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>Material to be set on the 2DEntity.<BR>
    <BR>
    */
    od->SetCategory("Visuals/2D");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3f7e5bff, 0x2326a71));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSet2DMaterialProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    return od;
}

CKERROR CreateSet2DMaterialProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set 2D Material");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Set2DMaterial);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Set2DMaterial(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get parent
    CKMaterial *e = (CKMaterial *)beh->GetInputParameterObject(0);

    ent->SetMaterial(e);

    return CKBR_OK;
}
