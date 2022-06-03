/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		   TT Get Float Value From Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorGetFloatValueFromRegistryDecl();
CKERROR CreateGetFloatValueFromRegistryProto(CKBehaviorPrototype **);
int GetFloatValueFromRegistry(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetFloatValueFromRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Get Float Value From Registry");
    od->SetDescription("Reads a float value from the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xFE6FE4, 0x12A66150));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetFloatValueFromRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetFloatValueFromRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Get Float Value From Registry");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("RegKey  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Value Name", CKPGUID_STRING);

    proto->DeclareOutParameter("Float Value", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetFloatValueFromRegistry);

    *pproto = proto;
    return CK_OK;
}

int GetFloatValueFromRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    float value = 0;
    char regKey[512] = {0};
    char valueName[128] = {0};
    beh->GetInputParameterValue(0, regKey);
    beh->GetInputParameterValue(1, valueName);

    CTTInterfaceManager *man = CTTInterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_GetFloatValueFromRegistry: im==NULL.");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_GetFloatValueFromRegistry: System was not sent by the TT player");
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    static char buffer[64];
    ::GetPrivateProfileStringA(regKey, valueName, "", buffer, 64, ini);
    if (strcmp(buffer, "") == 0)
    {
        beh->SetOutputParameterValue(0, &value);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    value = (float)atof(buffer);
    beh->SetOutputParameterValue(0, &value, sizeof(float));

    beh->ActivateOutput(0);
    return CKBR_OK;
}