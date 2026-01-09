//////////////////////////////////////////////
//////////////////////////////////////////////
//
//        TT GetNearestCurvePosition
//
//////////////////////////////////////////////
//////////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetNearestCurvePositionDecl();
CKERROR CreateGetNearestCurvePositionProto(CKBehaviorPrototype **pproto);
int GetNearestCurvePosition(const CKBehaviorContext &behcontext);
CKERROR GetNearestCurvePositionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetNearestCurvePositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetNearestCurvePosition");
    od->SetDescription("Get Nearest Curve Position");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4db6e14, 0x2c151f73));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetNearestCurvePositionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetNearestCurvePositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetNearestCurvePosition");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Curve", CKPGUID_CURVE);
    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0.0,0.0,0.0");

    proto->DeclareOutParameter("Position", CKPGUID_VECTOR, "0.0,0.0,0.0");
    proto->DeclareOutParameter("Step", CKPGUID_FLOAT, "0.0");

    proto->DeclareSetting("Prefered Height Delta", CKPGUID_FLOAT, "2.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetNearestCurvePosition);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(GetNearestCurvePositionCallBack);

    *pproto = proto;
    return CK_OK;
}

// Helper function to find closest point on a line segment to a query point
// Returns TRUE if a perpendicular projection exists on the segment
static CKBOOL ClosestPointOnSegment(VxVector *result,
                                    const VxVector &p1, const VxVector &p2,
                                    const VxVector &query)
{
    VxVector dir;
    dir.x = p2.x - p1.x;
    dir.y = p2.y - p1.y;
    dir.z = p2.z - p1.z;

    // Compute 2D perpendicular direction (in XZ plane)
    float perpX = -dir.z;
    float perpZ = dir.x;
    float perpLen = sqrtf(perpX * perpX + perpZ * perpZ);
    if (perpLen != 0.0f)
    {
        float invLen = 1.0f / perpLen;
        perpX *= invLen;
        perpZ *= invLen;
    }

    // Calculate relative position
    float relX = query.x - p1.x;
    float relZ = query.z - p1.z;

    // Calculate intersection parameter
    float denom = perpX * dir.z - perpZ * dir.x;
    float t = ((query.z - p1.z) * perpX - relX * perpZ) / denom;

    // Check if projection is within segment
    if (t > 1.0f || t < 0.0f)
        return FALSE;

    // Compute closest point
    float projX = perpX * (perpX * query.x + perpZ * query.z - perpZ * p1.z - perpX * p1.x) + query.x;
    float projZ = perpZ * (perpX * query.x + perpZ * query.z - perpZ * p1.z - perpX * p1.x) + query.z;

    result->x = perpX * ((query.z - p1.z) * dir.x - relX * dir.z) / denom + query.x;
    result->z = perpZ * ((query.z - p1.z) * dir.x - relX * dir.z) / denom + query.z;

    if (dir.x == 0.0f)
    {
        result->y = ((result->z - p1.z) / dir.z) * dir.y + p1.y;
    }
    else
    {
        result->y = ((result->x - p1.x) / dir.x) * dir.y + p1.y;
    }

    return TRUE;
}

int GetNearestCurvePosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);

    CKCurve *curve = (CKCurve *)beh->GetInputParameterObject(0);
    if (!curve)
    {
        ctx->OutputToConsole("TT_GetNearestCurvePosition : No Valid Curve");
        return CKBR_OK;
    }

    VxVector queryPos;
    beh->GetInputParameterValue(1, &queryPos);

    float preferedHeightDelta;
    beh->GetLocalParameterValue(0, &preferedHeightDelta);

    int pointCount = curve->GetControlPointCount();
    float minDist3D = 1e9f;
    float minDist2D = 1e9f;
    CKBOOL found2D = FALSE;

    VxVector nearestPos3D, nearestStart3D;
    VxVector nearestPos2D, nearestStart2D;
    CKCurvePoint *nearestPoint3D = NULL;
    CKCurvePoint *nearestPoint2D = NULL;

    for (int i = 0; i < pointCount; i++)
    {
        CKCurvePoint *pt1 = curve->GetControlPoint(i);
        CKCurvePoint *pt2;

        if (i == pointCount - 1)
            pt2 = curve->GetControlPoint(0);
        else
            pt2 = curve->GetControlPoint(i + 1);

        VxVector pos1, pos2;
        pt1->GetPosition(&pos1, NULL);
        pt2->GetPosition(&pos2, NULL);

        VxVector closestPt;
        CKBOOL hasProjection = ClosestPointOnSegment(&closestPt, pos1, pos2, queryPos);

        // Calculate distances
        VxVector diff1 = queryPos - pos1;
        VxVector diff2 = queryPos - pos2;
        float dist2D_1 = diff1.x * diff1.x + diff1.z * diff1.z;
        float dist2D_2 = diff2.x * diff2.x + diff2.z * diff2.z;

        if (hasProjection)
        {
            VxVector diff = closestPt - queryPos;
            float heightDiff = closestPt.y - queryPos.y;
            float dist3D = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

            if (dist3D < minDist3D)
            {
                minDist3D = dist3D;
                nearestPos3D = closestPt;
                nearestStart3D = pos1;
                nearestPoint3D = pt1;
            }

            float dist2D = (closestPt.x - queryPos.x) * (closestPt.x - queryPos.x) +
                          (closestPt.z - queryPos.z) * (closestPt.z - queryPos.z);
            if (dist2D < minDist2D && fabsf(heightDiff) < preferedHeightDelta)
            {
                minDist2D = dist2D;
                nearestPos2D = closestPt;
                nearestStart2D = pos1;
                nearestPoint2D = pt1;
                found2D = TRUE;
            }
        }
        else
        {
            // Check endpoint distances
            float dist3D_1 = diff1.x * diff1.x + diff1.y * diff1.y + diff1.z * diff1.z;
            float dist3D_2 = diff2.x * diff2.x + diff2.y * diff2.y + diff2.z * diff2.z;
            float heightDiff1 = pos1.y - queryPos.y;
            float heightDiff2 = pos2.y - queryPos.y;

            if (dist3D_1 < minDist3D)
            {
                minDist3D = dist3D_1;
                nearestPos3D = pos1;
                nearestStart3D = pos1;
                nearestPoint3D = pt1;
            }
            if (dist3D_2 < minDist3D)
            {
                minDist3D = dist3D_2;
                nearestPos3D = pos2;
                nearestStart3D = pos2;
                nearestPoint3D = pt2;
            }

            if (dist2D_1 < minDist2D && fabsf(heightDiff1) < preferedHeightDelta)
            {
                minDist2D = dist2D_1;
                nearestPos2D = pos1;
                nearestStart2D = pos1;
                nearestPoint2D = pt1;
                found2D = TRUE;
            }
            if (dist2D_2 < minDist2D && fabsf(heightDiff2) < preferedHeightDelta)
            {
                minDist2D = dist2D_2;
                nearestPos2D = pos2;
                nearestStart2D = pos2;
                nearestPoint2D = pt2;
                found2D = TRUE;
            }
        }
    }

    // Use 2D nearest if found within height tolerance, else 3D nearest
    VxVector resultPos, startPos;
    CKCurvePoint *resultPoint;

    if (found2D)
    {
        resultPos = nearestPos2D;
        startPos = nearestStart2D;
        resultPoint = nearestPoint2D;
    }
    else
    {
        resultPos = nearestPos3D;
        startPos = nearestStart3D;
        resultPoint = nearestPoint3D;
    }

    // Calculate step (normalized position along curve)
    float curveLength = curve->GetLength();
    float pointLength = resultPoint->GetLength();
    VxVector diff = resultPos - startPos;
    float segmentDist = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
    float step = (pointLength + segmentDist) / curveLength;

    beh->SetOutputParameterValue(0, &resultPos);
    beh->SetOutputParameterValue(1, &step);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}

CKERROR GetNearestCurvePositionCallBack(const CKBehaviorContext &behcontext)
{
    // No callback logic needed
    return CKBR_OK;
}