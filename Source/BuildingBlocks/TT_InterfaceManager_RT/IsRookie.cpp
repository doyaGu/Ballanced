/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Is Rookie
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorIsRookieDecl();
CKERROR CreateIsRookieProto(CKBehaviorPrototype **);
int IsRookie(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorIsRookieDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Is Rookie");
    od->SetDescription("Gets rookie mode");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6DC06ABB, 0x44451DE5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsRookieProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsRookieProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Is Rookie");
    if (!proto)
        return CKERR_OUTOFMEMORY;

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

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
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