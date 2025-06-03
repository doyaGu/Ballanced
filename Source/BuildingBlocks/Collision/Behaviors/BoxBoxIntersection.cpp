/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Box Box Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBoxBoxIntersectionDecl();
CKERROR CreateBoxBoxIntersectionProto(CKBehaviorPrototype **);
int BoxBoxIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoxBoxIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Box Box Intersection");
    od->SetDescription("Detects the collision between two 3D entities at the box level.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the 2 entities are in intersection.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the 2 entities are not in intersection.<BR>
    <BR>
    <SPAN CLASS=pin>Entity 0: </SPAN>first entity to be tested.<BR>
    <SPAN CLASS=pin>Entity 1: </SPAN>second entity to be tested.<BR>
    <SPAN CLASS=pin>Hierarchy 0: </SPAN>specify whether to concider the bounding box of the first entity and its children, or just the bounding box of the first entity.<BR>
    <SPAN CLASS=pin>Hierarchy 1: </SPAN>specify whether to concider the bounding box of the second entity and its children, or just the bounding box of the second entity.<BR>
    <BR>
    This building block detects the collision between two entity's bounding boxes.<BR>
    */
    /* warning:
    - This building block tells at a given time if the object 1 is touching or not the object 2.<BR>
    If you need to execute this test every 5 rendering process, you should then loop this building block with a 5 frame link.<BR>
    If you need to execute this test at each rendering process, you should then loop this building block with a 1 frame link.<BR>
    */
    od->SetCategory("Collisions/Intersection");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x64154401, 0x76cf37af));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoxBoxIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateBoxBoxIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Box Box Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Entity 0", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Entity 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy 0", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Hierarchy 1", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BoxBoxIntersection);

    *pproto = proto;
    return CK_OK;
}

int BoxBoxIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    // Set Input state
    beh->ActivateInput(0, FALSE);

    CK3dEntity *ent1 = (CK3dEntity *)beh->GetInputParameterObject(0);
    CK3dEntity *ent2 = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!ent1 || !ent2)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKBOOL eh1 = FALSE;
    beh->GetInputParameterValue(2, &eh1);
    CKBOOL eh2 = FALSE;
    beh->GetInputParameterValue(3, &eh2);

    CKBOOL Collision;

    Collision = cm->BoxBoxIntersection(ent1, eh1, TRUE, ent2, eh2, TRUE);

    if (Collision)
    {
        beh->ActivateOutput(0);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
