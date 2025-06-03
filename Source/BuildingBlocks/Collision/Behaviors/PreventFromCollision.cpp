/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SimpleManageCollision
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSimpleManageCollisionDecl();
CKERROR CreateSimpleManageCollisionProto(CKBehaviorPrototype **);
int SimpleManageCollision(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSimpleManageCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Prevent Collision");
    od->SetDescription("Prevents the 3DEntity from colliding obstacles.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Collision Occured: </SPAN>is activated when a collision occurs.<BR>
    <SPAN CLASS=out>No Collision: </SPAN>is activated while no collision occurs.<BR>
    <BR>
    <SPAN CLASS=pin>Geometry Precision: </SPAN>you can force a geometry precision for all the obstacles or use the attributes specified precision with the 'Automatic' value.<BR>
    <SPAN CLASS=pin>Detection Tests: </SPAN>the maximum number of tests to be performed in addition to the initial one, to determine if a collision occured. The building block stops testing at the first collision it encounters, then tests for a safe position from that point.<BR>
    <SPAN CLASS=pin>Safe Position Tests: </SPAN>the maximum number of tests to be performed in addition to the initial one, to determine the nearest safe position from the collision point.<BR>
    */
    /* warning:
    - The attached 3d entity will be given the "Moving Obstacle" attribute by this building block, but for each object you wish to avoid, the objects need to be assigned the "Fixed Obstacle" or "Moving Obstacle" attribute.<BR>
    */
    od->SetCategory("Collisions/3D Entity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x301971f0, 0x7919343f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSimpleManageCollisionProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateSimpleManageCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Prevent Collision");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Collision Occured");
    proto->DeclareOutput("No Collision");

    proto->DeclareInParameter("Geometry Precision", CKPGUID_OBSTACLEPRECISIONBEH, "0");
    proto->DeclareInParameter("Detection Tests", CKPGUID_INT, "0");
    proto->DeclareInParameter("Safe Position Tests", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SimpleManageCollision);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SimpleManageCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKAttributeManager *attman = ctx->GetAttributeManager();

    // we get the owner
    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        if (beh->IsInputActive(1))
            beh->ActivateInput(1, FALSE);
        else if (beh->IsInputActive(0))
            beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }

    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }
    else if (beh->IsInputActive(0))
    {
        beh->SetPriority(-32000);
        beh->ActivateInput(0, FALSE);
        ent->SetAttribute(attman->GetAttributeTypeByName("Moving Obstacle"));
    }

    // Set Input state
    beh->ActivateInput(0, FALSE);

    // Get the precision of detection
    CK_GEOMETRICPRECISION precis_level = CKCOLLISION_NONE;
    beh->GetInputParameterValue(0, &precis_level);

    // Get the precision of detection
    int detprecision = 0;
    beh->GetInputParameterValue(1, &detprecision);

    // Get the precision of detection
    int repprecision = 0;
    beh->GetInputParameterValue(2, &repprecision);

    CKBOOL Collision = FALSE;
    CKBOOL Coll1 = FALSE;

    // we get the current collision manager
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    ImpactDesc imp;
    //	for (;;) {
    Collision = cm->DetectCollision(ent, precis_level, repprecision, detprecision, IMPACTWORLDMATRIX, &imp);
    if (Collision)
    {
        Coll1 = TRUE;
        // Replacement
        ent->SetWorldMatrix(imp.m_ImpactWorldMatrix);

        /*
        float velocity = Magnitude(v);
        VxVector nv = Reflect(v,imp.m_ImpactNormal);
        float velocity2 = Magnitude(nv);
        nv *= 100.0f;
        ent->Translate(&nv);
        nv /= 100.0f;
        beh->SetLocalParameterValue(0,&nv);
        */
    }
    // else break;

    // TODO : le replacement...
    //	}

    if (Coll1)
    {
        beh->ActivateOutput(0);
        beh->ActivateOutput(1, FALSE);
    }
    else
    {
        beh->ActivateOutput(0, FALSE);
        beh->ActivateOutput(1);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
