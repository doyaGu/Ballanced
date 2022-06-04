/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Set Dynamic Position
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl();
CKERROR CreateSetDynamicPositionProto(CKBehaviorPrototype **);
int SetDynamicPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetDynamicPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Dynamic Position");
    od->SetDescription("Follows an 3D-Object smoothly.");
    od->SetCategory("TT Toolbox/3D Transformations");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xFD4755F, 0x7DE22DC8));
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
    if (!proto)
        return CKERR_OUTOFMEMORY;

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
    proto->DeclareInParameter("Coordinate-System", CKPGUID_3DENTITY);
    proto->DeclareInParameter("MaxDistance2Target", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("OldPosition", CKPGUID_VECTOR);
    proto->DeclareInParameter("DeltaTarget", CKPGUID_VECTOR);
    proto->DeclareInParameter("NewPosition", CKPGUID_VECTOR);
    proto->DeclareInParameter("fDist", CKPGUID_FLOAT);

    proto->DeclareLocalParameter("Status", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("Old Position", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetDynamicPosition);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT 0
#define FORCE_X 1
#define FORCE_Y 2
#define FORCE_Z 3
#define DAMPING_X 4
#define DAMPING_Y 5
#define DAMPING_Z 6
#define OFFSET_X 7
#define OFFSET_Y 8
#define OFFSET_Z 9
#define COORDINATE_SYSTEM 10
#define MAX_DISTANCE 11
#define OLD_POSITION 12
#define DELTA_TARGET 13
#define NEW_POSITION 14
#define FDIST 15

int SetDynamicPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *coordinateSystem = (CK3dEntity *)beh->GetInputParameterObject(COORDINATE_SYSTEM);

    CK3dEntity *target;
    VxVector targetPos;

    BOOL status = FALSE;
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
            beh->ActivateOutput(0);

            status = TRUE;
            beh->SetLocalParameterValue(0, &status);

            target = (CK3dEntity *)beh->GetTarget();
            if (!target)
            {
                return CKBR_OWNERERROR;
            }

            if (coordinateSystem)
            {
                target->GetPosition(&targetPos, coordinateSystem);
            }
            else
            {
                target->GetPosition(&targetPos);
            }

            return CKBR_ACTIVATENEXTFRAME;
        }
        beh->GetLocalParameterValue(0, &status);
    }

    target = (CK3dEntity *)beh->GetTarget();
    if (!target)
    {
        return CKBR_OWNERERROR;
    }

    CK3dEntity *object = (CK3dEntity *)beh->GetInputParameterObject(OBJECT);
    if (!object)
    {
        return CKBR_OWNERERROR;
    }

    VxVector objectPos;
    if (coordinateSystem)
    {
        object->GetPosition(&objectPos, coordinateSystem);
    }
    else
    {
        object->GetPosition(&objectPos);
    }

    VxVector oldPos;
    beh->GetLocalParameterValue(1, &oldPos);

    if (coordinateSystem)
    {
        target->GetPosition(&targetPos, coordinateSystem);
    }
    else
    {
        target->GetPosition(&targetPos);
    }

    beh->SetLocalParameterValue(1, &targetPos);

    VxVector v41 = targetPos - oldPos;
    objectPos -= targetPos;

    VxVector force;
    beh->GetInputParameterValue(FORCE_X, &force.x);
    beh->GetInputParameterValue(FORCE_Y, &force.y);
    beh->GetInputParameterValue(FORCE_Z, &force.z);

    VxVector damping;
    beh->GetInputParameterValue(DAMPING_X, &damping.x);
    beh->GetInputParameterValue(DAMPING_Y, &damping.y);
    beh->GetInputParameterValue(DAMPING_Z, &damping.z);

    VxVector offset;
    beh->GetInputParameterValue(OFFSET_X, &offset.x);
    beh->GetInputParameterValue(OFFSET_Y, &offset.y);
    beh->GetInputParameterValue(OFFSET_Z, &offset.z);

    float maxDistance;
    beh->GetInputParameterValue(MAX_DISTANCE, &maxDistance);

    // TODO

    return CKBR_OK;
}