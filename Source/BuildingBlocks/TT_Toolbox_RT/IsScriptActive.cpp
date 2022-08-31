/////////////////////////////////////
/////////////////////////////////////
//
//        TT_IsScriptActive
//
/////////////////////////////////////
/////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTIsScriptActiveBehaviorProto(CKBehaviorPrototype **pproto);
int TTIsScriptActive(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTIsScriptActiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsScriptActive");
    od->SetDescription("testet ob Script aktiv ist");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x66e06b8a,0x57db26d0));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTIsScriptActiveBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTIsScriptActiveBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsScriptActive");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTIsScriptActive);

    *pproto = proto;
    return CK_OK;
}

int TTIsScriptActive(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTIsScriptActiveCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}