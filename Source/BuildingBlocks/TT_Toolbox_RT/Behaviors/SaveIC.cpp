//////////////////////////////
//////////////////////////////
//
//        TT Save IC
//
//////////////////////////////
//////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSaveICDecl();
CKERROR CreateSaveICProto(CKBehaviorPrototype **pproto);
int SaveIC(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSaveICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Save IC");
    od->SetDescription("Saves an object's Initial Condition in the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x30362f34, 0x1935316f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSaveICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Save IC");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveIC);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SaveIC(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKBeObject *target = beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    CKScene *scene = behcontext.CurrentScene;
    if (!scene)
        return CKBR_OWNERERROR;

    if (target->IsInScene(scene))
    {
        CKStateChunk *chunk = CKSaveObjectState(target);
        scene->SetObjectInitialValue(target, chunk);
    }

    if (target->GetClassID() == CKCID_3DENTITY ||
        target->GetClassID() == CKCID_CAMERA ||
        target->GetClassID() == CKCID_TARGETCAMERA ||
        target->GetClassID() == CKCID_CURVEPOINT ||
        target->GetClassID() == CKCID_SPRITE3D ||
        target->GetClassID() == CKCID_LIGHT ||
        target->GetClassID() == CKCID_TARGETLIGHT ||
        target->GetClassID() == CKCID_CHARACTER ||
        target->GetClassID() == CKCID_3DOBJECT ||
        target->GetClassID() == CKCID_BODYPART ||
        target->GetClassID() == CKCID_CURVE)
    {
        CKBOOL hierarchy = FALSE;
        beh->GetInputParameterValue(0, &hierarchy);
        if (hierarchy)
        {
            CK3dEntity *entity = (CK3dEntity *)target;
            for (CK3dEntity *child = entity->HierarchyParser(NULL); child != NULL; child = entity->HierarchyParser(child))
            {
                if (child->IsInScene(scene))
                {
                    CKStateChunk *chunk = CKSaveObjectState(child);
                    scene->SetObjectInitialValue(child, chunk);
                }
            }
        }
    }

    return CKBR_OK;
}