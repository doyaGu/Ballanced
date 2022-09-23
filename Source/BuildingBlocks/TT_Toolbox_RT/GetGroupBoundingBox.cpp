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
    // TODO: To be finished.
    return CKBR_OK;
}