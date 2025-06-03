/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Set Quaternion Orientation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetQuaternionOrientationDecl();
CKERROR CreateSetQuaternionOrientationProto(CKBehaviorPrototype **pproto);
int SetQuaternionOrientation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetQuaternionOrientationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Quaternion Orientation");
    od->SetDescription("Sets the orientation of a 3D Entity with a quaternion.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Orientation: </SPAN>the orientation quaternion.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>referential in which the angles are expressed (-NULL- in most cases).<BR>
    <BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ae25172, 0x5c781faa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetQuaternionOrientationProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetQuaternionOrientationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Quaternion Orientation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Orientation", CKPGUID_QUATERNION, "0:0");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetQuaternionOrientation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetQuaternionOrientation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get dir
    VxQuaternion q;
    beh->GetInputParameterValue(0, &q);

    // Stick children
    CKBOOL hierarchy = TRUE;
    beh->GetInputParameterValue(1, &hierarchy);

    // Referential
    CK3dEntity *ref = NULL;
    ref = (CK3dEntity *)beh->GetInputParameterObject(2);

    ent->SetQuaternion(&q, ref, !hierarchy, TRUE);

    return CKBR_OK;
}
