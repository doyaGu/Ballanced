/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set Integer Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorSetIntegerValueToRegistryDecl();
CKERROR CreateSetIntegerValueToRegistryProto(CKBehaviorPrototype **pproto);
int SetIntegerValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetIntegerValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set Integer Value To Registry");
    od->SetDescription("Writes an integer value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x175c07d1, 0x1d235544));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetIntegerValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetIntegerValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set Integer Value To Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("Integer Value ", CKPGUID_INT);
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

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CGameInfo *gameInfo = man->GetGameInfo();
    if (!gameInfo)
    {
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: System was not sent by the TT player");
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
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: Failed to create %s %s", gameInfo->hkRoot, buffer);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_SetIntegerValueToRegistry: Failed to set %s %d", valueName, value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    ::RegCloseKey(hkResult);
    beh->ActivateOutput(0);
    return CKBR_OK;
}