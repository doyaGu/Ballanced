/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		      Avoiding Obstacles  v2
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAvoidingObstacles2Decl();
CKERROR CreateAvoidingObstacles2Proto(CKBehaviorPrototype **);
int AvoidingObstacles2(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAvoidingObstacles2Decl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Avoid Obstacles");
    od->SetDescription("Modifies the 'Direction' input parameter to avoid obstacles.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in the process loop.<BR>
    <BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pin>Direction: </SPAN>initial direction.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>referential in which the direction is expressed.<BR>
    <SPAN CLASS=pin>Inertia: </SPAN>affects the variation of direction. Ranged from 0 to 1, a high inertia decreases the direction changes.<BR>
    <SPAN CLASS=pin>Influence Factor: </SPAN>determines the radius of the influence sphere of an obstacle : Radius = Influence Factor * Radius of Obstacle Bounding Sphere.<BR>
    <SPAN CLASS=pin>Keep On Floor: </SPAN>if this boolean is set to TRUE, the object will stay on the floor<BR>
    <BR>
    <SPAN CLASS=pout>New Direction : </SPAN>the direction modified by the proximity of obstacles.<BR>
    <BR>
    */
    /* warning:
    - Each 3D entity to be considered as obstacle should be added to the collision manager's list using 'Add Obstacle'.<BR>
    - This building block doesn't prevent object to intersect from each other. It acts in best to avoid collisions with influence spheres.<BR>
    */
    od->SetCategory("Collisions/Influence");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x39460907, 0x30f329af));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateAvoidingObstacles2Proto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateAvoidingObstacles2Proto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Avoid Obstacles");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Inertia", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Influence Factor", CKPGUID_FLOAT, "2");
    proto->DeclareInParameter("Keep On Floor", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("New Direction", CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    // proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AvoidingObstacles2);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AvoidingObstacles2(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set Input state
    if (beh->IsInputActive(0))
        beh->ActivateInput(0, FALSE);
    else
        beh->ActivateInput(1, FALSE);

    CK3dEntity *Entity = (CK3dEntity *)beh->GetTarget();
    if (!Entity)
    {
        beh->ActivateOutput(0, TRUE);
        return CKBR_OWNERERROR;
    }

    // Get Initial Direction
    VxVector Direction;
    beh->GetInputParameterValue(0, &Direction);

    // Get Referential
    CK3dEntity *Referential = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (Referential)
        Referential->TransformVector(&Direction, &Direction);
    Direction.Normalize();

    // Get inertia
    float Inertia = 0.5f;
    beh->GetInputParameterValue(2, &Inertia);
    // Inertia = 0.7f + Inertia/10.0f;

    // Get influence factor
    float Factor = 2.0f;
    beh->GetInputParameterValue(3, &Factor);

    // Keep on floor ?
    CKBOOL KeepOnFloor = TRUE;
    beh->GetInputParameterValue(4, &KeepOnFloor);

    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);
    CKFloorManager *FloorManager = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    if (KeepOnFloor)
    {
        const VxBbox &box = Entity->GetBoundingBox();

        VxVector pos;
        pos = (box.Max + box.Min) / 2.0f;
        pos.y = box.Min.y;

        float distup, distdown;
        CK3dEntity *eup, *edown;

        CKFloorPoint fp;
        if (FloorManager->GetNearestFloors(pos, &fp))
        {
            distup = fp.m_UpDistance;
            distdown = fp.m_DownDistance;
            eup = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
            edown = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
            Entity->GetPosition(&pos);
            if (eup)
                pos.y += distup;
            else if (edown)
                pos.y += distdown;

            Entity->SetPosition(&pos);
        }
    }

    // Compute deviation generated by the Obstacles, store it in Deviation
    CK3dEntity *Obstacle;
    VxVector EntityCenter, ObstacleCenter, Deviation(0.0f, 0.0f, 0.0f), V;
    Entity->GetBaryCenter(&EntityCenter);

    float R, dist, amplitude, A = 10.0f;
    int ObstacleCount = cm->GetObstacleCount();

    for (int i = 0; i < ObstacleCount; i++)
    {
        Obstacle = cm->GetObstacle(i);
        if (!Obstacle)
        {
            beh->ActivateOutput(0, TRUE);
            return CKBR_PARAMETERERROR;
        }

        Obstacle->GetBaryCenter(&ObstacleCenter);
        R = Obstacle->GetRadius();

        dist = Magnitude(ObstacleCenter - EntityCenter);
        if (dist < Factor * R)
        {
            amplitude = A * (1 - dist / (Factor * R));
            V = (EntityCenter - ObstacleCenter) * (amplitude / dist);
            V.Normalize();

            if (KeepOnFloor)
            {
                Deviation.x += V.x;
                Deviation.z += V.z;
            }
            else
                Deviation += V;
        }
    }
    Deviation.Normalize();

    Direction = Direction * Inertia + Deviation * (1 - Inertia);

    if (Referential)
        Referential->InverseTransformVector(&Direction, &Direction);
    Direction.Normalize();

    beh->SetOutputParameterValue(0, &Direction);

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}
