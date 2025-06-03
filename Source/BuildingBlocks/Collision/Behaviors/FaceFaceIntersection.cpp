/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Face Face Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFaceFaceIntersectionDecl();
CKERROR CreateFaceFaceIntersectionProto(CKBehaviorPrototype **);
int FaceFaceIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFaceFaceIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Face Face Intersection");
    od->SetDescription("Detects the collision between two 3D entities at the face level.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the 2 entities are in intersection.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the 2 entities are not in intersection.<BR>
    <BR>
    <SPAN CLASS=pin>Entity 1: </SPAN>first entity to be tested.<BR>
    <SPAN CLASS=pin>Entity 2: </SPAN>second entity to be tested.<BR>
    <BR>
    This building block detects the collision between each faces of the first entity and each faces of the second entity.<BR>
    */
    /* warning:
    - This building block tells at a given time if the object 1 is touching or not the object 2.<BR>
    If you need to execute this test every 5 rendering process, you should then loop this building block with a 5 frame link.<BR>
    If you need to execute this test at each rendering process, you should then loop this building block with a 1 frame link.<BR>
    */
    od->SetCategory("Collisions/Intersection");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x11426a61, 0x1a815584));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFaceFaceIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateFaceFaceIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Face Face Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Entity 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Entity 2", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FaceFaceIntersection);

    *pproto = proto;
    return CK_OK;
}

int FaceFaceIntersection(const CKBehaviorContext &behcontext)
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

    CKBOOL Collision = cm->FaceFaceIntersection(ent1, ent2);

    if (Collision)
    {
        beh->ActivateOutput(0);
        beh->ActivateOutput(1, FALSE);
    }
    else
    {
        beh->ActivateOutput(0, FALSE);
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
