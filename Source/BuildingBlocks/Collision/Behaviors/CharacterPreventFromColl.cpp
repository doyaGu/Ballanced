/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		  Character Prevent From Collision
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCharacterPreventFromCollisionDecl();
CKERROR CreateCharacterPreventFromCollisionProto(CKBehaviorPrototype **);
int CharacterPreventFromCollision(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCharacterPreventFromCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Character Prevent From Collision");
    od->SetDescription("Prevents the character from colliding obstacles.");
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
    <BR>
    <SPAN CLASS=pout>BodyPart Touched: </SPAN>the collided sub object (body part) of the character.<BR>
    <SPAN CLASS=pout>Touched Obstacle: </SPAN>3D Entity touched by the body part (retrieves the parent of the obstacle if you specified the collision to be hierachical).<BR>
    <SPAN CLASS=pout>Touched Sub Obstacle: </SPAN>3D Entity touched by the body part (retrieves the exact object on a hierachical collision).<BR>
    For a quick How To, take a look at <a href="preventfromcollision.htm">this</a>.<BR>
    */
    /* warning:
    - Unlike the "DetectCollision" building block, "Character Prevent From Collision" doesn't need to be looped if you want the collision to be tested every frame.<BR>
    - The attached character will be given the "Moving Obstacle" attribute by this building block, but for each object you wish to avoid, the objects need to be assigned the "Fixed Obstacle" or "Moving Obstacle" attribute.<BR>
    */
    od->SetCategory("Collisions/Character");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x319971f0, 0x7919193f));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCharacterPreventFromCollisionProto);
    od->SetCompatibleClassId(CKCID_CHARACTER);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateCharacterPreventFromCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Character Prevent From Collision");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Collision Occured");
    proto->DeclareOutput("No Collision");

    proto->DeclareInParameter("Geometry Precision", CKPGUID_OBSTACLEPRECISIONBEH, "0");
    proto->DeclareInParameter("Detection Tests", CKPGUID_INT, "0");
    proto->DeclareInParameter("Safe Position Tests", CKPGUID_INT, "0");

    proto->DeclareOutParameter("BodyPart Touched", CKPGUID_BODYPART);
    proto->DeclareOutParameter("Touched Obstacle", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Touched Sub Obstacle", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(CharacterPreventFromCollision);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int CharacterPreventFromCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);
    CKAttributeManager *attman = ctx->GetAttributeManager();

    // we get the owner
    CKCharacter *ent = (CKCharacter *)beh->GetTarget();
    if (!ent)
    {
        if (beh->IsInputActive(1))
            beh->ActivateInput(1, FALSE);
        else
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
        CKParameterOut *pout = ent->GetAttributeParameter(attman->GetAttributeTypeByName("Moving Obstacle"));
        if (!pout)
            return FALSE;
        if (pout->GetGUID() == CKPGUID_OBSTACLE)
        {
            CK_ID *paramids = (CK_ID *)pout->GetReadDataPtr();
            CKParameterOut *hierapout = (CKParameterOut *)ctx->GetObject(paramids[1]);
            CKBOOL hiera = TRUE;
            hierapout->SetValue(&hiera);
        }
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

    CK_IMPACTINFO flags;
    flags = (CK_IMPACTINFO)(IMPACTWORLDMATRIX | OWNERENTITY | OBSTACLETOUCHED | SUBOBSTACLETOUCHED | IMPACTNORMAL);

    ImpactDesc imp;
    //	for (;;) {
    Collision = cm->DetectCollision(ent, precis_level, repprecision, detprecision, flags, &imp);
    if (Collision)
    {
        CKBOOL avoidhooking = TRUE;
        // TODO : remove this shit
        beh->GetInputParameterValue(3, &avoidhooking);
        if (avoidhooking)
        {
            VxVector lcardir, cardir;
            ent->GetEstimatedVelocity(behcontext.DeltaTime, &lcardir);
            ent->TransformVector(&cardir, &lcardir);
            CK3dEntity *ob;
            if (imp.m_SubObstacleTouched)
            {
                ob = (CK3dEntity *)ctx->GetObject(imp.m_SubObstacleTouched);
            }
            else
            {
                ob = (CK3dEntity *)ctx->GetObject(imp.m_ObstacleTouched);
            }

            VxVector pos, opos;
            ent->GetPosition(&pos);
            ob->GetBaryCenter(&opos);
            pos.y = opos.y;
            ob->InverseTransform(&pos, &pos);
            const VxBbox &box = ob->GetBoundingBox(TRUE);
            VxVector bary, scale;
            bary = (box.Min + box.Max) * 0.5f;
            scale = (box.Max - box.Min) * 0.5f;

            VxVector v = pos - bary;

            // we create the local normal
            VxVector normal(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < 3; ++i)
            {
                if (fabs(v.v[i]) > scale.v[i])
                {
                    VxVector p(0.0f, 0.0f, 0.0f);
                    if (v.v[i] > 0)
                        p.v[i] = 1.0f;
                    else
                        p.v[i] = -1.0f;
                    normal += p;
                }
            }

            ob->TransformVector(&normal, &normal);
            normal.Normalize();

            // Replacement
            if (DotProduct(cardir, normal) <= 0)
            {
                ent->SetWorldMatrix(imp.m_ImpactWorldMatrix);
            }
        }
        else
        {
            ent->SetWorldMatrix(imp.m_ImpactWorldMatrix);
        }
        beh->SetOutputParameterValue(0, &imp.m_OwnerEntity);
        beh->SetOutputParameterValue(1, &imp.m_ObstacleTouched);
        beh->SetOutputParameterValue(2, &imp.m_SubObstacleTouched);
    }
    // else break;

    // TODO : le replacement...
    //	}

    if (Collision)
    {
        beh->ActivateOutput(0);
        beh->ActivateOutput(1, FALSE);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_ACTIVATENEXTFRAME;
}
