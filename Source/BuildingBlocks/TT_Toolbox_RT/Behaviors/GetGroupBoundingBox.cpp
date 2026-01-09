//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT GetGroupBoundingBox
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetGroupBoundingBoxDecl();
CKERROR CreateGetGroupBoundingBoxProto(CKBehaviorPrototype **pproto);
int GetGroupBoundingBox(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGroupBoundingBoxDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetGroupBoundingBox");
    od->SetDescription("Get bounding box for group");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3cc07535, 0x37e351c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGroupBoundingBoxProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGroupBoundingBoxProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetGroupBoundingBox");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->DeclareOutParameter("BBox", CKPGUID_BOX);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGroupBoundingBox);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetGroupBoundingBox(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    int count = group->GetObjectCount();

    // Initialize with large values (10000 for min, -10000 for max)
    VxBbox bbox;
    bbox.Min.Set(10000.0f, 10000.0f, 10000.0f);
    bbox.Max.Set(-10000.0f, -10000.0f, -10000.0f);

    for (int i = 0; i < count; ++i)
    {
        CK3dEntity *entity = (CK3dEntity *)group->GetObject(i);
        if (CKIsChildClassOf(entity, CKCID_3DENTITY))
        {
            const VxBbox &entBox = entity->GetBoundingBox(FALSE);
            if (entBox.Min.x < bbox.Min.x) bbox.Min.x = entBox.Min.x;
            if (entBox.Min.y < bbox.Min.y) bbox.Min.y = entBox.Min.y;
            if (entBox.Min.z < bbox.Min.z) bbox.Min.z = entBox.Min.z;
            if (entBox.Max.x > bbox.Max.x) bbox.Max.x = entBox.Max.x;
            if (entBox.Max.y > bbox.Max.y) bbox.Max.y = entBox.Max.y;
            if (entBox.Max.z > bbox.Max.z) bbox.Max.z = entBox.Max.z;
        }
    }

    beh->SetOutputParameterValue(0, &bbox);
    return CKBR_OK;
}