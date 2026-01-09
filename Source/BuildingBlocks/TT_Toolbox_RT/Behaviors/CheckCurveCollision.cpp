//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT CheckCurveCollision
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorCheckCurveCollisionDecl();
CKERROR CreateCheckCurveCollisionProto(CKBehaviorPrototype **pproto);
int CheckCurveCollision(const CKBehaviorContext &behcontext);
CKERROR CheckCurveCollisionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCheckCurveCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT CheckCurveCollision");
    od->SetDescription("Switches the sequential stream to the corresponding ouput, following the result of comparison between 'Test' and the others input parameters");
    od->SetCategory("TT Toolbox/Collision");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e9a4415, 0x199e4eb3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCheckCurveCollisionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCheckCurveCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT CheckCurveCollision");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Check");

    proto->DeclareOutput("TRUE");
    proto->DeclareOutput("FALSE");

    proto->DeclareInParameter("Curve Group", CKPGUID_GROUP);
    proto->DeclareInParameter("HierachieBB", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("MinDistance", CKPGUID_FLOAT);

    proto->DeclareSetting("MinDistance", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CheckCurveCollision);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetBehaviorCallbackFct(CheckCurveCollisionCallBack);

    *pproto = proto;
    return CK_OK;
}

// Helper function to check line-box intersection
static CKBOOL LineBoxIntersect(const VxVector &lineStart, const VxVector &lineDir,
                               const VxVector &boxMin, const VxVector &boxMax)
{
    float halfSizeX = (boxMax.x + boxMin.x) != 0.0f ? (boxMax.x + boxMin.x) * 0.5f : 0.0f;
    float halfSizeY = (boxMax.y + boxMin.y) != 0.0f ? (boxMax.y + boxMin.y) * 0.5f : 0.0f;
    float halfSizeZ = (boxMax.z + boxMin.z) != 0.0f ? (boxMax.z + boxMin.z) * 0.5f : 0.0f;

    float centerX = boxMin.x - halfSizeX;
    float centerY = boxMin.y - halfSizeY;
    float centerZ = boxMin.z - halfSizeZ;

    float relX = lineStart.x - halfSizeX;
    float relY = lineStart.y - halfSizeY;
    float relZ = lineStart.z - halfSizeZ;

    float halfDirX = lineDir.x * 0.5f;
    float halfDirY = lineDir.y * 0.5f;
    float halfDirZ = lineDir.z * 0.5f;

    float testX = halfDirX + relX;
    float testY = halfDirY + relY;
    float testZ = halfDirZ + relZ;

    if (fabsf(halfDirX) + centerX < fabsf(testX)) return FALSE;
    if (fabsf(halfDirY) + centerY < fabsf(testY)) return FALSE;
    if (fabsf(halfDirZ) + centerZ < fabsf(testZ)) return FALSE;

    float absY = fabsf(lineDir.y);
    float absZ = fabsf(lineDir.z);
    if (absY * centerZ + absZ * centerY < fabsf(lineDir.z * testY - testZ * lineDir.y)) return FALSE;

    float absX = fabsf(lineDir.x);
    if (absX * centerZ + absZ * centerX < fabsf(testZ * lineDir.x - lineDir.z * testX)) return FALSE;
    if (absX * centerY + absY * centerX < fabsf(lineDir.y * testX - testY * lineDir.x)) return FALSE;

    return TRUE;
}

// Helper function to check collision between entity and curve group
static CKBOOL CheckEntityCurveCollision(CK3dEntity *entity, CKGroup *curveGroup, CKBOOL hierachie, float *minDistance)
{
    CKBOOL checkDistance = (*minDistance != -1.0f);
    VxBbox bbox;
    bbox.Min.Set(-1000000.0f, -1000000.0f, -1000000.0f);
    bbox.Max.Set(1000000.0f, 1000000.0f, 1000000.0f);

    VxVector worldPos, entityScale, entityRot;
    VxMatrix worldMatrix;

    if (hierachie)
    {
        // Store original transforms
        entity->GetPosition(&worldPos, NULL);
        entity->GetScale(&entityScale, FALSE);
        
        // Get hierarchical bounding box
        const VxBbox &hierBox = entity->GetHierarchicalBox(TRUE);
        bbox = hierBox;
        
        // Restore original scale
        entity->SetScale(&entityScale, FALSE, FALSE);
    }
    else
    {
        // Get local bounding box
        const VxBbox &localBox = entity->GetBoundingBox(TRUE);
        bbox = localBox;
    }

    // Get entity world position
    entity->GetPosition(&worldPos, NULL);
    *minDistance = 1.0e12f;

    int objectCount = curveGroup->GetObjectCount();
    if (objectCount <= 0)
        return FALSE;

    for (int i = 0; i < objectCount; i++)
    {
        CKCurve *curve = (CKCurve *)curveGroup->GetObject(i);
        int pointCount = curve->GetControlPointCount() - 1;

        if (pointCount <= 0)
            continue;

        for (int j = 0; j < pointCount; j++)
        {
            CKCurvePoint *point1 = curve->GetControlPoint(j);
            CKCurvePoint *point2 = curve->GetControlPoint(j + 1);

            VxVector pos1, pos2;
            point1->GetPosition(&pos1, entity);
            point2->GetPosition(&pos2, entity);

            if (checkDistance)
            {
                VxVector worldPos1, worldPos2;
                point1->GetPosition(&worldPos1, NULL);
                point2->GetPosition(&worldPos2, NULL);

                VxVector diff = worldPos2 - worldPos1;
                float dist1 = sqrtf((worldPos.x - worldPos2.x) * (worldPos.x - worldPos2.x) +
                                   (worldPos.y - worldPos2.y) * (worldPos.y - worldPos2.y) +
                                   (worldPos.z - worldPos2.z) * (worldPos.z - worldPos2.z));
                float dist2 = sqrtf((worldPos.x - worldPos1.x) * (worldPos.x - worldPos1.x) +
                                   (worldPos.y - worldPos1.y) * (worldPos.y - worldPos1.y) +
                                   (worldPos.z - worldPos1.z) * (worldPos.z - worldPos1.z));
                float segLen = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
                float distSum = dist1 + dist2 - segLen;
                if (distSum < *minDistance)
                    *minDistance = distSum;
            }

            VxVector lineDir = pos2 - pos1;
            if (LineBoxIntersect(pos1, lineDir, bbox.Min, bbox.Max))
                return TRUE;
        }
    }

    return FALSE;
}

int CheckCurveCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CKGroup *curveGroup = (CKGroup *)beh->GetInputParameterObject(0);
    if (!curveGroup)
        return CKBR_PARAMETERERROR;

    CKBOOL hierachie = FALSE;
    beh->GetInputParameterValue(1, &hierachie);

    CKBOOL showDistance = FALSE;
    beh->GetLocalParameterValue(0, &showDistance);

    float minDistance = showDistance ? 0.0f : -1.0f;

    if (CheckEntityCurveCollision(target, curveGroup, hierachie, &minDistance))
    {
        beh->ActivateOutput(0, TRUE);
        beh->ActivateOutput(1, FALSE);
    }
    else
    {
        beh->ActivateOutput(0, FALSE);
        beh->ActivateOutput(1, TRUE);
    }

    if (showDistance)
        beh->SetOutputParameterValue(0, &minDistance);

    beh->ActivateInput(0, FALSE);
    return CKBR_OK;
}

CKERROR CheckCurveCollisionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL showDistance = FALSE;
    beh->GetLocalParameterValue(0, &showDistance);

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORSETTINGSEDITED:
        if (showDistance)
            beh->EnableOutputParameter(0, TRUE);
        else
            beh->EnableOutputParameter(0, FALSE);
        break;
    }

    return CKBR_OK;
}