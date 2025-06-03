/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Rotate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRotateDecl();
CKERROR CreateRotateProto(CKBehaviorPrototype **pproto);
int Rotate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRotateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Rotate");
    od->SetDescription("Rotates the 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Axis Of Rotation: </SPAN>vector expressed in the given referential to define the orientation axis.<BR>
    <SPAN CLASS=pin>Angle Of Rotation: </SPAN>angle in degrees and number of turns.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>referential used to define the direction of the rotation axis.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    This building block makes a 3D Entity rotate around its local axis. Use Rotate Around to rotate a 3D Entity around another object.<BR>
    <BR>
    See Also: Rotate Around.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xffffffee, 0xeeffffff));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRotateProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateRotateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Rotate");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Axis Of Rotation", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Angle Of Rotation", CKPGUID_ANGLE, "0:2");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Rotate);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Rotate(const CKBehaviorContext &behcontext)
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
    float angle = 0.01f;
    beh->GetInputParameterValue(1, &angle);

    // Get referential of the axis
    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(2);

    // Stick children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(3, &k);
    k = !k;

    ent->Rotate(&axis, angle, ref, k);

    return CKBR_OK;
}
