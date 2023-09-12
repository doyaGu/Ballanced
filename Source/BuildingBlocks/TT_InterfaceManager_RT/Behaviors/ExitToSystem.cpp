/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Exit to System
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorExitToSystemDecl();
CKERROR CreateExitToSystemProto(CKBehaviorPrototype **pproto);
int ExitToSystem(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorExitToSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Exit to System");
    od->SetDescription("Sends Exit to System");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5bc50af4, 0x41e92536));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExitToSystemProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateExitToSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Exit to System");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ExitToSystem);

    *pproto = proto;
    return CK_OK;
}

int ExitToSystem(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_EXIT_TO_SYS, 0, 0);

    beh->ActivateOutput(0);
    return CKBR_OK;
}