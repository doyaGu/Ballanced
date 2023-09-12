/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Get Integer Value From Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetIntegerValueFromRegistryDecl();
CKERROR CreateGetIntegerValueFromRegistryProto(CKBehaviorPrototype **pproto);
int GetIntegerValueFromRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetIntegerValueFromRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Integer Value From Registry");
    od->SetDescription("Reads an integer value from the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x50c02b30, 0x2e5a2eda));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetIntegerValueFromRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetIntegerValueFromRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Integer Value From Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->DeclareOutParameter("Integer Value ", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetIntegerValueFromRegistry);

    *pproto = proto;
    return CK_OK;
}

int GetIntegerValueFromRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    int value = 0;
    char regKey[512];
    char valueName[128];
    beh->GetInputParameterValue(0, regKey);
    beh->GetInputParameterValue(1, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_GetIntegerValueFromRegistry: im==NULL.");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        context->OutputToConsoleExBeep("TT_GetIntegerValueFromRegistry: System was not sent by the TT player");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char buffer[512];
    sprintf(buffer, "%s%s", gameInfo->regSubkey, regKey);

    HKEY hkResult;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(gameInfo->hkRoot, buffer, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        value = 1;
        ::RegCloseKey(hkResult);
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(value);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)&value, &cbData) != ERROR_SUCCESS)
    {
        value = 2;
        ::RegCloseKey(hkResult);
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    ::RegCloseKey(hkResult);
    beh->SetOutputParameterValue(0, &value, cbData);
    beh->ActivateOutput(0);
    return CKBR_OK;
}