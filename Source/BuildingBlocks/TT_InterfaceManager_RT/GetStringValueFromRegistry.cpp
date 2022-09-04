/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Get String Value From Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

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

    char str[256] = {0};
    char regKey[512] = {0};
    char valueName[128] = {0};
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

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_GetStringValueFromRegistry: System was not sent by the TT player");
        beh->SetOutputParameterValue(0, " ");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    int size = ::GetPrivateProfileStringA(regKey, valueName, "", str, 256, ini);
    if (strcmp(str, "") == 0)
    {
        beh->SetOutputParameterValue(0, " ");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    beh->SetOutputParameterValue(0, str, size + 1);
    beh->ActivateOutput(0);
    return CKBR_OK;
}