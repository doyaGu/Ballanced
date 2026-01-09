/////////////////////////////////////////
/////////////////////////////////////////
//
//        TT Restore Dynamic IC
//
/////////////////////////////////////////
/////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorRestoreDynamicICDecl();
CKERROR CreateRestoreDynamicICProto(CKBehaviorPrototype **pproto);
int RestoreDynamicIC(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRestoreDynamicICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restore Dynamic IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x680269cb, 0x50814d03));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRestoreDynamicICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRestoreDynamicICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restore Dynamic IC");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RestoreDynamicIC);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

// Helper to check if an entity type is a 3D entity or derived
static CKBOOL Is3DEntityClass(CK3dEntity *entity)
{
    CK_CLASSID classId = entity->GetClassID();
    return (classId == CKCID_3DENTITY ||
            classId == CKCID_CAMERA ||
            classId == CKCID_TARGETCAMERA ||
            classId == CKCID_CURVEPOINT ||
            classId == CKCID_SPRITE3D ||
            classId == CKCID_LIGHT ||
            classId == CKCID_TARGETLIGHT ||
            classId == CKCID_CHARACTER ||
            classId == CKCID_3DOBJECT ||
            classId == CKCID_BODYPART ||
            classId == CKCID_CURVE);
}

// Helper to restore a single entity's IC
static void RestoreEntityIC(CK3dEntity *entity, CKScene *scene)
{
    if (!entity->IsInScene(scene))
        return;

    CKStateChunk *chunk = scene->GetObjectInitialValue(entity);
    if (!chunk)
        return;

    CKDWORD flags = entity->GetObjectFlags();
    CKBOOL wasDynamic = (flags & CK_OBJECT_DYNAMIC) && (flags & CK_OBJECT_HIERACHICALHIDE);

    if (wasDynamic)
    {
        // Clear dynamic flags temporarily
        entity->ModifyObjectFlags(0, CK_OBJECT_DYNAMIC | CK_OBJECT_HIERACHICALHIDE);

        // Preserve current mesh
        CKMesh *currentMesh = NULL;
        if (Is3DEntityClass(entity))
            currentMesh = entity->GetCurrentMesh();

        // Restore IC
        CKReadObjectState(entity, chunk);

        // Restore flags
        entity->ModifyObjectFlags(CK_OBJECT_DYNAMIC | CK_OBJECT_HIERACHICALHIDE, 0);

        // Restore mesh if it was changed
        if (currentMesh)
            entity->SetCurrentMesh(currentMesh, TRUE);
    }
    else
    {
        CKReadObjectState(entity, chunk);
    }
}

int RestoreDynamicIC(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();
    if (!entity)
        return CKBR_OWNERERROR;

    CKScene *scene = behcontext.CurrentScene;
    if (!scene)
        return CKBR_GENERICERROR;

    // Restore the main entity
    RestoreEntityIC(entity, scene);

    // Handle hierarchy if requested
    if (Is3DEntityClass(entity))
    {
        CKBOOL hierarchy = FALSE;
        beh->GetInputParameterValue(0, &hierarchy);

        if (hierarchy)
        {
            int childCount = entity->GetChildrenCount();
            CK3dEntity *child = NULL;
            while ((child = entity->HierarchyParser(child)) != NULL)
            {
                RestoreEntityIC(child, scene);
                child->SetParent(entity, TRUE);
            }
        }
    }

    return CKBR_OK;
}