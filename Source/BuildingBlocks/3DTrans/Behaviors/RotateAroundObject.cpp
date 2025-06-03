/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RotateAround
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRotateAroundDecl();
CKERROR CreateRotateAroundProto(CKBehaviorPrototype **pproto);
int RotateAround(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRotateAroundDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Rotate Around");
    od->SetDescription("Rotates a 3D Entity around another object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Axis Of Rotation: </SPAN>vector expressed in a given referential.<BR>
    <SPAN CLASS=pin>Angle Of Rotation: </SPAN>angle in degrees and number of turns.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>object used as point of reference for the rotation.<BR>
    <SPAN CLASS=pin>Keep Orientation: </SPAN>if TRUE, the 3D entity will keep its initial orientation during rotation.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    This building block makes a 3D Entity rotate around another 3D Entity (named as the referential for the axis).<BR>
    <BR>
    See also: Rotate.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfdfd999e, 0xdefeded8));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRotateAroundProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateRotateAroundProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Rotate Around");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Axis Of Rotation", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Angle Of Rotation", CKPGUID_ANGLE, "0:2");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Keep Orientation", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RotateAround);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RotateAround(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get axis of rotation
    VxVector axis(0, 1, 0);
    beh->GetInputParameterValue(0, &axis);

    // Get angle of rotation
    float angle = 0.0f;
    beh->GetInputParameterValue(1, &angle);

    // Get referential of the axis
    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(2);

    // Keep orientation ?
    CKBOOL ori = TRUE;
    beh->GetInputParameterValue(3, &ori);

    // Stick children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(4, &k);
    k = !k;

    if (!ori)
        ent->Rotate(&axis, angle, ref, k);

    VxVector pos_mov, new_pos_mov;
    ent->GetPosition(&pos_mov, ref);

    VxMatrix mat_rot;
    Vx3DMatrixFromRotation(mat_rot, axis, angle);

    Vx3DRotateVector(&new_pos_mov, mat_rot, &pos_mov);
    ent->SetPosition(&new_pos_mov, ref, k);

    return CKBR_OK;
}
