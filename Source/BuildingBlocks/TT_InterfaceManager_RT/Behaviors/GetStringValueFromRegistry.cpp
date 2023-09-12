/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Get String Value From Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorGetStringValueFromRegistryDecl();
CKERROR CreateGetStringValueFromRegistryProto(CKBehaviorPrototype **pproto);
int GetStringValueFromRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetStringValueFromRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get String Value From Registry");
    od->SetDescription("Reads a string value from the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x75ff55a7, 0x378205d0));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetStringValueFromRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetStringValueFromRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get String Value From Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->DeclareOutParameter("String Value ", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetStringValueFromRegistry);

    *pproto = proto;
    return CK_OK;
}

int GetStringValueFromRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    char str[256];
    char regKey[512];
    char valueName[128];
    beh->GetInputParameterValue(0, regKey);
    beh->GetInputParameterValue(1, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_GetStringValueFromRegistry: im==NULL.");
        beh->SetOutputParameterValue(0, " ");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        context->OutputToConsoleExBeep("TT_GetStringValueFromRegistry: System was not sent by the TT player");
        beh->SetOutputParameterValue(0, " ");
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
        beh->SetOutputParameterValue(0, " ");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    DWORD dwType = REG_SZ;
    DWORD cbData = sizeof(str);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)str, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        beh->SetOutputParameterValue(0, " ");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    ::RegCloseKey(hkResult);
    beh->SetOutputParameterValue(0, str, cbData);
    beh->ActivateOutput(0);
    return CKBR_OK;
}