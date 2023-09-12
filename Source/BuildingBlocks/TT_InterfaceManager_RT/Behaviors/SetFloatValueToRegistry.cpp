/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set Float Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetFloatValueToRegistryDecl();
CKERROR CreateSetFloatValueToRegistryProto(CKBehaviorPrototype **pproto);
int SetFloatValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFloatValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Float Value To Registry");
    od->SetDescription("Writes a Float value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x60522f, 0x41d4c1f));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFloatValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetFloatValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Float Value To Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("Float Value ", CKPGUID_FLOAT);
    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFloatValueToRegistry);

    *pproto = proto;
    return CK_OK;
}

int SetFloatValueToRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    float value;
    char regKey[512] = {0};
    char valueName[128] = {0};
    beh->GetInputParameterValue(0, &value);
    beh->GetInputParameterValue(1, regKey);
    beh->GetInputParameterValue(2, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetFloatValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        context->OutputToConsoleExBeep("TT_SetFloatValueToRegistry: System was not sent by the TT player");
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
        context->OutputToConsoleExBeep("TT_SetFloatValueToRegistry: Failed to create %s %s", gameInfo->hkRoot, buffer);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_SetFloatValueToRegistry: Failed to set %s %f", valueName, value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    ::RegCloseKey(hkResult);
    beh->ActivateOutput(0);
    return CKBR_OK;
}