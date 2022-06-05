/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Exit to System
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSendExitToSystemDecl();
CKERROR CreateSendExitToSystemProto(CKBehaviorPrototype **);
int SendExitToSystem(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSendExitToSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Exit to System");
    od->SetDescription("Sends Exit to System");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5BC50AF4, 0x41E92536));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSendExitToSystemProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSendExitToSystemProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Exit to System");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SendExitToSystem);

    *pproto = proto;
    return CK_OK;
}

int SendExitToSystem(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_EXIT_TO_SYS, 0, 0);
    
    beh->ActivateOutput(0);
    return CKBR_OK;
}