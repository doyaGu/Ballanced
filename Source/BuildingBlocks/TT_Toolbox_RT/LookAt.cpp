/////////////////////////////
/////////////////////////////
//
//        TT LookAt
//
/////////////////////////////
/////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTLookAtBehaviorProto(CKBehaviorPrototype **pproto);
int TTLookAt(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTLookAtDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT LookAt");
    od->SetDescription("Achsenbeschr�nktes Lock at.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d4861f8,0x2861703d));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTLookAtBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTLookAtBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT LookAt");
    if(!proto) return CKERR_OUTOFMEMORY;

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
    proto->SetFunction(TTLookAt);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTLookAt(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTLookAtCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}