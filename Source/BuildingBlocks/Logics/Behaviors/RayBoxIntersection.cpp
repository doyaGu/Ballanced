/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Ray Box Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRayBoxIntersectionDecl();
CKERROR CreateRayBoxIntersectionProto(CKBehaviorPrototype **);
int RayBoxIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRayBoxIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Ray Box Intersection");
    od->SetDescription("Returns the nearest object intersected by the ray, the position of the intersection point and the normal at this point.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated when the ray intersects an object.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated when the ray doesn't intersect any object.<BR>
    <SPAN CLASS=out>Origin Inside: </SPAN>is activated when the ray origin is inside a bounding box of an object.<BR>
    <BR>
    <SPAN CLASS=pin>Ray Origin: </SPAN>the starting point of the ray.<BR>
    <SPAN CLASS=pin>Ray Direction: </SPAN>the direction vector of the ray.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>the referential in which all the above coordinates are expressed.<BR>
    <SPAN CLASS=pin>Depth: </SPAN>Depth of the intersection detection area (ie: the distance where the ray stops).<BR>
    <SPAN CLASS=pin>Group : </SPAN>group of the parsed objects.<BR>
    <BR>
    <SPAN CLASS=pout>Object Intersected: </SPAN>The nearest object intersected by the ray.<BR>
    <SPAN CLASS=pout>Intersection Point: </SPAN>coordinates of the intersection point.<BR>
    <SPAN CLASS=pout>Intersection Normal: </SPAN>coordinates of the normal vector of the intersection point.<BR>
    <SPAN CLASS=pout>Distance: </SPAN>distance between the origin of the ray and the intersection point.
    */
    /* warning:
    - The object to which the building block applies,  can not be returned.<BR>
    */
    od->SetCategory("Logics/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x671e5a87, 0x383b2372));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRayBoxIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRayBoxIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Ray Box Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");
    proto->DeclareOutput("Origin Inside");

    proto->DeclareInParameter("Ray Origin", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Ray Direction", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Depth", CKPGUID_FLOAT, "100");
    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->DeclareOutParameter("Object intersected", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Intersection Point", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Intersection Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Distance", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RayBoxIntersection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RayBoxIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CK3dObject *owner = (CK3dObject *)beh->GetTarget();
    if (!owner)
        return CKBR_OWNERERROR;

    // Set Input State
    beh->ActivateInput(0, FALSE);

    // Get the ray origin
    VxVector Origin(0.0f), RayBegin(0.0f);
    beh->GetInputParameterValue(0, &Origin);

    // Get the ray direction
    VxVector Direction(0.0f);
    beh->GetInputParameterValue(1, &Direction);

    VxRay ray(Origin, Direction, NULL);
    VxRay localray;

    // Get the ray referential
    CK3dEntity *referential = (CK3dEntity *)beh->GetInputParameterObject(2);

    // Transformation of the ray by the referential
    if (referential)
    {
        ray.Transform(ray, referential->GetWorldMatrix());
    }

    // Get depth of detection
    float Depth = 100.0f;
    beh->GetInputParameterValue(3, &Depth);

    // Compute the intersection
    VxVector NearestIntPoint, NearestIntNormal;

    float MinimumDist = 100000.0f;

    VxVector IntersectionPoint, IntersectionNormal;

    CKScene *scene = ctx->GetCurrentScene();
    CK3dEntity *NearestEntity = NULL;

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(4);
    if (!group)
        return CK_OK;

    // If the group contains anything else that is not a 3D Entity, we exit...
    if (!CKIsChildClassOf(group->GetCommonClassID(), CKCID_3DENTITY))
        return CK_OK;

    for (int i = 0; i < group->GetObjectCount(); ++i) // For all 3DOBJECTS
    {
        CK3dEntity *ent = (CK3dEntity *)group->GetObject(i);
        //________________________________/ Rejection if = Owner
        if (ent && (ent != owner))
        {
            //________________________________/ Rejection if not in current Scene
            if (ent->IsInScene(scene))
            {

                //______________________________/ Rejection by Bounding Cube (in world)
                const VxBbox &box = ent->GetBoundingBox(TRUE);

                // We transform the ray to be local of the entity
                ray.Transform(localray, ent->GetInverseWorldMatrix());

                CKBOOL b;
                if (b = VxIntersect::RayBox(localray, box, IntersectionPoint, NULL, &IntersectionNormal))
                {
                    // the origin is inside the box
                    if (b == -1)
                    {
                        beh->SetOutputParameterObject(0, ent);
                        beh->ActivateOutput(2, TRUE);
                        return CKBR_OK;
                    }
                    ent->Transform(&IntersectionPoint, &IntersectionPoint);
                    // We've found one....
                    float d = SquareMagnitude(IntersectionPoint - ray.m_Origin);
                    // OPTIM : prealculate depth*depth
                    if (d < MinimumDist)
                    {
                        MinimumDist = d;
                        NearestEntity = ent;
                        NearestIntPoint = IntersectionPoint;
                        NearestIntNormal = IntersectionNormal;
                    }
                }
            }
        }
    }

    if (NearestEntity && (MinimumDist < Depth * Depth))
    {
        // Set output parameters
        beh->SetOutputParameterValue(1, &NearestIntPoint);

        beh->SetOutputParameterObject(0, NearestEntity);
        NearestEntity->TransformVector(&NearestIntNormal, &NearestIntNormal);

        NearestIntNormal.Normalize();
        beh->SetOutputParameterValue(2, &NearestIntNormal);

        MinimumDist = sqrtf(MinimumDist);
        beh->SetOutputParameterValue(3, &MinimumDist);

        beh->ActivateOutput(0, TRUE);
        return CKBR_OK;
    }

    beh->ActivateOutput(1, TRUE);
    return CKBR_OK;
}
