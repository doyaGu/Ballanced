/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Is Rookie
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorIsRookieDecl();
CKERROR CreateIsRookieProto(CKBehaviorPrototype **pproto);
int IsRookie(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsRookieDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Is Rookie");
    od->SetDescription("Check PlayerRookie");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6dc06abb, 0x44451de5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsRookieProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsRookieProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Is Rookie");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Rookie");
    proto->DeclareOutput("No Rookie");
    proto->DeclareOutput("FAILED");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsRookie);

    *pproto = proto;
    return CK_OK;
}

int IsRookie(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        beh->ActivateOutput(2);
        return CKBR_OK;
    }

    if (man->IsRookie())
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    beh->ActivateOutput(2);
    return CKBR_OK;
}