/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		        TT Window Activate?
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorWindowActivateDecl();
CKERROR CreateWindowActivateProto(CKBehaviorPrototype **pproto);
int WindowActivate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWindowActivateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Window Activate?");
    od->SetDescription("TT Window Activate?");
    od->SetCategory("TT InterfaceManager/Display");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x70936783, 0x7fee4a3b));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWindowActivateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWindowActivateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Window Activate?");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Activate");
    proto->DeclareOutput("else");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WindowActivate);

    *pproto = proto;
    return CK_OK;
}

int WindowActivate(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (man && man->IsWindowActivated())
    {
        man->SetWindowActivated(false);
        beh->ActivateOutput(0);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}