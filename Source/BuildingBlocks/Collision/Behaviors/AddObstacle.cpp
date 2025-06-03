/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		              Add Obstacle
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddObstacleDecl();
CKERROR CreateAddObstacleProto(CKBehaviorPrototype **);
int AddObstacle(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddObstacleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Obstacle");
    od->SetDescription("Adds the 3D Entity to the list of the Collision Manager.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    Use this building block on 3d Objects that must be tested during collision detection.
    This building block is kept for compatibility but the approved method for declaring obstacle is now
    either to add them manually an obstacle attribute(see Attribute Setup), or to use the "Set Attribute" behavior to set them dynamically.<BR>
    */
    od->SetCategory("Collisions/Obstacle");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x321a42c4, 0x6133d5));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddObstacleProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateAddObstacleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Obstacle");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(AddObstacle);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddObstacle(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *Entity = (CK3dEntity *)beh->GetTarget();
    if (!Entity)
        return CKBR_OWNERERROR;

    CKCollisionManager *cm = (CKCollisionManager *)behcontext.Context->GetManagerByGuid(COLLISION_MANAGER_GUID);
    cm->AddObstacle(Entity);

    return CKBR_OK;
}
