/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Curve Step
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetCurveStepDecl();
CKERROR CreateSetCurveStepProto(CKBehaviorPrototype **pproto);
int SetCurveStep(const CKBehaviorContext &behcontext);
CKERROR SetCurveStepCallBack(const CKBehaviorContext &behcontext);

VxVector BB_Orientate(CK3dEntity *ent, VxVector dir, int dirMode = 5, float roll = 0.0f, VxVector up = VxVector::axisY(), float speedValue = 1.0f, CKBOOL k = FALSE, CKBOOL unitaryScale = FALSE);

CKObjectDeclaration *FillBehaviorSetCurveStepDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Position On Curve");
    od->SetDescription("Places a 3D Entity at a specific position on a 3D Curve.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Curve: </SPAN>3D curve on which the 3D Entity will be positioned.<BR>
    <SPAN CLASS=pin>Progression: </SPAN>progression in percent on the curve (0% = start / 100% = end).<BR>
    <SPAN CLASS=pin>Follow: </SPAN>if TRUE, the 3D Entity's orientation will point in the direction of the curve's tangent.<BR>
    <SPAN CLASS=pin>Bank: </SPAN>if TRUE, the 3D Entity' will bank along the curve.<BR>
    <SPAN CLASS=pin>Bank Amount: </SPAN>level of banking effect (1 is a standard banking, 2 and higher values increase the effect).<BR>
    <SPAN CLASS=pin>Direction: </SPAN>specifies which base vector is to be the following direction (if the 'Follow' parameter is set to TRUE).<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    <SPAN CLASS=setting>Roll: </SPAN>specifies number of turns and angle for 3D Entity when performing Look At behavior.<BR>
    <BR>
    See Also: 'Curve Follow'.<BR>
    */
    /* warning:
    - the <SPAN CLASS=pin>axis</SPAN> must be a multiple of one of the 3 main axes: for example (0,0,1) or (0,0,-1) or (0,1,0) or (-8,0,0) but not (1,1,0).
    - also Remember: PERCENTAGE values are compatible with FLOAT values, and ANGLE values.<BR>
    (for the 'Progression' input parameter)<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x676776d0, 0x20d457bd));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetCurveStepProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetCurveStepProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Position On Curve");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Curve", CKPGUID_CURVE);
    proto->DeclareInParameter("Progression", CKPGUID_PERCENTAGE, "50");
    proto->DeclareInParameter("Follow", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Bank", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Bank Amount", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Direction", CKPGUID_DIRECTION, "Z");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->DeclareLocalParameter("", CKPGUID_VECTOR, "0,1,0"); // up vector
    proto->DeclareSetting("Roll", CKPGUID_ANGLE, "0:0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCurveStep);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SetCurveStepCallBack);

    *pproto = proto;
    return CK_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR SetCurveStepCallBack(const CKBehaviorContext &behcontext)
{

    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    {
        VxVector up(99, 0, 0);
        beh->SetLocalParameterValue(0, &up);
    }
    }

    return CKBR_OK;
}

/*******************************************************/
/*               Main Function                         */
/*******************************************************/
int SetCurveStep(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // get the 3d curve
    CKCurve *path = (CKCurve *)beh->GetInputParameterObject(0);
    if (!path)
        return CKBR_PARAMETERERROR;

    // get the Progression
    float x = 0.5;
    beh->GetInputParameterValue(1, &x);

    // Follow ?
    CKBOOL follow = FALSE;
    beh->GetInputParameterValue(2, &follow);

    // Bank ?
    CKBOOL bank = FALSE;
    beh->GetInputParameterValue(3, &bank);

    // Hierarchy ?
    CKBOOL hiera = TRUE;
    beh->GetInputParameterValue(6, &hiera);
    hiera = !hiera;

    // Roll
    float roll = 0;
    beh->GetLocalParameterValue(1, &roll);

    // Place on curve
    VxVector pos;
    path->GetPos(x, &pos);

    ent->SetPosition(&pos, NULL, hiera);

    //_________________________ Follow-Bank Part
    if (follow || bank)
    {

        // get the Direction axis
        int naxis = 0;

        CKParameterIn *pin = beh->GetInputParameter(5);
        if (pin->GetGUID() == CKPGUID_VECTOR)
        { // if old version (vector)
            VxVector axis;
            beh->GetInputParameterValue(5, &axis);
            while (axis[naxis] == 0.0f)
                naxis++;
            naxis = naxis * 2 + 1;
        }
        else
        { // if new version (CKPGUID_DIRECTION)

            naxis = 5; // Z by default
            beh->GetInputParameterValue(5, &naxis);
        }

        // next/previous
        VxVector nextpos, prevpos;
        path->GetPos(x + 0.01f, &nextpos);
        path->GetPos(x - 0.01f, &prevpos);

        // up
        VxVector up = VxVector::axisY();
        beh->GetLocalParameterValue(0, &up);

        // follow
        if (follow)
        {
            VxVector dir = nextpos - prevpos;
            dir.Normalize();

            up = BB_Orientate(ent, dir, naxis, roll, up, 1.0f, hiera);

            beh->SetLocalParameterValue(0, &up);
        }

        static VxVector rotaxis[6] = {
            // read only array
            VxVector(1, 0, 0),	//  X
            VxVector(-1, 0, 0), //  -X
            VxVector(0, 1, 0),	//  Y
            VxVector(0, -1, 0), //  -Y
            VxVector(0, 0, 1),	//  Z
            VxVector(0, 0, -1), //  -Z
        };

        // bank
        if (bank)
        {
            float ba = 1.0f;
            beh->GetInputParameterValue(4, &ba); // bank amount

            VxVector to, back;

            to = nextpos - pos;
            to.y = 0;
            back = prevpos - pos;
            back.y = 0;
            float lto = Magnitude(to);
            float lback = Magnitude(back);
            if (lto > 0 && lback > 0)
            {
                VxVector cp = CrossProduct(Normalize(to), Normalize(back));
                float angle = asinf(Magnitude(cp));
                if (DotProduct(cp, up) > 0.0f)
                {
                    if (angle)
                        ent->Rotate(&rotaxis[naxis - 1], ba * angle, ent, hiera);
                }
                else
                {
                    if (angle)
                        ent->Rotate(&rotaxis[naxis - 1], -ba * angle, ent, hiera);
                }
            }
        }
    }

    return CKBR_OK;
}
