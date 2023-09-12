/////////////////////////////////////
/////////////////////////////////////
//
//        TT Homing Missile
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorHomingMissileDecl();
CKERROR CreateHomingMissileProto(CKBehaviorPrototype **pproto);
int HomingMissile(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHomingMissileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Homing Missile");
    od->SetDescription("Leads a 3D Entity towards its target.");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x44d43591, 0x28f91084));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHomingMissileProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateHomingMissileProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Homing Missile");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Target Reach");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Destination", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Missilespeed", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Adaptationspeed", CKPGUID_FLOAT, "0.5");

    proto->DeclareOutParameter("Current Velocity", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(HomingMissile);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int HomingMissile(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    CK3dEntity *destination = (CK3dEntity *)beh->GetInputParameterObject(0);

    if (!target || !destination)
    {
        return CKBR_OWNERERROR;
    }

    VxVector targetPos;
    VxVector destinationPos;
    target->GetPosition(&targetPos);
    destination->GetPosition(&destinationPos);

    VxVector dir = destinationPos - targetPos;
    VxVector targetReach, vec2;

    float missileSpeed;
    beh->GetInputParameterValue(1, &missileSpeed);
    missileSpeed *= behcontext.DeltaTime * 0.001f;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        targetReach = Normalize(dir) * missileSpeed;
    }
    else
    {
        beh->GetOutputParameterValue(0, &targetReach);
        beh->ActivateInput(1, FALSE);
    }

    float adaptationSpeed;
    beh->GetInputParameterValue(2, &adaptationSpeed);

    dir = Normalize(dir) * adaptationSpeed;

    targetReach = Normalize(targetReach) * (1.0f - adaptationSpeed) + dir;
    targetReach.Normalize();

    VxVector up(0.0f, 1.0f, 0.0f);
    targetReach *= missileSpeed;

    dir = Normalize(targetReach);
    VxVector right = CrossProduct(up, dir);
    right.Normalize();

    up.Set(right.z * dir.y - right.y * dir.z,
           right.x * dir.z - right.z * dir.x,
           right.y * dir.x - right.x * dir.y);
    up.Normalize();

    targetPos += targetReach;
    target->SetOrientation(&dir, &up, &right);
    target->SetPosition(&targetPos);

    beh->SetOutputParameterValue(0, &targetReach);

    if (Magnitude(targetPos - destinationPos) > destination->GetRadius())
    {
        beh->ActivateOutput(1);
    }
    else
    {
        beh->ActivateOutput(0);
    }

    return CKBR_OK;
}