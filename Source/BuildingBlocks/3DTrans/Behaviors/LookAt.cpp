/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		               Look At
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLookAtDecl();
CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto);
int LookAt(const CKBehaviorContext &behcontext);

VxVector BB_Orientate(CK3dEntity *ent, VxVector dir, int dirMode = 5, float roll = 0.0f, VxVector up = VxVector::axisY(), float speedValue = 1.0f, CKBOOL k = FALSE, CKBOOL unitaryScale = FALSE);

CKObjectDeclaration *FillBehaviorLookAtDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Look At");
    od->SetDescription("Makes a 3D Entity's local Z axis point to a given position.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>position vector to look at.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>3D Entity referential from which the position is expressed.<BR>
    <SPAN CLASS=pin>Following Speed: </SPAN>how fast the 3D Entity should take to look at the given position.
    The higher the percentage, the faster the 3D Entity will turn to look.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    <SPAN CLASS=setting>Time Based: </SPAN>If checked, this building block will be Time and not Frame Rate dependant.
    Making this building block Time dependant has the advantage that compositions will run at the
    same rate on all computer configurations.<BR>
    <SPAN CLASS=setting>Direction: </SPAN>specifies which base vector is to be the looking direction.<BR>
    <SPAN CLASS=setting>Roll: </SPAN>specifies number of turns and angle for 3D Entity when performing Look At behavior.<BR>
    <BR>
    This building block should be looped if the 3D Entity is to keep a constant distance from the object all the time.<BR>
    */
    /* warning:
    - This behavior does not work properly with animated characters as their directional axis is not Z.<BR>
    When modeled, the 3D Entity's Z axis should be set as the object's looking direction, and the Y axis as the up direction.<BR>
    - As a time dependant looping process this building block has to be looped with a 1 frame link delay.
    The reason of this is because the internally laps of time used is always equal to the duration of one global process of the building blocks.<BR>
    This means, if you use 2 or 3 frame link delay, the process will become frame dependant.<BR>
    Note: if you don't use any 'Following Speed' value (ie 100), the above remark is useless (because movement is instantaneous).<BR>
    */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x26843971, 0x86249317));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLookAtProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Look At");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Following Speed", CKPGUID_PERCENTAGE, "10");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Direction", CKPGUID_DIRECTION, "Z");
    proto->DeclareSetting("Roll", CKPGUID_ANGLE, "0:0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LookAt);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// LOOK AT V2.0
int LookAt(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    VxVector tmp1, tmp2;
    float speedValue = 1.0f;
    CKBOOL k = TRUE;

    /////////////////////////////////////////////////////////////////////////////////////
    if (beh->GetInputParameterCount() == 4) // we use Version 2.0 of 'Look At' Behavior
    {
        // Get Position
        VxVector tmp3 = VxVector::axis0();
        beh->GetInputParameterValue(0, &tmp3);

        // Referential
        CK3dEntity *Target = (CK3dEntity *)beh->GetInputParameterObject(1);

        // Following Speed.
        beh->GetInputParameterValue(2, &speedValue);

        // Stick Children ?
        beh->GetInputParameterValue(3, &k);
        k = !k;

        ent->GetPosition(&tmp1);

        if (Target)
            Target->Transform(&tmp2, &tmp3);
        else
        {
            tmp2 = tmp3;
        }

        /////////////////////////////////////////////////////////////////////////////////////
    }
    else // we use Version 1.0 of 'Look At' Behavior
    {

        // Get Target
        CK3dEntity *Target = (CK3dEntity *)beh->GetInputParameterObject(0);

        // Effect Value.
        beh->GetInputParameterValue(1, &speedValue);

        // Stick Children ?
        beh->GetInputParameterValue(2, &k);
        k = !k;

        ent->GetPosition(&tmp1);

        if (Target)
            Target->GetPosition(&tmp2);
        else
            tmp2 = VxVector::axis0();
    }

    /////////////////////////////////////////////////////-- End of Version difference

    // Time Based ?
    CKBOOL time_based = FALSE;
    beh->GetLocalParameterValue(0, &time_based);

    if (time_based)
        speedValue *= behcontext.DeltaTime * 0.07f;
    if (speedValue > 1.0f)
        speedValue = 1.0f;

    VxVector dir = tmp2 - tmp1;
    if (dir != VxVector::axis0())
    {
        dir.Normalize();

        // get direction mode (enum)
        int dirMode = 5; // Z by default
        beh->GetLocalParameterValue(1, &dirMode);

        // get roll
        float roll = 0;
        beh->GetLocalParameterValue(2, &roll);

        BB_Orientate(ent, dir, dirMode, roll, VxVector::axisY(), speedValue);
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
