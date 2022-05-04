/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set Float Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "StdAfx.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetBooleanValueToRegistryDecl();
CKERROR CreateSetBooleanValueToRegistryProto(CKBehaviorPrototype **);
int SetBooleanValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBooleanValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Boolean Value To Registry");
    od->SetDescription("Writes a boolean value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x69FB3783, 0x50262517));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBooleanValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetBooleanValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Boolean Value To Registry");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("True ? ", CKPGUID_BOOL);
    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetBooleanValueToRegistry);

    *pproto = proto;
    return CK_OK;
}

int SetBooleanValueToRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    BOOL value;
    char regKey[512] = {0};
    char valueName[128] = {0};
    beh->GetInputParameterValue(0, &value);
    beh->GetInputParameterValue(1, regKey);
    beh->GetInputParameterValue(2, valueName);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    static char buffer[4];
    _itoa(value, buffer, 10);
    if (!::WritePrivateProfileStringA(regKey, valueName, buffer, ini))
    {
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: Failed to write %s to %s.", regKey, ini);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}