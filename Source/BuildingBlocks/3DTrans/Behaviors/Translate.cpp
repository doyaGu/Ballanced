/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Translate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTranslateDecl();
CKERROR CreateTranslateProto(CKBehaviorPrototype **pproto);
int Translate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTranslateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Translate");
    od->SetDescription("Translates a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Translate Vector: </SPAN>3D Vector.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>coordinate system in which the Translate Vector is expressed.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x000d000d, 0x000d000d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTranslateProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateTranslateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Translate");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Translate Vector", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Translate);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Translate(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get translation vector
    VxVector vector;
    beh->GetInputParameterValue(0, &vector);

    // Get referential
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Stick children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(2, &k);
    k = !k;

    ent->Translate(&vector, e, k);

    return CKBR_OK;
}
