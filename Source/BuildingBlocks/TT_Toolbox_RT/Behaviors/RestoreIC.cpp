/////////////////////////////////
/////////////////////////////////
//
//        TT Restore IC
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorRestoreICDecl();
CKERROR CreateRestoreICProto(CKBehaviorPrototype **pproto);
int RestoreIC(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRestoreICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restore IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e0f2f58, 0x232d3109));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRestoreICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRestoreICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restore IC");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RestoreIC);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RestoreIC(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetTarget();
    if (!bo)
        return CKBR_OWNERERROR;

    CKScene *scn = behcontext.CurrentScene;
    if (!scn)
        return CKBR_GENERICERROR;

    //----- Restore the IC
    if (bo->IsInScene(scn))
    {
        CKStateChunk *chunk = scn->GetObjectInitialValue(bo);
        if (chunk)
        {
            CKReadObjectState(bo, chunk);
        }
    }

    if (bo->GetClassID() == CKCID_3DENTITY ||
        bo->GetClassID() == CKCID_CAMERA ||
        bo->GetClassID() == CKCID_TARGETCAMERA ||
        bo->GetClassID() == CKCID_CURVEPOINT ||
        bo->GetClassID() == CKCID_SPRITE3D ||
        bo->GetClassID() == CKCID_LIGHT ||
        bo->GetClassID() == CKCID_TARGETLIGHT ||
        bo->GetClassID() == CKCID_CHARACTER ||
        bo->GetClassID() == CKCID_3DOBJECT ||
        bo->GetClassID() == CKCID_BODYPART ||
        bo->GetClassID() == CKCID_CURVE)
    {
        CKBOOL hierarchy = FALSE;
        beh->GetInputParameterValue(0, &hierarchy);
        if (hierarchy)
        {
            CK3dEntity *ent = (CK3dEntity *)bo;
            for (CK3dEntity *child = ent->HierarchyParser(NULL); child != NULL; child = ent->HierarchyParser(child))
            {
                if (child->IsInScene(scn))
                {
                    CKStateChunk *chunk = scn->GetObjectInitialValue(child);
                    if (chunk)
                    {
                        CKReadObjectState(child, chunk);
                    }
                }
            }
        }
    }

    return CKBR_OK;
}