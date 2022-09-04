/////////////////////////////////////
/////////////////////////////////////
//
//        TT_IsObjectActive
//
/////////////////////////////////////
/////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorIsObjectActiveDecl();
CKERROR CreateIsObjectActiveProto(CKBehaviorPrototype **pproto);
int IsObjectActive(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsObjectActiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_IsObjectActive");
    od->SetDescription("testet ob Objekt aktiv ist");
    od->SetCategory("TT Toolbox/Test");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x409f03d9, 0x7616062c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsObjectActiveProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsObjectActiveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_IsObjectActive");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsObjectActive);

    *pproto = proto;
    return CK_OK;
}

int IsObjectActive(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}