/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Player Active
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorPlayerActiveDecl();
CKERROR CreatePlayerActiveProto(CKBehaviorPrototype **pproto);
int PlayerActive(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPlayerActiveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Player Active");
    od->SetDescription("Check PlayerActive");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x694658d, 0x7ffd236f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePlayerActiveProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreatePlayerActiveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Player Active");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Active");
    proto->DeclareOutput("InActive");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(PlayerActive);

    *pproto = proto;
    return CK_OK;
}

int PlayerActive(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    if (!context->IsInInterfaceMode())
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_OK;
}