/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CurveLayout3D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCurveLayout3DDecl();
CKERROR CreateCurveLayout3DProto(CKBehaviorPrototype **pproto);
int CurveLayout3D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorCurveLayout3DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("3D Curve Layout");
    od->SetDescription("Organize a set of 3D belong a 3D curve in space");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Curve: </SPAN>curve to position the 3D entities to organize.<BR>
    <SPAN CLASS=pin>A: </SPAN>starting position in percent on the curve to consider.<BR>
    <SPAN CLASS=pin>B: </SPAN>ending position in percent on the curve to consider.<BR>
    <SPAN CLASS=pin>Follow: </SPAN>if TRUE, the 3D Entity's orientation will point in the direction of the curve's tangent.<BR>
    <SPAN CLASS=pin>Roll: </SPAN>angle to roll around the follow axis.<BR>
    <SPAN CLASS=pin>Direction: </SPAN>specifies which base vector is to be the following direction (if the 'Follow' parameter is set to TRUE).<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    <BR>

    The entities in the group are placed along the curve.
    */

    // Category in Virtools interface
    od->SetCategory("Interface/Layouts");

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x2557342c, 0x15246746));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateCurveLayout3DProto);
    // Class ID of the objects to which the behavior can applied
    od->SetCompatibleClassId(CKCID_GROUP);
    return od;
}

//-------------------------------------------------
// CurveLayout3D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateCurveLayout3DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("3D Curve Layout");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Curve", CKPGUID_CURVE);
    proto->DeclareInParameter("A", CKPGUID_PERCENTAGE, "0");
    proto->DeclareInParameter("B", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Follow", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Roll", CKPGUID_ANGLE, "0");
    proto->DeclareInParameter("Direction", CKPGUID_DIRECTION, "Z");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    // Function that will be called upon behavior execution
    proto->SetFunction(CurveLayout3D);

    *pproto = proto;
    return CK_OK;
}

void BB_Orientate(CK3dEntity *ent, VxVector &dir, int dirMode = 5, float roll = 0.0f, const VxVector &_up = VxVector::axisY(), float speedValue = 1.0f, CKBOOL k = FALSE, CKBOOL unitaryScale = FALSE)
{
    VxVector scale;
    if (!unitaryScale)
        ent->GetScale(&scale);

    if ((dirMode & 1) == 0)
        dir = -dir;
    VxVector old_dir, right;

    //--- tend to be Y world axis
    VxVector up = _up;
    up.x *= 0.5f;
    up.z *= 0.5f;

    //--- direction mode dependant
    switch (dirMode)
    {
    case 5: //  Z
    case 6: // -Z
    {
        if (up.x > 1.5f)
            ent->GetOrientation(NULL, &up); // memorize first up vector

        ent->GetOrientation(&old_dir, NULL);

        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(up, dir);
        up = CrossProduct(dir, right);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&dir, &up, &right, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(0, 0, 1, roll, ent, k);
    }
    break;

    case 1: //  X
    case 2: // -X
    {
        if (up.x > 1.5f)
            ent->GetOrientation(NULL, &up); // memorize first up vector

        ent->GetOrientation(NULL, NULL, &old_dir);

        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(dir, up);
        up = CrossProduct(right, dir);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&right, &up, &dir, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(1, 0, 0, roll, ent, k);
    }
    break;

    case 3: //  Y
    case 4: // -Y
    {
        if (up.x > 1.5f)
            ent->GetOrientation(&up, NULL); // memorize first up vector

        ent->GetOrientation(NULL, &old_dir);
        dir = old_dir + speedValue * (dir - old_dir);
        right = CrossProduct(up, dir);
        up = CrossProduct(dir, right);

        dir.Normalize();
        right.Normalize();
        up.Normalize();

        ent->SetOrientation(&right, &dir, &up, NULL, k);
        if (roll != 0.0f)
            ent->Rotate3f(0, 1, 0, roll, ent, k);
    }
    break;
    }

    if (!unitaryScale)
        ent->SetScale(&scale);
}

//-------------------------------------------------
// CurveLayout3D behavior execution function
//
//-------------------------------------------------
int CurveLayout3D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // IO activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get the reference curve
    CKCurve *curve = (CKCurve *)beh->GetInputParameterObject(0);
    if (!curve)
        return CKBR_PARAMETERERROR;

    // Get the group in the curve layout
    CKGroup *group = (CKGroup *)beh->GetTarget();
    if (!group)
        return CKBR_OK;

    int objectCount = group->GetObjectCount();
    if (!objectCount)
        return CKBR_OK;

    // Check if the group is only formed of 3D entities
    if (!CKIsChildClassOf(group->GetCommonClassID(), CKCID_3DENTITY))
        return CKBR_OK;

    float start = 0.0f;
    beh->GetInputParameterValue(1, &start);

    float end = 1.0f;
    beh->GetInputParameterValue(2, &end);

    CKBOOL follow = TRUE;
    beh->GetInputParameterValue(3, &follow);

    float roll = 0.0f;
    beh->GetInputParameterValue(4, &roll);

    int nAxis = 5; // Z by default
    beh->GetInputParameterValue(5, &nAxis);

    CKBOOL hierarchy = TRUE;
    beh->GetInputParameterValue(6, &hierarchy);

    // Position of each entity in the group relative to the reference frame
    VxVector currentPosition, dir;

    float current = start;
    int steps = objectCount - 1;
    if (!steps)
        steps = 1;
    float step = (end - start) / steps;

    for (int i = 0; i < objectCount; i++, current += step)
    {
        CK3dEntity *obj = (CK3dEntity *)group->GetObject(i);

        curve->GetPos(current, &currentPosition, &dir);

        obj->SetPosition(&currentPosition, NULL, !hierarchy);

        if (follow)
        {
            BB_Orientate(obj, dir, nAxis, roll, VxVector::axisY(), 1.0f, !hierarchy);
        }
    }

    return CKBR_OK;
}
