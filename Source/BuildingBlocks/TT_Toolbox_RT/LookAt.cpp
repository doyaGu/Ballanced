/////////////////////////////
/////////////////////////////
//
//        TT LookAt
//
/////////////////////////////
/////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorLookAtDecl();
CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto);
int LookAt(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLookAtDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT LookAt");
    od->SetDescription("Axis constrained lock at.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d4861f8, 0x2861703d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLookAtProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateLookAtProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT LookAt");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Following Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("look X-Axis", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("look Y-Axis", CKPGUID_BOOL, "false");
    proto->DeclareInParameter("look Z-Axis", CKPGUID_BOOL, "false");

    proto->DeclareSetting("Time Based", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Direction", CKPGUID_DIRECTION, "Z");
    proto->DeclareSetting("Roll", CKPGUID_ANGLE, "0:0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LookAt);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int LookAt(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}