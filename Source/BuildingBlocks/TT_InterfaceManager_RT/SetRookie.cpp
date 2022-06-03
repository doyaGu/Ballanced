/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           TT Set Rookie
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetRookieDecl();
CKERROR CreateSetRookieProto(CKBehaviorPrototype **);
int SetRookie(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRookieDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Rookie");
    od->SetDescription("Sets rookie mode");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x77DE64DF, 0x694F141F));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRookieProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetRookieProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Rookie");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Active");
    proto->DeclareOutput("Inactive");

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

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    BOOL rookie;
    beh->GetInputParameterValue(0, &rookie);
    man->SetRookie(rookie == TRUE);

    beh->ActivateOutput(0);
    return CKBR_OK;
}