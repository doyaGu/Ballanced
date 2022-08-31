/////////////////////////////////////////
/////////////////////////////////////////
//
//        TT Restore Dynamic IC
//
/////////////////////////////////////////
/////////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTRestoreDynamicICBehaviorProto(CKBehaviorPrototype **pproto);
int TTRestoreDynamicIC(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTRestoreDynamicICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restore Dynamic IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x680269cb,0x50814d03));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTRestoreDynamicICBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTRestoreDynamicICBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restore Dynamic IC");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTRestoreDynamicIC);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTRestoreDynamicIC(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTRestoreDynamicICCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}