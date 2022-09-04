/////////////////////////////////////
/////////////////////////////////////
//
//        TT_IsScriptActive
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorIsScriptActiveDecl();
CKERROR CreateIsScriptActiveProto(CKBehaviorPrototype **pproto);
int IsScriptActive(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsScriptActiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsScriptActive");
    od->SetDescription("testet ob Script aktiv ist");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x66e06b8a, 0x57db26d0));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsScriptActiveProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsScriptActiveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsScriptActive");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsScriptActive);

    *pproto = proto;
    return CK_OK;
}

int IsScriptActive(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}