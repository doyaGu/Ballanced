/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Set Rookie
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetRookieDecl();
CKERROR CreateSetRookieProto(CKBehaviorPrototype **pproto);
int SetRookie(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRookieDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Rookie");
    od->SetDescription("Set PlayerRookie");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x77de64df, 0x694f141f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRookieProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetRookieProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Rookie");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Active");
    proto->DeclareOutput("InActive");

    proto->DeclareInParameter("Is Rookie", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetRookie);

    *pproto = proto;
    return CK_OK;
}

int SetRookie(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKBOOL rookie;
    beh->GetInputParameterValue(0, &rookie);
    man->SetRookie(rookie == TRUE);

    beh->ActivateOutput(0);
    return CKBR_OK;
}