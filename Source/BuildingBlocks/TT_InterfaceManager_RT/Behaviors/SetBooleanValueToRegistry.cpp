/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set Float Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetBooleanValueToRegistryDecl();
CKERROR CreateSetBooleanValueToRegistryProto(CKBehaviorPrototype **pproto);
int SetBooleanValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBooleanValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Boolean Value To Registry");
    od->SetDescription("Writes a Boolean value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x69fb3783, 0x50262517));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBooleanValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetBooleanValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Boolean Value To Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

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
    char regKey[512];
    char valueName[128];
    beh->GetInputParameterValue(0, &value);
    beh->GetInputParameterValue(1, regKey);
    beh->GetInputParameterValue(2, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char buffer[512];
    sprintf(buffer, "%s%s", gameInfo->regSubkey, regKey);

    HKEY hkResult;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(gameInfo->hkRoot, buffer, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: Failed to create %s %s", gameInfo->hkRoot, buffer);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_SetBooleanValueToRegistry: Failed to set %s %d", valueName, (value) ? "TRUE" : "FALSE");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    ::RegCloseKey(hkResult);
    beh->ActivateOutput(0);
    return CKBR_OK;
}