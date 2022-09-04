/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Set String Value To Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorSetStringValueToRegistryDecl();
CKERROR CreateSetStringValueToRegistryProto(CKBehaviorPrototype **pproto);
int SetStringValueToRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetStringValueToRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Set String Value To Registry");
    od->SetDescription("Writes a string value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x74a35f75, 0x3ad00faa));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetStringValueToRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetStringValueToRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Set String Value To Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("String Value (256 chars)", CKPGUID_STRING);
    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetStringValueToRegistry);

    *pproto = proto;
    return CK_OK;
}

int SetStringValueToRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    char str[256] = {0};
    char regKey[512] = {0};
    char valueName[128] = {0};
    beh->GetInputParameterValue(0, str);
    beh->GetInputParameterValue(1, regKey);
    beh->GetInputParameterValue(2, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetStringValueToRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_SetStringValueToRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    if (!::WritePrivateProfileStringA(regKey, valueName, str, ini))
    {
        context->OutputToConsoleExBeep("TT_SetStringValueToRegistry: Failed to write %s to %s.", regKey, ini);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}