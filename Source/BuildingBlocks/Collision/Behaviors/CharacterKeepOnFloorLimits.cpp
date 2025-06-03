/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		    Character Keep On Floor Limits
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateCharacterKeepOnFloorLimitsBehaviorProto(CKBehaviorPrototype **);
int DoCharacterKeepOnFloorLimits(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCharacterKeepOnFloorLimitsDecl()
{

    CKObjectDeclaration *od = CreateCKObjectDeclaration("Character Keep On Floor Limits");
    od->SetDescription("Forces a character to keep to a floor's limits, simulating a collision with a wall if the character tries to venture outside.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    This building block makes a character stay on floors and simulates a collision with a wall if
    the character attempts to go outside the floor limits.<BR>
    To have a floor reference object (for example feet), the character must have an object containing
    ("FloorRef","Foot","Pas") in its name, and be a child of the root of the character in order to be recognized.<BR>
    If this object is not present the building block uses a character's bounding box to make contact.<BR>
    Objects representing floors must be declared as floors first, either by using the behavior
    "DeclareFloors" or giving them the Floor attribute. This building block is also a easy way to prevent
    characters from going outside of a floor's bounding box limits.<BR>
    See also : Enhanced Character Keep On Floor, and Character Keep On Floor.<BR>
    */
    /* warning:
    - This building block should be looped if the character is to keep to the floor limits all the time.<BR>
    The character is kept on the floor's limits by comparing its position with the limits of the floor's
    bounding box. This building block is useful as it avoids the need for processing big calculations, but it
    is very approximative and will not work well for terrrains that are not square.<BR>
    */
    od->SetCategory("Characters/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x788061fb, 0x249f29a1));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCharacterKeepOnFloorLimitsBehaviorProto);
    od->SetCompatibleClassId(CKCID_CHARACTER);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR DaCharacterKeepOnFloorLimitsCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
    {
        beh->SetPriority(-32100);
        CKCharacter *carac = (CKCharacter *)beh->GetOwner();
        if (!carac)
            return CKBR_OWNERERROR;

        VxVector Old;
        carac->GetPosition(&Old);
        beh->SetLocalParameterValue(0, &Old);

        return CKBR_OK;
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateCharacterKeepOnFloorLimitsBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Character Keep On Floor Limits");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    // Old Position
    proto->DeclareLocalParameter(NULL, CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(DoCharacterKeepOnFloorLimits);
    proto->SetBehaviorCallbackFct(DaCharacterKeepOnFloorLimitsCB);

    *pproto = proto;
    return CK_OK;
}

int DoCharacterKeepOnFloorLimits(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKCharacter *carac = (CKCharacter *)beh->GetOwner();

    CKContext *ctx = behcontext.Context;
    CKLevel *level = ctx->GetCurrentLevel();
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    if (beh->IsInputActive(0))
        beh->ActivateInput(0, FALSE);

    VxVector Old;
    beh->GetLocalParameterValue(0, &Old);

    VxVector pos, T;
    float distup, DistanceUp, distdown, DistanceDown;
    CK3dEntity *eup, *Up, *edown, *Down;

    CK3dEntity *pas = carac->GetFloorReferenceObject();
    if (pas)
    {
        carac->GetPosition(&pos);

        VxVector RefPos;
        pas->GetPosition(&RefPos);
        CKFloorPoint fp;
        if (floorman->GetNearestFloors(RefPos, &fp))
        {
            distup = fp.m_UpDistance;
            distdown = fp.m_DownDistance;
            eup = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
            edown = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

            const VxBbox &Box = carac->GetBoundingBox();

            VxVector FrontLeft, FrontRight, BackLeft, BackRight, LocalFrontLeft, LocalFrontRight, LocalBackLeft, LocalBackRight;
            FrontLeft.x = Box.Min.x;
            FrontLeft.y = Box.Min.y;
            FrontLeft.z = Box.Max.z;

            FrontRight.x = Box.Max.x;
            FrontRight.y = Box.Min.y;
            FrontRight.z = Box.Max.z;

            BackLeft.x = Box.Min.x;
            BackLeft.y = Box.Min.y;
            BackLeft.z = Box.Min.z;

            BackRight.x = Box.Max.x;
            BackRight.y = Box.Min.y;
            BackRight.z = Box.Min.z;

            if (eup)
            {
                if (!floorman->GetNearestFloors(FrontLeft, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(FrontRight, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(BackLeft, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(BackRight, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                pos.y += distup;
            }
            else if (edown)
            {
                if (!floorman->GetNearestFloors(FrontLeft, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(FrontRight, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(BackLeft, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                if (!floorman->GetNearestFloors(BackRight, &fp))
                {
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

                    T = (Old - pos) * 1.5 + pos;
                    carac->SetPosition(&T);

                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }

                pos.y += distdown;
            }

            carac->SetPosition(&pos);
            beh->SetLocalParameterValue(0, &pos);
        }
    }
    else
    {
        const VxBbox &Box = carac->GetBoundingBox();
        pos = (Box.Max + Box.Min) / 2.0f;
        pos.y = Box.Min.y;

        CKFloorPoint fp;
        if (floorman->GetNearestFloors(pos, &fp))
        {
            distup = fp.m_UpDistance;
            distdown = fp.m_DownDistance;
            eup = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
            edown = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

            VxVector FrontLeft, FrontRight, BackLeft, BackRight, LocalFrontLeft, LocalFrontRight, LocalBackLeft, LocalBackRight;
            FrontLeft.x = Box.Min.x;
            FrontLeft.y = Box.Min.y;
            FrontLeft.z = Box.Max.z;

            FrontRight.x = Box.Max.x;
            FrontRight.y = Box.Min.y;
            FrontRight.z = Box.Max.z;

            BackLeft.x = Box.Min.x;
            BackLeft.y = Box.Min.y;
            BackLeft.z = Box.Min.z;

            BackRight.x = Box.Max.x;
            BackRight.y = Box.Min.y;
            BackRight.z = Box.Min.z;

            carac->GetPosition(&pos);
            if (eup)
            {
                CKMesh *FloorMesh = eup->GetCurrentMesh();
                const VxBbox &FloorBox = FloorMesh->GetLocalBox();

                eup->InverseTransform(&LocalFrontLeft, &FrontLeft);
                eup->InverseTransform(&LocalFrontRight, &FrontRight);
                eup->InverseTransform(&LocalBackLeft, &BackLeft);
                eup->InverseTransform(&LocalBackRight, &BackRight);
                eup->InverseTransform(&pos, &pos);

                // Test Front Left Point
                if (LocalFrontLeft.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontLeft.x - FloorBox.Max.x);
                }

                if (LocalFrontLeft.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontLeft.x - FloorBox.Min.x);
                }

                if (LocalFrontLeft.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontLeft.z - FloorBox.Max.z);
                }

                if (LocalFrontLeft.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontLeft.z - FloorBox.Min.z);
                }

                // Test Front Right Point
                if (LocalFrontRight.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontRight.x - FloorBox.Max.x);
                }

                if (LocalFrontRight.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontRight.x - FloorBox.Min.x);
                }

                if (LocalFrontRight.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontRight.z - FloorBox.Max.z);
                }

                if (LocalFrontRight.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontRight.z - FloorBox.Min.z);
                }

                // Test Back Left Point
                if (LocalBackLeft.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackLeft.x - FloorBox.Max.x);
                }

                if (LocalBackLeft.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackLeft.x - FloorBox.Min.x);
                }

                if (LocalBackLeft.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackLeft.z - FloorBox.Max.z);
                }

                if (LocalBackLeft.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackLeft.z - FloorBox.Min.z);
                }

                // Test Back Right Point
                if (LocalBackRight.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackRight.x - FloorBox.Max.x);
                }

                if (LocalBackRight.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackRight.x - FloorBox.Min.x);
                }

                if (LocalBackRight.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackRight.z - FloorBox.Max.z);
                }

                if (LocalBackRight.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackRight.z - FloorBox.Min.z);
                }

                eup->Transform(&pos, &pos);
                pos.y += distup;
            }
            else if (edown)
            {
                CKMesh *FloorMesh = edown->GetCurrentMesh();
                const VxBbox &FloorBox = FloorMesh->GetLocalBox();

                edown->InverseTransform(&LocalFrontLeft, &FrontLeft);
                edown->InverseTransform(&LocalFrontRight, &FrontRight);
                edown->InverseTransform(&LocalBackLeft, &BackLeft);
                edown->InverseTransform(&LocalBackRight, &BackRight);
                edown->InverseTransform(&pos, &pos);

                // Test Front Left Point
                if (LocalFrontLeft.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontLeft.x - FloorBox.Max.x);
                }

                if (LocalFrontLeft.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontLeft.x - FloorBox.Min.x);
                }

                if (LocalFrontLeft.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontLeft.z - FloorBox.Max.z);
                }

                if (LocalFrontLeft.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(FrontLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontLeft.z - FloorBox.Min.z);
                }

                // Test Front Right Point
                if (LocalFrontRight.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontRight.x - FloorBox.Max.x);
                }

                if (LocalFrontRight.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalFrontRight.x - FloorBox.Min.x);
                }

                if (LocalFrontRight.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontRight.z - FloorBox.Max.z);
                }

                if (LocalFrontRight.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(FrontRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalFrontRight.z - FloorBox.Min.z);
                }

                // Test Back Left Point
                if (LocalBackLeft.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackLeft.x - FloorBox.Max.x);
                }

                if (LocalBackLeft.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackLeft.x - FloorBox.Min.x);
                }

                if (LocalBackLeft.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackLeft.z - FloorBox.Max.z);
                }

                if (LocalBackLeft.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(BackLeft, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackLeft.z - FloorBox.Min.z);
                }

                // Test Back Right Point
                if (LocalBackRight.x > FloorBox.Max.x)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackRight.x - FloorBox.Max.x);
                }

                if (LocalBackRight.x < FloorBox.Min.x)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.x -= (LocalBackRight.x - FloorBox.Min.x);
                }

                if (LocalBackRight.z > FloorBox.Max.z)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackRight.z - FloorBox.Max.z);
                }

                if (LocalBackRight.z < FloorBox.Min.z)
                {
                    floorman->GetNearestFloors(BackRight, &fp);
                    DistanceUp = fp.m_UpDistance;
                    DistanceDown = fp.m_DownDistance;
                    Up = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
                    Down = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
                    if (!Up && !Down)
                        pos.z -= (LocalBackRight.z - FloorBox.Min.z);
                }

                edown->Transform(&pos, &pos);
                pos.y += distdown;
            }

            carac->SetPosition(&pos);
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}
