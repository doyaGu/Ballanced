/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT GetGroupBoundingBox
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorGetGroupBoundingBoxDecl();
CKERROR CreateGetGroupBoundingBoxProto(CKBehaviorPrototype **);
int GetGroupBoundingBox(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetGroupBoundingBoxDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetGroupBoundingBox");
    od->SetDescription("Fetches BoundingBox for group");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3CC07535, 0x37E351C));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetGroupBoundingBoxProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetGroupBoundingBoxProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetGroupBoundingBox");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Group", CKPGUID_GROUP);

    proto->DeclareOutParameter("BBox", CKPGUID_BOX);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetGroupBoundingBox);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int GetGroupBoundingBox(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}