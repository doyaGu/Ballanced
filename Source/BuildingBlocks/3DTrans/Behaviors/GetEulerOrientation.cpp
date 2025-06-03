/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        Get Euler Orientation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorGetEulerOrientationDecl();
CKERROR CreateGetEulerOrientationProto(CKBehaviorPrototype **pproto);
int GetEulerOrientation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetEulerOrientationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Euler Orientation");
    od->SetDescription("Gets the orientation of a 3D Entity with Euler angles.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>X: </SPAN>rotation angle around X axis.<BR>
    <SPAN CLASS=pout>Y: </SPAN>rotation angle around Y axis.<BR>
    <SPAN CLASS=pout>Z: </SPAN>rotation angle around Z axis.<BR>
    <BR>
    */
    /* warning:
    - This building block is now obsolete because you can use the Parameter Operations "Get Orientation" and "Get Local Orientation".<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xc4977a8, 0x6c645d14));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetEulerOrientationProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateGetEulerOrientationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Euler Orientation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("X", CKPGUID_ANGLE, "0:0");
    proto->DeclareOutParameter("Y", CKPGUID_ANGLE, "0:0");
    proto->DeclareOutParameter("Z", CKPGUID_ANGLE, "0:0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(GetEulerOrientation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetEulerOrientation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    float f1, f2, f3;

    Vx3DMatrixToEulerAngles(ent->GetWorldMatrix(), &f1, &f2, &f3);

    // Write values
    beh->SetOutputParameterValue(0, &f1);
    beh->SetOutputParameterValue(1, &f2);
    beh->SetOutputParameterValue(2, &f3);

    return CKBR_OK;
}
