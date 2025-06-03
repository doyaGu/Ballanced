/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Position
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetPositionDecl();
CKERROR CreateSetPositionProto(CKBehaviorPrototype **pproto);
int SetPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Position");
    od->SetDescription("Sets the Position of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>position vector expressed in a given referential.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>coordinate system in which the position is expressed.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe456e78a, 0x456789aa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPositionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetPositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Position");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPosition);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get position
    VxVector vector(0, 0, 0);
    beh->GetInputParameterValue(0, &vector);

    // Get referential
    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Stick children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(2, &k);
    k = !k;

    ent->SetPosition(&vector, ref, k);

    return CKBR_OK;
}
