/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set Integer Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetIntegerValueToRegistryDecl();
CKERROR CreateSetIntegerValueToRegistryProto(CKBehaviorPrototype **);
int SetIntegerValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetIntegerValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Integer Value To Registry");
    od->SetDescription("Writes an integer value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x175C07D1, 0x1D235544));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetIntegerValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetIntegerValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Integer Value To Registry");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("Integer Value", CKPGUID_INT);
    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetIntegerValueToRegistry);

    *pproto = proto;
    return CK_OK;
}

int SetIntegerValueToRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int value;
    beh->GetInputParameterValue(0, &value);
    CKSTRING regKey = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
    CKSTRING valueName = (CKSTRING)beh->GetInputParameterReadDataPtr(2);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    static char buffer[64];
    _itoa(value, buffer, 10);
    if (!::WritePrivateProfileStringA(regKey, valueName, buffer, ini))
    {
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: Failed to write %s to %s.", regKey, ini);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}