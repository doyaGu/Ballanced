/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Limit Framerate
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorLimitFramerateDecl();
CKERROR CreateLimitFramerateProto(CKBehaviorPrototype **pproto);
int LimitFramerate(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLimitFramerateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Limit Framerate");
    od->SetDescription("Limits the framerate.");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x12a40e8f, 0x6f2d0613));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLimitFramerateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLimitFramerateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Limit Framerate");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Framerate", CKPGUID_INT, "60");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LimitFramerate);

    *pproto = proto;
    return CK_OK;
}

int LimitFramerate(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int framerate = 60;
    beh->GetInputParameterValue(0, &framerate);

    ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(0)->GetWindowHandle(), TT_MSG_LIMIT_FPS, framerate, 0);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    return CKBR_OK;
}