/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Ray Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRayIntersectionDecl();
CKERROR CreateRayIntersectionProto(CKBehaviorPrototype **);
int RayIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRayIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Ray Intersection");
    od->SetDescription("Returns the nearest object intersected by the ray, the position of the intersection point and the normal at this point.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated when the ray intersects an object.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated when the ray doesn't intersect any object.<BR>
    <BR>
    <SPAN CLASS=pin>Ray Origin: </SPAN>the starting point of the ray.<BR>
    <SPAN CLASS=pin>Ray Direction: </SPAN>the direction vector of the ray.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>the referential in which all the above coordinates are expressed.<BR>
    <SPAN CLASS=pin>Depth: </SPAN>depth of the intersection detection area (ie: the distance where the ray stops).<BR>
    <SPAN CLASS=pin>Group : </SPAN>group of the parsed objects. If no group is specified, it will parse all the objects of the scene.<BR>
    <BR>
    <SPAN CLASS=pout>Object Intersected: </SPAN>the nearest object intersected by the ray.<BR>
    <SPAN CLASS=pout>Face Index: </SPAN>index of the face intersected.<BR>
    <SPAN CLASS=pout>Nearest Vertex Index: </SPAN>index of the nearest vertex on the face intersected.<BR>
    <SPAN CLASS=pout>Intersection Point: </SPAN>coordinates of the intersection point.<BR>
    <SPAN CLASS=pout>Intersection Normal: </SPAN>coordinates of the normal vector of the intersection point.<BR>
    <SPAN CLASS=pout>Distance: </SPAN>distance between the origin of the ray and the intersection point.<BR>
    <BR>
    <SPAN CLASS=setting>Output Param Update: </SPAN>uncheck this if you're only interested by the intersected object. The other output parameter values won't be updated (it saves process time).<BR>
    <SPAN CLASS=setting>Skip Owner: </SPAN>check this if you want the object on which the "Ray Intersection" BB applies not to be parsed in the collision test.<BR>
    */
    od->SetCategory("Logics/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x671e4a87, 0x383b2912));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRayIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRayIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Ray Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Ray Origin", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Ray Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Depth", CKPGUID_FLOAT, "100");
    proto->DeclareInParameter("Filter", CKPGUID_GROUP);

    proto->DeclareSetting("Output Param Update", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Skip Owner", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Skip Transparent", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("Object Intersected", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Face Index", CKPGUID_INT);
    proto->DeclareOutParameter("Nearest Vertex Index", CKPGUID_INT);
    proto->DeclareOutParameter("Intersection Point", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Intersection Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Distance", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Body Part", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RayIntersection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

struct IntersectionDesc
{
    // The Ray Itself
    VxRay m_Ray;
    VxVector m_RayEnd;
    float m_Depth;

    VxIntersectionDesc m_MinDesc;
    CK3dEntity *m_MinEntity;
    float m_MinDistance2; // square magnitude temporary distance to help us know which will be the nearest intersected object
};

void Intersect(CK3dEntity *CurrentEntity, IntersectionDesc *idesc, CKScene *scene, CKBOOL iSkipTransparent)
{
    // not a 3D Entity
    if (!CKIsChildClassOf(CurrentEntity, CKCID_3DENTITY))
        return;
    CKBOOL character = FALSE;
    if (CKIsChildClassOf(CurrentEntity, CKCID_CHARACTER))
        character = TRUE;

    //________________________________/ Rejection if not in current Scene
    if (CurrentEntity->IsInScene(scene))
    {

        //________________________________/ Rejection if not visible
        if (!iSkipTransparent || CurrentEntity->IsVisible())
        {

            if (CKIsChildClassOf(CurrentEntity, CKCID_SPRITE3D))
            {
                // Ray Inter
                VxIntersectionDesc Inter;
                VxVector IntersectionPoint;
                if (CurrentEntity->RayIntersection(&idesc->m_Ray.m_Origin, &idesc->m_RayEnd, &Inter, NULL))
                {
                    CurrentEntity->Transform(&IntersectionPoint, &Inter.IntersectionPoint);
                    float Dist = SquareMagnitude(IntersectionPoint - idesc->m_Ray.m_Origin);
                    if (Dist < idesc->m_MinDistance2)
                    {
                        idesc->m_MinDistance2 = Dist;
                        idesc->m_MinEntity = CurrentEntity;
                        idesc->m_MinDesc = Inter;
                    }
                }

                return; // stop there
            }

            //________________________________/ Rejection by Bounding Sphere / Depth
            float radius = CurrentEntity->GetRadius();
            VxVector pos;
            CurrentEntity->GetBaryCenter(&pos);
            VxVector dif = pos - idesc->m_Ray.m_Origin;
            float Dist = SquareMagnitude(dif);

            if (Dist < (idesc->m_Depth + radius) * (idesc->m_Depth + radius))
            {

                //______________________________/ Rejection by Sphere / Behind
                float s = DotProduct(dif, idesc->m_Ray.m_Direction);
                if (s + radius > 0.0f)
                {

                    //______________________________/ Rejection by Bounding Sphere / Segment
                    if (radius * radius > Dist - s * s)
                    {

                        //______________________________/ Rejection by Bounding Cube (in world)
                        const VxBbox &box = CurrentEntity->GetBoundingBox();
                        VxVector IntersectionPoint;
                        if (VxIntersect::RayBox(idesc->m_Ray, box))
                        {
                            if (character)
                            {
                                int count = ((CKCharacter *)CurrentEntity)->GetBodyPartCount();
                                while (count)
                                {
                                    CKBodyPart *bp;
                                    if (bp = ((CKCharacter *)CurrentEntity)->GetBodyPart(--count))
                                    {
                                        //______________________________/ Reject BodyPart by Bounding Cube (in world)
                                        if (VxIntersect::RayBox(idesc->m_Ray, bp->GetBoundingBox()))
                                        {
                                            // Ray Inter
                                            VxIntersectionDesc Inter;
                                            if (bp->RayIntersection(&idesc->m_Ray.m_Origin, &idesc->m_RayEnd, &Inter, NULL))
                                            {
                                                bp->Transform(&IntersectionPoint, &Inter.IntersectionPoint);
                                                Dist = SquareMagnitude(IntersectionPoint - idesc->m_Ray.m_Origin);
                                                if (Dist < idesc->m_MinDistance2)
                                                {
                                                    idesc->m_MinDistance2 = Dist;
                                                    idesc->m_MinEntity = CurrentEntity;
                                                    idesc->m_MinDesc = Inter;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                // Ray Inter
                                VxIntersectionDesc Inter;
                                if (CurrentEntity->RayIntersection(&idesc->m_Ray.m_Origin, &idesc->m_RayEnd, &Inter, NULL))
                                {
                                    CurrentEntity->Transform(&IntersectionPoint, &Inter.IntersectionPoint);
                                    Dist = SquareMagnitude(IntersectionPoint - idesc->m_Ray.m_Origin);
                                    if (Dist < idesc->m_MinDistance2)
                                    {
                                        idesc->m_MinDistance2 = Dist;
                                        idesc->m_MinEntity = CurrentEntity;
                                        idesc->m_MinDesc = Inter;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int RayIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dObject *owner = (CK3dObject *)beh->GetTarget();
    if (!owner)
        return CKBR_OWNERERROR;

    // Set Input State
    beh->ActivateInput(0, FALSE);

    ////////////////////////////////////////
    // Input Members
    ///////////////////////////////////////

    // Do we want to exclude the owner from the tests ?
    CKBOOL skip = TRUE;
    beh->GetLocalParameterValue(1, &skip);
    if (!skip)
        owner = NULL;

    CKBOOL skipTransparent = FALSE;
    beh->GetLocalParameterValue(2, &skipTransparent);

    // Get the ray origin
    VxVector Origin(0.0f);
    beh->GetInputParameterValue(0, &Origin);

    // Get the ray direction
    VxVector Direction(0.0f);
    beh->GetInputParameterValue(1, &Direction);

    // Get the ray referential
    CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(2);

    if (referential)
    {
        referential->Transform(&Origin, &Origin);
        referential->TransformVector(&Direction, &Direction);
    }

    Direction.Normalize();

    // Get depth of detection
    float Depth = 100.0f;
    beh->GetInputParameterValue(3, &Depth);

    // Get Filter Group
    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(4);

    /////////////////////////////////
    // IntersectionDesc Structure
    /////////////////////////////////

    IntersectionDesc idesc;
    // Ray Members
    idesc.m_Ray.m_Origin = Origin;
    idesc.m_Ray.m_Direction = Direction;
    idesc.m_RayEnd = Origin + Direction;
    idesc.m_Depth = Depth;

    // Nearest Members
    idesc.m_MinDistance2 = Depth * Depth;
    idesc.m_MinEntity = NULL;

    CKScene *scene = ctx->GetCurrentScene();

    if (group)
    { // A filter group was provided
        int count = group->GetObjectCount();
        for (int i = 0; i < count; ++i)
        {
            CK3dObject *CurrentEntity = (CK3dObject *)group->GetObject(i);

            //________________________________/ Rejection if = Owner
            if (CurrentEntity != owner)
                Intersect(CurrentEntity, &idesc, scene, skipTransparent);
        }
    }
    else
    { // No filter group : we take them all
        int count = ctx->GetObjectsCountByClassID(CKCID_3DOBJECT);
        CK_ID *id = ctx->GetObjectsListByClassID(CKCID_3DOBJECT);
        int i;
        for (i = 0; i < count; ++i)
        { // For all 3DOBJECTS
            CK3dObject *CurrentEntity = (CK3dObject *)ctx->GetObject(id[i]);

            //________________________________/ Rejection if = Owner
            if (CurrentEntity != owner)
                Intersect(CurrentEntity, &idesc, scene, skipTransparent);
        }

        count = ctx->GetObjectsCountByClassID(CKCID_CHARACTER);
        id = ctx->GetObjectsListByClassID(CKCID_CHARACTER);
        for (i = 0; i < count; ++i)
        { // For all CHARACTERS
            CK3dObject *CurrentEntity = (CK3dObject *)ctx->GetObject(id[i]);

            //________________________________/ Rejection if = Owner
            if (CurrentEntity != owner)
                Intersect(CurrentEntity, &idesc, scene, skipTransparent);
        }

        count = ctx->GetObjectsCountByClassID(CKCID_SPRITE3D);
        id = ctx->GetObjectsListByClassID(CKCID_SPRITE3D);
        for (i = 0; i < count; ++i)
        { // For all Sprite 3D
            CK3dEntity *CurrentEntity = (CK3dEntity *)ctx->GetObject(id[i]);

            //________________________________/ Rejection if = Owner
            if (CurrentEntity != owner)
                Intersect(CurrentEntity, &idesc, scene, skipTransparent);
        }
    }

    if (idesc.m_MinEntity && (idesc.m_MinDistance2 < Depth * Depth))
    {
        // update intersected object
        beh->SetOutputParameterObject(0, idesc.m_MinEntity);

        CKBOOL update = TRUE;
        beh->GetLocalParameterValue(0, &update);

        if (update)
        {
            CK3dEntity *ent = idesc.m_MinEntity;

            if (beh->GetOutputParameterCount() == 7)
            { // check if there is the bodypart output param
                if (CKIsChildClassOf(idesc.m_MinEntity, CKCID_CHARACTER))
                {
                    ent = (CK3dEntity *)(idesc.m_MinDesc.Object);
                    beh->SetOutputParameterObject(6, ent);
                }
                else
                {
                    beh->SetOutputParameterObject(6, NULL);
                }
            }

            // find the nearest vertex
            CKMesh *mesh = ent->GetCurrentMesh();
            ;
            if (mesh)
            {
                int i1, i2, i3;
                mesh->GetFaceVertexIndex(idesc.m_MinDesc.FaceIndex, i1, i2, i3);

                VxVector V1, V2, V3;
                mesh->GetVertexPosition(i1, &V1);
                mesh->GetVertexPosition(i2, &V2);
                mesh->GetVertexPosition(i3, &V3);

                float d1, d2, d3;
                d1 = Magnitude(V1 - idesc.m_MinDesc.IntersectionPoint);
                d2 = Magnitude(V2 - idesc.m_MinDesc.IntersectionPoint);
                d3 = Magnitude(V3 - idesc.m_MinDesc.IntersectionPoint);

                int NearestVertexIndex = d1 < d2 ? (d1 < d3 ? i1 : i3) : (d2 < d3 ? i2 : i3);

                // Set output parameters
                beh->SetOutputParameterValue(2, &NearestVertexIndex);
            }

            // Compute the intersection point and intersection normal in global coordinates
            VxVector IntersectionPoint;
            ent->Transform(&IntersectionPoint, &idesc.m_MinDesc.IntersectionPoint);
            VxVector IntersectionNormal;
            ent->TransformVector(&IntersectionNormal, &idesc.m_MinDesc.IntersectionNormal);
            IntersectionNormal.Normalize();

            // Set output parameters
            beh->SetOutputParameterValue(1, &idesc.m_MinDesc.FaceIndex);
            beh->SetOutputParameterValue(3, &IntersectionPoint);
            beh->SetOutputParameterValue(4, &IntersectionNormal);
            idesc.m_MinDistance2 = sqrtf(idesc.m_MinDistance2);
            beh->SetOutputParameterValue(5, &idesc.m_MinDistance2);
        }
        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    beh->ActivateOutput(1, TRUE);
    return CKBR_OK;
}
