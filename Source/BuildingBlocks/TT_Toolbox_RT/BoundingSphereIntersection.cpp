/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	       TT Bounding Sphere Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl();
CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **);
int BoundingSphereIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Bounding Sphere Intersection");
    od->SetDescription("Detects the collision between two 3D entities at the bounding sphere level.");
    od->SetCategory("TT Toolbox/Collision");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32A6308D, 0x1D3D0931));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoundingSphereIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Bounding Sphere Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

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

int BoundingSphereIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}
