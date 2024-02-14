/////////////////////////////
/////////////////////////////
//
//        TT LookAt
//
/////////////////////////////
/////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorLookAtDecl();
CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto);
int LookAt(const CKBehaviorContext &behcontext);

VxVector BB_Orientate(CK3dEntity *ent, VxVector dir, int dirMode = 5, float roll = 0.0f, VxVector up = VxVector::axisY(), float speedValue = 1.0f, CKBOOL k = FALSE, CKBOOL unitaryScale = FALSE);

CKObjectDeclaration *FillBehaviorLookAtDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT LookAt");
    od->SetDescription("Axis constrained lock at.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d4861f8, 0x2861703d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLookAtProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT LookAt");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Following Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("look X-Axis", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("look Y-Axis", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("look Z-Axis", CKPGUID_BOOL, "false");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Direction", CKPGUID_DIRECTION, "Z");
    proto->DeclareSetting("Roll", CKPGUID_ANGLE, "0:0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LookAt);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int LookAt(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    VxVector tmp1, tmp2;
    float speedValue = 1.0f;
    CKBOOL k = TRUE;

    // Get Position
    VxVector tmp3 = VxVector::axis0();
    beh->GetInputParameterValue(0, &tmp3);

    // Referential
    CK3dEntity *target = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Following Speed.
    beh->GetInputParameterValue(2, &speedValue);

    // Stick Children ?
    beh->GetInputParameterValue(3, &k);
    k = !k;

    CKBOOL lookX = TRUE;
    CKBOOL lookY = TRUE;
    CKBOOL lookZ = TRUE;
    beh->GetInputParameterValue(4, &lookX);
    beh->GetInputParameterValue(4, &lookY);
    beh->GetInputParameterValue(4, &lookZ);

    ent->GetPosition(&tmp1);

    if (target)
        target->Transform(&tmp2, &tmp3);
    else
    {
        tmp2 = tmp3;
    }

    // Time Based ?
    CKBOOL timeBased = FALSE;
    beh->GetLocalParameterValue(0, &timeBased);

    if (timeBased)
        speedValue *= behcontext.DeltaTime * 0.07f;
    if (speedValue > 1.0f)
        speedValue = 1.0f;

    VxVector dir = tmp2 - tmp1;
    if (dir != VxVector::axis0())
    {
        if (lookX)
            dir.x = 0.0;
        if (lookY)
            dir.y = 0.0;
        if (lookZ)
            dir.z = 0.0;
        if (dir.Magnitude() != 0.0)
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

VxVector BB_Orientate(CK3dEntity *ent, VxVector dir, int dirMode, float roll, VxVector up, float speedValue, CKBOOL k, CKBOOL unitaryScale)
{

    VxVector scale;
    if (!unitaryScale)
        ent->GetScale(&scale);

    if ((dirMode & 1) == 0)
        dir = -dir;
    VxVector old_dir, right;

    //--- tend to be Y world axis
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
        right = CrossProduct(dir, up);
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

    return up;
}
