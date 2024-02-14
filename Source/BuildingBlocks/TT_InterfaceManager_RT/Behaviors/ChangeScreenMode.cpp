/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         TT Change ScreenMode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorChangeScreenModeDecl();
CKERROR CreateChangeScreenModeProto(CKBehaviorPrototype **pproto);
int ChangeScreenMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChangeScreenModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Change ScreenMode");
    od->SetDescription("Change Screenmode");
    od->SetCategory("TT InterfaceManager/Display");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38b84d97, 0x13932f28));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChangeScreenModeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateChangeScreenModeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Change ScreenMode");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Driver ID", CKPGUID_INT);
    proto->DeclareInParameter("ScreenMode ID", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ChangeScreenMode);

    *pproto = proto;
    return CK_OK;
}

int ChangeScreenMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int driver = 0;
    int screenMode = 0;
    beh->GetInputParameterValue(0, &driver);
    beh->GetInputParameterValue(1, &screenMode);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("Change Screen Mode: im == NULL");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    VxDriverDesc *drDesc = context->GetRenderManager()->GetRenderDriverDescription(driver);
    if (!drDesc)
    {
        context->OutputToConsoleExBeep("Change Screen Mode: No Driver Description for Driver-ID '%d' is found", driver);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

#if CKVERSION == 0x13022002
    if (screenMode < 0 || screenMode >= drDesc->DisplayModeCount ||
#else
    if (screenMode < 0 || screenMode >= drDesc->DisplayModes.Size() ||
#endif
        drDesc->DisplayModes[screenMode].Bpp < 16)
    {
        context->OutputToConsoleExBeep("Change Screen Mode: ScreenMode is not exist");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (!::SendMessageA((HWND)context->GetMainWindow(), TT_MSG_SCREEN_MODE_CHG, screenMode, driver))
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}