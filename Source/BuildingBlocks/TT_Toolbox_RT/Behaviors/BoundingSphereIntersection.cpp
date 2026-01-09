///////////////////////////////////////////////////
///////////////////////////////////////////////////
//
//        TT Bounding Sphere Intersection
//
///////////////////////////////////////////////////
///////////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl();
CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **pproto);
int BoundingSphereIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Bounding Sphere Intersection");
    od->SetDescription("Detects the collision between two 3D entities at the bounding sphere level.");
    od->SetCategory("TT Toolbox/Collision");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32a6308d, 0x1d3d0931));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoundingSphereIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Bounding Sphere Intersection");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Entity 0", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Entity 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy 0", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Hierarchy 1", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Threshold", CKPGUID_FLOAT, "0.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BoundingSphereIntersection);

    *pproto = proto;
    return CK_OK;
}

// Helper function to get hierarchical bounding box center and radius
static void GetEntityBoundingSphere(CK3dEntity *entity, CKBOOL hierarchy, float threshold,
                                    VxVector &center, float &radiusSq)
{
    VxBbox bbox;
    if (hierarchy)
    {
        const VxBbox &hierBox = entity->GetHierarchicalBox(TRUE);
        bbox = hierBox;
    }
    else
    {
        const VxBbox &localBox = entity->GetBoundingBox(TRUE);
        bbox = localBox;
    }

    // Transform bbox corners to world space
    VxVector worldMin, worldMax;
    entity->Transform(&worldMin, &bbox.Min, NULL);
    entity->Transform(&worldMax, &bbox.Max, NULL);

    // Calculate diagonal length squared (bounding sphere diameter squared)
    VxVector diff = worldMax - worldMin;
    radiusSq = (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) * threshold;

    // Center of bounding sphere
    center.x = (worldMin.x + worldMax.x) * threshold;
    center.y = (worldMin.y + worldMax.y) * threshold;
    center.z = (worldMin.z + worldMax.z) * threshold;
}

int BoundingSphereIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CK3dEntity *entity0 = (CK3dEntity *)beh->GetInputParameterObject(0);
    CK3dEntity *entity1 = (CK3dEntity *)beh->GetInputParameterObject(1);

    CKBOOL hierarchy0 = FALSE;
    CKBOOL hierarchy1 = FALSE;
    float threshold = 0.5f;

    beh->GetInputParameterValue(2, &hierarchy0);
    beh->GetInputParameterValue(3, &hierarchy1);
    beh->GetInputParameterValue(4, &threshold);

    if (!entity0 || !entity1)
    {
        beh->ActivateOutput(1, TRUE);
        return CKBR_OK;
    }

    // Get bounding spheres for both entities
    VxVector center0, center1;
    float radiusSq0, radiusSq1;

    GetEntityBoundingSphere(entity0, hierarchy0, threshold, center0, radiusSq0);
    GetEntityBoundingSphere(entity1, hierarchy1, threshold, center1, radiusSq1);

    // Calculate distance between centers
    VxVector diff = center1 - center0;
    float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

    // Check intersection
    if (distSq <= radiusSq0 + radiusSq1)
        beh->ActivateOutput(0, TRUE); // Intersecting
    else
        beh->ActivateOutput(1, TRUE); // Not intersecting

    return CKBR_OK;
}