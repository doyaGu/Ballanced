/*************************************************************************/
/*	File : CollisionManager.cpp										     */
/*																		 */
/*************************************************************************/
#include "CKAll.h"

#include "CollisionManager.h"

// {secret}
// LARGE_INTEGER temp1,temp2;

/*************************************************
Name: DetectCollision

Summary: Check if an entity is in collision with any of the declared obstacles, at the time the function is called.

Arguments:
    ent: the object to be checked (it needs to be defined as an obstacle, probably moving)
    precis_level: the geometric precision level you want the tests to be at. Use CKCOLLISION_NONE if you want the tests
    to use the precision chosen for each obstacle. Otherwise, you can force the tests to be on bounding boxes for
    everyone by transmitting CKCOLLISION_BOX for example...
    p_replacementPrecision: an integer that describes the maximum number of tests to be executed to determine
    the nearest safe position to the collision gjkPoint, testing backwards to the starting gjkPoint. (By safe
    position, we mean posion with no collision at all. If it can't found anyone, the starting matrix will be given).
    p_detectionPrecision: an integer that describes the maximum number of tests to be executed to determine
    if a collision occured starting with the starting gjkPoint (before the behavioral process begins) and testing forward
    to the current gjkPoint. The behavior stops testing at the first collision it encounters, then tests for a
    safe position from that gjkPoint.
    ImpactFlags: flags determining which information you want to be calculated and returned in the ImpactDesc structure.
    imp: pointer to an ImpactDesc structure that will be filled with all the information you asked, NULL if you don't need these info.

Return Value:
    TRUE if a collision occured, FALSE otherwise.

{Group:Manager Detection}

See also: CollisionManager, CollisionManager::AddObstacle, ImpactDesc
*************************************************/
CKBOOL CollisionManager::DetectCollision(CK3dEntity *ent, CK_GEOMETRICPRECISION precis_level, int p_replacementPrecision, int p_detectionPrecision, CK_IMPACTINFO ImpactFlags, ImpactDesc *imp)
{
    if (p_detectionPrecision < 0)
        p_detectionPrecision = 0;

    if (p_replacementPrecision < 0)
        p_replacementPrecision = 0;

    int detectionPrecision;
    int replacementPrecision;

    m_BoundariesManager.UpdateEndMovings();

    CK_GEOMETRICPRECISION colltype;

    CK3dEntity *obstaclesubentity = NULL;
    CK3dEntity *subentity = NULL;

    CKBOOL obmoving;
    VxMatrix obStartMatrix, obEndMatrix, obCurrentMatrix;

    VxMatrix lastSafeMatrix, startMatrix, endMatrix, currentMatrix, replacementMatrix;
    VxBbox oldbox;

    XArray<CKObject *> collarray;

    CK_GEOMETRICPRECISION enttype;
    CKBOOL enthiera;
    if (ReadAttributeValues(ent, enttype, enthiera))
    {
        // The object is an obstacle
        int ind = SeekMovingMatrix(ent);
        if (ind < m_MovingEntitiesCount)
        {
            memcpy(startMatrix, m_MovingEntitiesMatrix[ind].m_WorldMatrix, sizeof(VxMatrix));
            memcpy(&oldbox, &(m_MovingEntitiesMatrix[ind].m_LastWorldBox), sizeof(VxBbox));
        }
        else
            startMatrix = ent->GetWorldMatrix();
        endMatrix = ent->GetWorldMatrix();

        // we first find all the potentially overlapped obstacles
        collarray = m_BoundariesManager.FindOverlapping(ent);
    }
    else
    {
        // The object is not an obstacle
        enttype = CKCOLLISION_BOX;
        enthiera = FALSE;
        endMatrix = ent->GetWorldMatrix();
        VxBbox totalbox = ent->GetBoundingBox();
        if (ent->GetFlags() & CK_3DENTITY_UPDATELASTFRAME)
        { // we have the old information for the object
            startMatrix = ent->GetLastFrameMatrix();
            // we calculate the old bounding box in the world
            oldbox = ent->GetBoundingBox(TRUE);
            oldbox.TransformFrom(oldbox, startMatrix);

            // we add the old box to the new one
            totalbox.Merge(oldbox);
        }
        else
        {
            startMatrix = ent->GetWorldMatrix();
        }

        // we first find all the potentially overlapped obstacles with the non-obstacle object
        collarray = m_BoundariesManager.FindOverlapping(totalbox);
    }
    if (precis_level)
        enttype = precis_level;

    if (collarray.Size() == 0)
    {
        return FALSE;
    }

    // we now try to eliminate the objects too far by a radius test
    // TODO : recode this
    // RadiusTest(ent,&collarray);

    /////////////////////////////////
    // Main Loop
    /////////////////////////////////

    // We clear the impact data
    m_TouchedFaceIndex = -1;
    m_TouchingFaceIndex = -1;

    CKBOOL collision = FALSE;
    // Potential obstacle iteration
    for (int i = 0; i < collarray.Size(); i++)
    {
        // we get the current obstacle
        CK3dEntity *ob = (CK3dEntity *)collarray[i];

        // we reinit the precision variables
        detectionPrecision = p_detectionPrecision;
        replacementPrecision = p_replacementPrecision;
        obstaclesubentity = NULL;

        // we test if the obstacle is moving
        if (ob->HasAttribute(m_MovingAttribute))
        {
            // we get the moving matrix index
            int ind = SeekMovingMatrix(ob);
            // we get the starting matrix
            if (ind < m_MovingEntitiesCount)
            {
                memcpy(obStartMatrix, m_MovingEntitiesMatrix[ind].m_WorldMatrix, sizeof(VxMatrix));
            }
            else
                obStartMatrix = ob->GetWorldMatrix();
            // we get the ending matrix
            obEndMatrix = ob->GetWorldMatrix();
            obmoving = TRUE;
        }
        else
        {
            obmoving = FALSE;
        }

        ///////////////////////////////////////
        // Detection loop
        ///////////////////////////////////////
        // we are trying to see if there is a collision

        // Variable initialisation
        CKBOOL obhiera;
        float detectionSlerpMat = 0.0f;
        float detectionSlerpStep = 1.0f;
        detectionSlerpStep = 1.0f / (float)(++detectionPrecision);

        // the lastest safe position is the one before we start
        memcpy(lastSafeMatrix, startMatrix, sizeof(VxMatrix));

        while (detectionPrecision)
        {
            detectionSlerpMat += detectionSlerpStep;
            detectionPrecision--;

            ReadAttributeValues(ob, colltype, obhiera);
            // we test if the user force a collision check type
            if (precis_level)
                colltype = precis_level;

            // TODO : stocker les interpolations de matrices (peut etre)
            // ou mettre le nombre d'interpolations en parametres de l'attribut
            // et calculer les interpolations always...
            // Peut etre essayer de se rapprocher plus du modele client serveur.
            Vx3DInterpolateMatrix(detectionSlerpMat, currentMatrix, startMatrix, endMatrix);
            ent->SetWorldMatrix(currentMatrix);

            // we need to interpolate the matrix obstacle too, if it's moving
            if (obmoving)
            {
                Vx3DInterpolateMatrix(detectionSlerpMat, obCurrentMatrix, obStartMatrix, obEndMatrix);
                ob->SetWorldMatrix(obCurrentMatrix);
            }

            if (ent->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
            {
                if (ob->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
                {
                    collision = IsHierarchyInCollisionWithHierarchy(ent, enttype, ob, colltype, &subentity, &obstaclesubentity);
                }
                else
                {
                    CK3dEntity *root = ent;
                    if (ent->GetClassID() == CKCID_CHARACTER)
                        root = ((CKCharacter *)ent)->GetRootBodyPart();
                    if (root)
                    {
                        subentity = IsInCollisionWithHierarchy(ob, colltype, root, enttype);
                        if (subentity)
                            collision = TRUE;
                        else
                            collision = FALSE;
                    }
                    else
                        collision = FALSE;
                }
            }
            else
            {
                if (ob->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
                {
                    obstaclesubentity = IsInCollisionWithHierarchy(ent, enttype, ob, colltype);
                    if (obstaclesubentity)
                        collision = TRUE;
                    else
                        collision = FALSE;
                }
                else
                {
                    collision = IsInCollision(ent, enttype, ob, colltype);
                }
            }

            if (collision)
                break;
            else
            {
                // we found a new position without collision : we keep it
                memcpy(lastSafeMatrix, currentMatrix, sizeof(VxMatrix));
            }
        }

        // We finished the Dtection tests and none collision occured
        // -> So we move to the next obstacle
        if (!collision)
        {
            // we reset the matrix of the obstacle
            if (obmoving)
                ob->SetWorldMatrix(obEndMatrix);

            continue;
        }

        // we found a collision : now we will process back the animation
        if (ent->GetClassID() == CKCID_CHARACTER)
        {
            CKCharacter *character = (CKCharacter *)ent;
            if (character->IsAutomaticProcess())
            {
                CKKeyedAnimation *anim = (CKKeyedAnimation *)character->GetActiveAnimation();
                if (anim)
                {
                    float animstep = anim->GetStep();
                    float deltat = m_Context->GetTimeManager()->GetLastDeltaTime();
                    if (anim->IsLinkedToFrameRate())
                    {
                        float newstep = anim->GetNextFrame(-deltat);
                        anim->SetFrame(newstep);
                        anim->CenterAnimation(newstep);
                        anim->SetCurrentStep(animstep);
                    }
                    else
                    {
                        anim->SetStep(animstep - 1);
                        anim->CenterAnimation(animstep);
                    }
                }
            }
        }

        ///////////////////////////////////////
        // Replacement loop
        ///////////////////////////////////////
        // we are trying to find a safe place

        float slerpMat = 1.0f;
        float slerpMatStep = 0.5f;
        VxMatrix tempMatrix;
        VxMatrix obTempMatrix;

        while (replacementPrecision)
        {
            if (collision)
            {
                slerpMat -= slerpMatStep;
            }
            else
            {
                slerpMat += slerpMatStep;
            }

            Vx3DInterpolateMatrix(slerpMat, tempMatrix, startMatrix, currentMatrix);
            ent->SetWorldMatrix(tempMatrix);

            // we need to interpolate the matrix obstacle too, if it's moving
            if (obmoving)
            {
                Vx3DInterpolateMatrix(slerpMat, obTempMatrix, obStartMatrix, obCurrentMatrix);
                ob->SetWorldMatrix(obTempMatrix);
            }

            if (ent->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
            {
                if (ob->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
                {
                    collision = IsHierarchyInCollisionWithHierarchy(ent, enttype, ob, colltype, &subentity, &obstaclesubentity);
                }
                else
                {
                    subentity = IsInCollisionWithHierarchy(ob, colltype, ent, enttype);
                    if (subentity)
                        collision = TRUE;
                    else
                        collision = FALSE;
                }
            }
            else
            {
                if (ob->GetFlags() & CK_3DENTITY_HIERARCHICALOBSTACLE)
                {
                    obstaclesubentity = IsInCollisionWithHierarchy(ent, enttype, ob, colltype);
                    if (obstaclesubentity)
                        collision = TRUE;
                    else
                        collision = FALSE;
                }
                else
                {
                    collision = IsInCollision(ent, enttype, ob, colltype);
                }
            }

            replacementPrecision--;
            slerpMatStep *= 0.5f;
            if (!collision)
                memcpy(replacementMatrix, tempMatrix, sizeof(VxMatrix));
        } // end of replacement

        // if the replacement hadn't find a safe place (or if no replacement was asked)
        // the replacement matrix is the startMatrix
        if (collision)
            memcpy(replacementMatrix, lastSafeMatrix, sizeof(VxMatrix));

        // We fill the impact structure...
        FillImpactStructure(ImpactFlags, imp, ent, subentity, ob, obstaclesubentity, replacementMatrix);

        // we reset the worldMatrix of the object : we just detect the collision
        ent->SetWorldMatrix(endMatrix);
        if (obmoving)
        {
            ob->SetWorldMatrix(obEndMatrix);
        }

        // we reset the matrix of the obstacle
        if (obmoving)
            ob->SetWorldMatrix(obEndMatrix);

        return TRUE;
    }

    // we reset the worldMatrix of the object : we just detect the collision
    ent->SetWorldMatrix(endMatrix);

    return FALSE;
}

CKBOOL CollisionManager::TestPair(CK_GEOMETRICPRECISION iPrecision, CK3dEntity *iEntity1, CK3dEntity *iEntity2, CK_IMPACTINFO iImpactFlags, ImpactDesc &oImpact)
{
    // Precision level : BOX, FACES ?
    CK_GEOMETRICPRECISION e1Geom = iPrecision;
    CK_GEOMETRICPRECISION e2Geom = iPrecision;

    // test hierarchic ?
    CKBOOL e1Hiera = FALSE;
    CKBOOL e2Hiera = FALSE;

    if (!iPrecision)
    { // No precission level has been forced
        ReadAttributeValues(iEntity1, e1Geom, e1Hiera);
        ReadAttributeValues(iEntity2, e2Geom, e2Hiera);
    }

    CKBOOL collide = FALSE;
    CK3dEntity *subEntity1 = NULL;
    CK3dEntity *subEntity2 = NULL;

    // Different cases following using hierarchies or not
    if (e1Hiera)
    {
        if (e2Hiera)
        {
            collide = IsHierarchyInCollisionWithHierarchy(iEntity1, e1Geom, iEntity2, e2Geom, &subEntity1, &subEntity2);
        }
        else
        {
            subEntity1 = IsInCollisionWithHierarchy(iEntity2, e2Geom, iEntity1, e1Geom);
            collide = (subEntity1) ? TRUE : FALSE;
        }
    }
    else
    {
        if (e2Hiera)
        {
            subEntity2 = IsInCollisionWithHierarchy(iEntity1, e1Geom, iEntity2, e2Geom);
            collide = (subEntity2) ? TRUE : FALSE;
        }
        else
        {
            collide = IsInCollision(iEntity1, e1Geom, iEntity2, e2Geom);
        }
    }

    // We fill the impact desc
    FillImpactStructure(iImpactFlags, &oImpact, iEntity1, subEntity1, iEntity2, subEntity2, VxMatrix::Identity());

    return collide;
}

const XArray<ImpactDesc> &CollisionManager::DetectCollision(CK_GEOMETRICPRECISION iPrecision, CK_IMPACTINFO iImpactFlags)
{
    StartProfile(); // Profiling

    // Clear the previous impacts
    m_ImpactArray.Resize(0);

    // we update the moving objects
    m_BoundariesManager.UpdateEndMovings();

    // actual filling
    const XArray<BoundariesManager::CPair> &pairs = m_BoundariesManager.FindOverlapping();

    // Max. is number of pairs
    m_ImpactArray.Resize(pairs.Size());
    m_ImpactArray.Resize(0);

    // filling impact desc with colliding pairs
    for (int i = 0; i < pairs.Size(); ++i)
    {
        CK3dEntity *e1 = (CK3dEntity *)CKGetObject(pairs[i].object1);
        CK3dEntity *e2 = (CK3dEntity *)CKGetObject(pairs[i].object2);

        // TODO : manage a detection step

        CKBOOL collide = TestPair(iPrecision, e1, e2, iImpactFlags, *m_ImpactArray.End());
        if (collide)
        {
            m_ImpactArray.Expand();
            //			m_ImpactArray.Back().m_OwnerEntity		= pairs[i].object1;
            //			m_ImpactArray.Back().m_ObstacleTouched	= pairs[i].object2;
        }
    }

    StopProfile(); // Profiling

    // returning value
    return m_ImpactArray;
}

void CollisionManager::FillImpactStructure(CK_IMPACTINFO ImpactFlags, ImpactDesc *imp, CK3dEntity *ent, CK3dEntity *subent, CK3dEntity *ob, CK3dEntity *subob, const VxMatrix &replacementMatrix)
{
    if (!imp)
        return; // no structure provided : we exit

    // Default Data, always written
    imp->m_Entity = CKOBJID(ent);
    imp->m_ObstacleTouched = CKOBJID(ob);

    ////////////////////////////////////////////////////////////
    // TOUCHED SUB OBSTACLE
    ///////////////////////////////////////////////////////////

    if (ImpactFlags & (SUBOBSTACLETOUCHED))
    {
        imp->m_SubObstacleTouched = CKOBJID(subob);
    }

    ////////////////////////////////////////////////////////////
    // TOUCHED SUB ENTITY
    ///////////////////////////////////////////////////////////

    if (ImpactFlags & (OWNERENTITY))
    {
        imp->m_OwnerEntity = CKOBJID(subent);
    }

    ////////////////////////////////////////////////////////////
    // TOUCHED VERTEX
    ///////////////////////////////////////////////////////////

    CK3dEntity *obstacle;
    if (subob)
        obstacle = subob;
    else
        obstacle = ob;
    CKMesh *obstaclemesh = obstacle->GetCurrentMesh();

    CK3dEntity *entity;
    if (subent)
        entity = subent;
    else
        entity = ent;
    CKMesh *entitymesh = entity->GetCurrentMesh();

    ////////////////////////////////////////////////////////////
    // TOUCHED FACE
    ///////////////////////////////////////////////////////////

    if (ImpactFlags & (TOUCHEDFACE | IMPACTNORMAL | IMPACTPOINT))
    {
        imp->m_TouchedFace = m_TouchedFaceIndex;
    }

    ////////////////////////////////////////////////////////////
    // TOUCHING FACE
    ///////////////////////////////////////////////////////////

    if (ImpactFlags & (TOUCHINGFACE))
    {
        imp->m_TouchingFace = m_TouchingFaceIndex;
    }
    // If we have both faces indices we compute
    // the estimated impact point
    // otherwise it will be computed later...

    if (obstaclemesh && entitymesh)
    {
        if ((ImpactFlags & IMPACTPOINT) && (m_TouchedFaceIndex >= 0) && (m_TouchingFaceIndex >= 0))
        {
            // avoid computing impact point later...
            (CKDWORD &)ImpactFlags &= ~IMPACTPOINT;

            //-- Compute face vertex positions in world coordinates
            VxVector &oV0 = obstaclemesh->GetFaceVertex(m_TouchedFaceIndex, 0);
            VxVector &oV1 = obstaclemesh->GetFaceVertex(m_TouchedFaceIndex, 1);
            VxVector &oV2 = obstaclemesh->GetFaceVertex(m_TouchedFaceIndex, 2);
            VxVector &eV0 = entitymesh->GetFaceVertex(m_TouchingFaceIndex, 0);
            VxVector &eV1 = entitymesh->GetFaceVertex(m_TouchingFaceIndex, 1);
            VxVector &eV2 = entitymesh->GetFaceVertex(m_TouchingFaceIndex, 2);
            VxVector oV0W, oV1W, oV2W;
            VxVector eV0W, eV1W, eV2W;
            obstacle->Transform(&oV0W, &oV0);
            obstacle->Transform(&oV1W, &oV1);
            obstacle->Transform(&oV2W, &oV2);
            entity->Transform(&eV0W, &eV0);
            entity->Transform(&eV1W, &eV1);
            entity->Transform(&eV2W, &eV2);

            VxRay o1(oV0W, oV1W), o2(oV1W, oV2W), o3(oV2W, oV0W);
            VxRay e1(eV0W, eV1W), e2(eV1W, eV2W), e3(eV2W, eV0W);
            VxVector eNorm = CrossProduct(eV2W - eV0W, eV1W - eV0W);
            VxVector oNorm = CrossProduct(oV2W - oV0W, oV1W - oV0W);
            VxVector baryCenter(0, 0, 0);
            VxVector res;
            float dist;
            int nbPoints = 0;

            if (VxIntersect::SegmentFace(o1, eV0W, eV1W, eV2W, eNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (VxIntersect::SegmentFace(o2, eV0W, eV1W, eV2W, eNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (VxIntersect::SegmentFace(o3, eV0W, eV1W, eV2W, eNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (VxIntersect::SegmentFace(e1, oV0W, oV1W, oV2W, oNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (VxIntersect::SegmentFace(e2, oV0W, oV1W, oV2W, oNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (VxIntersect::SegmentFace(e3, oV0W, oV1W, oV2W, oNorm, res, dist))
            {
                baryCenter += res;
                ++nbPoints;
            }
            if (nbPoints)
            {
                imp->m_ImpactPoint = baryCenter / (float)nbPoints;
            }
        }

        if (ImpactFlags & (TOUCHEDVERTEX))
        {
            // we get the nearest obstacle vertex from the entity barycenter
            // if we don't have face information otherwise we use the nearest
            // face point
            if (m_TouchedFaceIndex >= 0)
            {
                // Which vertex of the face is the nearest from the
                // estimated point of impact
                VxVector entPos;
                obstacle->InverseTransform(&entPos, &imp->m_ImpactPoint);
                int fa, fb, fc;
                obstaclemesh->GetFaceVertexIndex(m_TouchedFaceIndex, fa, fb, fc);
                VxVector v1, v2, v3;
                obstaclemesh->GetVertexPosition(fa, &v1);
                obstaclemesh->GetVertexPosition(fb, &v2);
                obstaclemesh->GetVertexPosition(fc, &v3);
                float d1 = SquareMagnitude(v1 - entPos);
                float d2 = SquareMagnitude(v2 - entPos);
                float d3 = SquareMagnitude(v3 - entPos);
                if (d1 > d2)
                {
                    if (d2 > d3)
                        imp->m_TouchedVertex = fc;
                    else
                        imp->m_TouchedVertex = fb;
                }
                else
                {
                    if (d1 > d3)
                        imp->m_TouchedVertex = fc;
                    else
                        imp->m_TouchedVertex = fa;
                }
            }
            else
            {
                VxVector pos;
                entity->GetBaryCenter(&pos);
                imp->m_TouchedVertex = GetNearestVertex(obstacle, pos);
            }
        }

        ////////////////////////////////////////////////////////////
        // TOUCHING VERTEX
        ///////////////////////////////////////////////////////////

        if (ImpactFlags & (TOUCHINGVERTEX | IMPACTPOINT))
        {
            if (m_TouchingFaceIndex >= 0)
            {
                // Which vertex of the face is the nearest from the
                // point of impact ?
                VxVector obsPos;
                entity->InverseTransform(&obsPos, &imp->m_ImpactPoint);
                int fa, fb, fc;
                entitymesh->GetFaceVertexIndex(m_TouchingFaceIndex, fa, fb, fc);
                VxVector v1, v2, v3;
                entitymesh->GetVertexPosition(fa, &v1);
                entitymesh->GetVertexPosition(fb, &v2);
                entitymesh->GetVertexPosition(fc, &v3);
                float d1 = SquareMagnitude(v1 - obsPos);
                float d2 = SquareMagnitude(v2 - obsPos);
                float d3 = SquareMagnitude(v3 - obsPos);
                if (d1 > d2)
                {
                    if (d2 > d3)
                        imp->m_TouchingVertex = fc;
                    else
                        imp->m_TouchingVertex = fb;
                }
                else
                {
                    if (d1 > d3)
                        imp->m_TouchingVertex = fc;
                    else
                        imp->m_TouchingVertex = fa;
                }
            }
            else
            {
                // We don't have face information so
                // we get the nearest entity vertex from the obstacle barycenter
                VxVector pos;
                obstacle->GetBaryCenter(&pos);
                imp->m_TouchingVertex = GetNearestVertex(entity, pos);
            }
        }

        ////////////////////////////////////////////////////////////
        // IMPACT gjkPoint
        ///////////////////////////////////////////////////////////
        if (ImpactFlags & IMPACTPOINT)
        {
            if (imp->m_TouchedFace >= 0 && imp->m_TouchingVertex >= 0)
            {
                // we get the nereast vertex of the entity in the referntial of the obstacle
                VxVector pos;
                VxVector tvert;
                entitymesh->GetVertexPosition(imp->m_TouchingVertex, &tvert);
                obstacle->InverseTransform(&pos, &tvert, entity);

                if (imp->m_TouchedFace < obstaclemesh->GetFaceCount())
                {
                    VxVector norm = obstaclemesh->GetFaceNormal(imp->m_TouchedFace);
                    // TODO : check if this necessary always...
                    norm.Normalize();

                    VxVector va = obstaclemesh->GetFaceVertex(imp->m_TouchedFace, 0);

                    float d = DotProduct(pos - va, norm);
                    VxVector lpos = pos - norm * d;
                    // VxVector lpos = pos;// - norm*d;
                    obstacle->Transform(&(imp->m_ImpactPoint), &lpos);
                }
            }
        }

        ////////////////////////////////////////////////////////////
        // IMPACT NORMAL
        ///////////////////////////////////////////////////////////

        if (ImpactFlags & IMPACTNORMAL)
        {
            if (imp->m_TouchedFace >= 0)
            {
                VxVector norm = obstaclemesh->GetFaceNormal(imp->m_TouchedFace);
                // we transform the normal in the world referential
                obstacle->TransformVector(&imp->m_ImpactNormal, &norm);
                imp->m_ImpactNormal.Normalize();
            }
        }
    }

    ////////////////////////////////////////////////////////////
    // IMPACT WORLD MATRIX
    ///////////////////////////////////////////////////////////

    if (ImpactFlags & IMPACTWORLDMATRIX)
    {
        memcpy(imp->m_ImpactWorldMatrix, replacementMatrix, sizeof(VxMatrix));
    }
}

/*************************************************
Name: ObstacleBetween

Summary: Check if an entity, declared as an obstacle, is between two other objects, declared as obstacles too.

Arguments:
    pos: first position
    endpos: second position
    width : width of the bean traced, added to the right and to the left of the base ray
    height : height of the beam traced, added to the top and to the bottom of the base ray

Return Value:
    TRUE if an obstacle is found in between, FALSE otherwise

Remarks:
{Group:Manager Detection}

See also: CollisionManager, CollisionManager::AddObstacle
*************************************************/
CKBOOL CollisionManager::ObstacleBetween(const VxVector &pos, const VxVector &endpos, float width, float height)
{
    StartProfile();

    m_BoundariesManager.UpdateEndMovings();

    // we calculate the positions extrema
    VxBbox box;
    int i;
    for (i = 0; i < 3; i++)
    {
        if (pos.v[i] < endpos.v[i])
        {
            box.Min.v[i] = pos.v[i];
            box.Max.v[i] = endpos.v[i];
        }
        else
        {
            box.Min.v[i] = endpos.v[i];
            box.Max.v[i] = pos.v[i];
        }
    }

    box.Min.x -= width * 0.5f;
    box.Min.y -= height;
    box.Min.z -= width * 0.5f;
    box.Max.x += width * 0.5f;
    box.Max.y += height;
    box.Max.z += width * 0.5f;

    // we first find all the potentially overlapped objects
    const XArray<CKObject *> &collarray = m_BoundariesManager.FindOverlapping(box);

    CK3dEntity *obstacle;

    VxBbox extbox;
    VxVector tpos, tendpos;

    int count = collarray.Size();
    VxVector up(0, 1, 0);
    VxMatrix mat;
    VxMatrix Mat1;
    Vx3DMatrixIdentity(mat);

    VxVector dir = endpos - pos;
    dir.y = 0;
    // we now have to generate the box to test
    VxVector newup;
    VxVector right;
    right = Normalize(CrossProduct(up, dir));
    newup = Normalize(CrossProduct(dir, right));
    float z = Magnitude(dir);
    dir /= z;

    // we create the matrix of the extent box
    memcpy(&mat[0][0], &right, sizeof(VxVector));
    memcpy(&mat[1][0], &up, sizeof(VxVector));
    memcpy(&mat[2][0], &dir, sizeof(VxVector));
    memcpy(&mat[3][0], &pos, sizeof(VxVector));

    extbox.Min.Set(-width, -height, 0);
    extbox.Max.Set(width, height, z);

    VxOBB obb(extbox, mat);

    // we now check each object
    for (i = 0; i < count; i++)
    {
        // first a ray-bounding box test
        obstacle = (CK3dEntity *)collarray[i];

        // we get the box
        box = obstacle->GetBoundingBox(TRUE);
        Mat1 = obstacle->GetWorldMatrix();

        if (VxIntersect::OBBOBB(obb, VxOBB(box, Mat1)))
        { // it intersect the local box
            break;
        }
    }

    StopProfile();

    return (i < count);
}

struct ZObject
{
    CK3dEntity *entity;
    float zmin;

    int operator<(const ZObject &iZ) const
    {
        return zmin < iZ.zmin;
    }
};

CK3dEntity *CollisionManager::ObstacleBetween(const VxVector &pos, const VxVector &endpos, CKBOOL iFace, CKBOOL iFirstContact, CKBOOL iIgnoreAlpha, VxIntersectionDesc *oDesc)
{
    StartProfile();

    m_BoundariesManager.UpdateEndMovings();

    // we calculate the positions extrema
    VxBbox box;
    box.Merge(pos);
    box.Merge(endpos);

    // we first find all the potentially overlapped objects
    const XArray<CKObject *> &collarray = m_BoundariesManager.FindOverlapping(box);

    VxRay ray(pos, endpos);
    CK3dEntity *obstacle = NULL;

    // Ray Intersection Options
    CK_RAYINTERSECTION options = CKRAYINTERSECTION_SEGMENT;
    if (iIgnoreAlpha)
        options = CK_RAYINTERSECTION(options | CKRAYINTERSECTION_IGNOREALPHA);
    if (iFirstContact)
        options = CK_RAYINTERSECTION(options | CKRAYINTERSECTION_FIRSTCONTACT);

    // either exit on the first
    if (iFirstContact)
    {
        // we now check each object
        int count = collarray.Size();
        for (int i = 0; i < count; ++i)
        {

            // first a ray-bounding box test
            CK3dEntity *ent = (CK3dEntity *)collarray[i];
            if (!ent)
                continue;

            if (iFace)
            { // face Level
                // Segment - World Box
                if (!VxIntersect::SegmentBox(ray, ent->GetBoundingBox())) // the segment does not intersect the world box
                    continue;

                // Segment - Face
                if (ent->RayIntersection(&pos, &endpos, oDesc, NULL, options))
                { // real face intersection

                    obstacle = ent;
                    break;
                }
            }
            else
            { // OBB Level
                VxRay localRay;
                ray.Transform(localRay, ent->GetInverseWorldMatrix());

                // Segment - OBB
                if (VxIntersect::SegmentBox(localRay, ent->GetBoundingBox(TRUE))) // the segment does not intersect the world box
                    break;
            }
        }
    }
    else
    {
        // or at the nearest

        float minDistance = 1e30f;
        VxIntersectionDesc tempDesc;
        VxVector boxIntersection;

        VxPlane distPlane(Normalize(ray.GetDirection()), ray.GetOrigin());

        // first sort the entities by Zdist to the ray origin

        XArray<ZObject> zobjects;

        ZObject dummy;

        // we insert each object in a sorted list of z ditance to the ray origin
        int count = collarray.Size();
        for (int i = 0; i < count; ++i)
        {

            // first a ray-bounding box test
            CK3dEntity *ent = (CK3dEntity *)collarray[i];
            if (!ent)
                continue;

            dummy.entity = ent;
            dummy.zmin = distPlane.Classify(ent->GetBoundingBox(TRUE), ent->GetWorldMatrix());

            zobjects.InsertSorted(dummy);
        }

        // we now check each object
        for (ZObject *it = zobjects.Begin(); it != zobjects.End(); ++it)
        {

            // first a ray-bounding box test
            CK3dEntity *ent = (CK3dEntity *)it->entity;
            if (!ent)
                continue;

            // we now have to test if the next object is farther than the already found intersection
            if (obstacle)
            {
                if (it->zmin > minDistance)
                    break;
            }

            if (iFace)
            { // face Level

                const VxBbox &entBox = ent->GetBoundingBox();

                if (!entBox.IsBoxInside(box))
                { // The ray is not inside the object

                    // Segment - World Box
                    if (!VxIntersect::SegmentBox(ray, entBox, boxIntersection)) // the segment does not intersect the world box
                        continue;

                    if (obstacle)
                    { // we look for the nearest

                        if (SquareMagnitude(boxIntersection - ray.GetOrigin()) > minDistance * minDistance)
                            continue; // we discard an object that cannot have intersection nearer than the min one
                    }
                }

                // Segment - Face
                if (ent->RayIntersection(&pos, &endpos, &tempDesc, NULL, options))
                { // real face intersection

                    // we need to keep the nearest
                    if (tempDesc.Distance < minDistance)
                    {
                        minDistance = tempDesc.Distance;
                        obstacle = ent;

                        if (oDesc)
                            *oDesc = tempDesc;
                    }
                }
            }
            else
            { // OBB Level

                VxRay localRay;

                ray.Transform(localRay, ent->GetInverseWorldMatrix());

                // Segment - OBB
                if (VxIntersect::SegmentBox(localRay, ent->GetBoundingBox(TRUE)))
                { // the segment does not intersect the world box
                    obstacle = ent;
                    break; // For now, no nearest at the Bbox level
                }
            }
        }
    }

    StopProfile();

    return obstacle;
}
