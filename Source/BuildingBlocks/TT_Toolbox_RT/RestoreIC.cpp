/////////////////////////////////
/////////////////////////////////
//
//        TT Restore IC
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTRestoreICBehaviorProto(CKBehaviorPrototype **pproto);
int TTRestoreIC(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTRestoreICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restore IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e0f2f58,0x232d3109));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTRestoreICBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTRestoreICBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restore IC");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTRestoreIC);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTRestoreIC(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTRestoreICCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}