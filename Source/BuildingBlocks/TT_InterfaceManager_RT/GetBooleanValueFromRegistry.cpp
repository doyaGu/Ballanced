/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Get Boolean Value From Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetBooleanValueFromRegistryDecl();
CKERROR CreateGetBooleanValueFromRegistryProto(CKBehaviorPrototype **pproto);
int GetBooleanValueFromRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetBooleanValueFromRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Boolean Value From Registry");
    od->SetDescription("Reads a Boolean value from the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5ace7e01, 0x199b76fd));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetBooleanValueFromRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetBooleanValueFromRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Boolean Value From Registry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("RegKey  ..\\...\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->DeclareOutParameter("Integer Value ", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetBooleanValueFromRegistry);

    *pproto = proto;
    return CK_OK;
}

int GetBooleanValueFromRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    BOOL value = FALSE;
    char regKey[512] = {0};
    char valueName[128] = {0};
    beh->GetInputParameterValue(0, regKey);
    beh->GetInputParameterValue(1, valueName);

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_GetBooleanValueFromRegistry: im==NULL.");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_GetBooleanValueFromRegistry: System was not sent by the TT player");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    value = ::GetPrivateProfileIntA(regKey, valueName, -1, ini);
    if (value == -1)
    {
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->SetOutputParameterValue(0, &value, sizeof(BOOL));
    beh->ActivateOutput(0);
    return CKBR_OK;
}