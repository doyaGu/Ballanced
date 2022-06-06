/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT Load CMO
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include <string.h>

#include "ErrorProtocol.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSendCMOLoadDecl();
CKERROR CreateSendCMOLoadProto(CKBehaviorPrototype **);
int SendCMOLoad(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSendCMOLoadDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Load CMO");
    od->SetDescription("Sends Load CMO");
    od->SetCategory("TT InterfaceManager/General");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x63DD09A7, 0x567F7218));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSendCMOLoadProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSendCMOLoadProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Load CMO");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("CMO Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SendCMOLoad);

    *pproto = proto;
    return CK_OK;
}

int SendCMOLoad(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CKSTRING name = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man || !man->GetGameInfo())
        TT_ERROR("SendCMOLoad.cpp", "int SendCMOLoad(...)", " gameInfo == NULL, exit CMO");

    man->SetCmoName(name);
    ::PostMessageA((HWND)context->GetRenderManager()->GetRenderContext(man->GetDriver())->GetWindowHandle(), TT_MSG_CMO_LOAD, (WPARAM)man->GetCmoName(), 0);
    beh->ActivateOutput(0);

    return CKBR_OK;
}