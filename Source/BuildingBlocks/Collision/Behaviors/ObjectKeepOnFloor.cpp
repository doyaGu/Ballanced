/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Object Keep On Floor
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateObjectKeepOnFloorBehaviorProto(CKBehaviorPrototype **);
int DoObjectKeepOnFloor(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorObjectKeepOnFloorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Object Keep On Floor");
    od->SetDescription("Forces an object stay on a declared floor (so it will not hover above it).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    Makes a 3D entity stay on the floors.<BR>
    This building block should be looped. It uses the object's bounding box to test the object's position
    relative to the floor. Floors must be defined beforehand, either by using the building block
    "Declare Floors" or by giving them the "Floor" attribute.<BR>
    */
    /* warning:
    - You must loop this building block if you need to perform this action continually.<BR>
  - This building block has been designed to work with 3D Objects only. See 'Character Keep On Floor' for characters.<BR>
    */
    od->SetCategory("3D Transformations/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x43617677, 0x7de15e12));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateObjectKeepOnFloorBehaviorProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateObjectKeepOnFloorBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Object Keep On Floor");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DoObjectKeepOnFloor);

    *pproto = proto;
    return CK_OK;
}

int DoObjectKeepOnFloor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();
    if (!entity)
        return CKBR_OWNERERROR;

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKFloorPoint fp;
    CK_FLOORNEAREST floorn;

    const VxBbox &box = entity->GetBoundingBox();

    VxVector pos = (box.Max + box.Min) * 0.5f;
    pos.y = box.Min.y;

    if (floorn = floorman->GetNearestFloors(pos, &fp))
    {
        switch (floorn)
        {
        case CKFLOOR_DOWN:
            entity->Translate3f(0, fp.m_DownDistance, 0);
            break;
        case CKFLOOR_UP:
            entity->Translate3f(0, fp.m_UpDistance, 0);
            break;
        }
    }

    return CKBR_OK;
}
