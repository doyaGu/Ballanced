/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Load CMO
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorLoadCMODecl();
CKERROR CreateLoadCMOProto(CKBehaviorPrototype **pproto);
int LoadCMO(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLoadCMODecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Load CMO");
    od->SetDescription("Sends Load CMO");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x63dd09a7, 0x567f7218));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLoadCMOProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLoadCMOProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Load CMO");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("CMO Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LoadCMO);

    *pproto = proto;
    return CK_OK;
}

int LoadCMO(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKSTRING name = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man || !man->GetGameInfo())
        context->OutputToConsoleExBeep("LoadCMO: gameInfo == NULL, exit CMO");

    man->SetCmoName(name);
    ::PostMessageA((HWND)context->GetMainWindow(), TT_MSG_CMO_LOAD, (WPARAM)man->GetCmoName(), 0);
    beh->ActivateOutput(0);

    return CKBR_OK;
}