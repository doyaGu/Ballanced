///////////////////////////////////////////
///////////////////////////////////////////
//
//        TT Set Dynamic Position
//
///////////////////////////////////////////
///////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl();
CKERROR CreateSetDynamicPositionProto(CKBehaviorPrototype **pproto);
int SetDynamicPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Dynamic Position");
    od->SetDescription("Follows an 3D-Object smoothly.");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfd4755f, 0x7de22dc8));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetDynamicPositionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetDynamicPositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Dynamic Position");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("On");
    proto->DeclareOutput("Off");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Force X", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Force Y", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Force Z", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Damping X", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Damping Y", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Damping Z", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset X", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset Y", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Offset Z", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Coordinate-System", CKPGUID_3DENTITY, "NULL");
    proto->DeclareInParameter("MaxDistance2Target", CKPGUID_FLOAT, "0.0");

    proto->DeclareOutParameter("OldPosition", CKPGUID_VECTOR);
    proto->DeclareOutParameter("DeltaTarget", CKPGUID_VECTOR);
    proto->DeclareOutParameter("NewPosition", CKPGUID_VECTOR);
    proto->DeclareOutParameter("fDist", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("Status", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("Old Position", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetDynamicPosition);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetDynamicPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CK3dEntity *coordinateSystem = (CK3dEntity *)beh->GetInputParameterObject(10);

    CKBOOL status = FALSE;

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->SetLocalParameterValue(0, &status);
    }
    else
    {
        if (beh->IsInputActive(0))
        {
            beh->ActivateInput(0, FALSE);
            beh->ActivateOutput(0, TRUE);

            status = TRUE;
            beh->SetLocalParameterValue(0, &status);

            VxVector targetPosition;
            if (coordinateSystem)
                target->GetPosition(&targetPosition, coordinateSystem);
            else
                target->GetPosition(&targetPosition);
            beh->SetLocalParameterValue(1, &targetPosition);
            return CKBR_ACTIVATENEXTFRAME;
        }

        beh->GetLocalParameterValue(0, &status);
    }

    CK3dEntity *object = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (!object)
        return CKBR_OWNERERROR;

    VxVector objectPosition;
    if (coordinateSystem)
        object->GetPosition(&objectPosition, coordinateSystem);
    else
        object->GetPosition(&objectPosition);

    VxVector oldTargetPosition;
    beh->GetLocalParameterValue(1, &oldTargetPosition);

    VxVector targetPosition;
    if (coordinateSystem)
        target->GetPosition(&targetPosition, coordinateSystem);
    else
        target->GetPosition(&targetPosition);
    beh->SetLocalParameterValue(1, &targetPosition);

    VxVector delta = targetPosition - oldTargetPosition;
    VxVector position = objectPosition - targetPosition;

    float forceX, forceY, forceZ;
    beh->GetInputParameterValue(1, &forceX);
    beh->GetInputParameterValue(2, &forceY);
    beh->GetInputParameterValue(3, &forceZ);

    float dampingX, dampingY, dampingZ;
    beh->GetInputParameterValue(4, &dampingX);
    beh->GetInputParameterValue(5, &dampingY);
    beh->GetInputParameterValue(6, &dampingZ);

    float offsetX, offsetY, offsetZ;
    beh->GetInputParameterValue(7, &offsetX);
    beh->GetInputParameterValue(8, &offsetY);
    beh->GetInputParameterValue(9, &offsetZ);

    float maxDistance2Target;
    beh->GetInputParameterValue(11, &maxDistance2Target);

    float cof = behcontext.DeltaTime * 0.001f;
    forceX *= cof;
    forceY *= cof;
    forceZ *= cof;

    targetPosition.x += (position.x - offsetX) * forceX + delta.x * dampingX;
    targetPosition.y += (position.y - offsetY) * forceY + delta.y * dampingY;
    targetPosition.z += (position.z - offsetZ) * forceZ + delta.z * dampingZ;

    objectPosition -= targetPosition;

    float distance = (objectPosition != VxVector::axis0()) ? objectPosition.Magnitude() : 0.0f;

    VxVector vector(0.0f, 0.0f, 0.0f);
    if (distance > maxDistance2Target && maxDistance2Target > 0.0f)
    {
        vector = objectPosition;
        if (vector == VxVector::axis0())
            vector.Normalize();
        vector *= maxDistance2Target;
        objectPosition = delta - vector;
        targetPosition = objectPosition;
    }

    if (coordinateSystem)
        target->SetPosition(&targetPosition, coordinateSystem);
    else
        target->SetPosition(&targetPosition);

    beh->SetOutputParameterValue(0, &vector);

    if (status)
        return CKBR_ACTIVATENEXTFRAME;

    beh->ActivateOutput(1, TRUE);
    return CKBR_OK;
}