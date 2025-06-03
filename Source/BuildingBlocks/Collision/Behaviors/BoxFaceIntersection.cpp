/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Box FAce Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBoxFaceIntersectionDecl();
CKERROR CreateBoxFaceIntersectionProto(CKBehaviorPrototype **);
int BoxFaceIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoxFaceIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Box Face Intersection");
    od->SetDescription("Detects the collision between the first entity's bounding box and each faces of the second entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the 2 entities are in intersection.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the 2 entities are not in intersection.<BR>
    <BR>
    <SPAN CLASS=pin>Entity 1: </SPAN>first entity to be tested.<BR>
    <SPAN CLASS=pin>Entity 2: </SPAN>second entity to be tested.<BR>
    <BR>
    This building block detects the collision between the first entity's bounding box and each faces of the second entity.<BR>
    */
    /* warning:
    - This building block tells at a given time if the object 1 is touching or not the object 2.<BR>
    If you need to execute this test every 5 rendering process, you should then loop this building block with a 5 frame link.<BR>
    If you need to execute this test at each rendering process, you should then loop this building block with a 1 frame link.<BR>
    */
    od->SetCategory("Collisions/Intersection");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf1c5480, 0x4a811cb));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoxFaceIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateBoxFaceIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Box Face Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Entity 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Entity 2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy 1", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BoxFaceIntersection);

    *pproto = proto;
    return CK_OK;
}

int BoxFaceIntersection(const CKBehaviorContext &behcontext)
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

    CKBOOL Collision = cm->BoxFaceIntersection(ent1, eh1, TRUE, ent2);

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
