/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Read Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "InterfaceManager.h"

#include <stdlib.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

CKObjectDeclaration *FillBehaviorReadRegistryDecl();
CKERROR CreateReadRegistryProto(CKBehaviorPrototype **pproto);
int ReadRegistry(const CKBehaviorContext &behcontext);
CKERROR ReadRegistryCallBack(const CKBehaviorContext &behcontext);

int ReadIntegerFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName);
float ReadFloatFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName);
char *ReadStringFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName);

CKObjectDeclaration *FillBehaviorReadRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReadRegistry");
    od->SetDescription("Writes an integer value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x460044b5, 0x6e927b66));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReadRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateReadRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ReadRegistry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("Reg-Section  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Reg-Entry", CKPGUID_STRING);
    proto->DeclareInParameter("Destination-Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Array to load", CKPGUID_STRING);

    proto->DeclareOutParameter("Data", CKPGUID_INT);

    proto->DeclareSetting("SaveArray-Mode", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReadRegistry);

    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS);
    proto->SetBehaviorCallbackFct(ReadRegistryCallBack);

    *pproto = proto;
    return CK_OK;
}

int ReadRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    BOOL saveArrayMode = false;
    beh->GetLocalParameterValue(0, &saveArrayMode);

    char regSection[200];
    char regEntry[64];
    char arrayToLoad[64];
    beh->GetInputParameterValue(0, regSection);

    if (saveArrayMode)
        beh->GetInputParameterValue(3, arrayToLoad);
    else
        beh->GetInputParameterValue(1, regEntry);

    if (saveArrayMode)
    {
        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(2);
        int cols = array->GetColumnCount();
        int rows = array->GetRowCount();
        strcat(regSection, "\\");
        strcat(regSection, array->GetName());
        strcat(regSection, "\\");

        char buffer[200];
        strcpy(buffer, regSection);

        if (cols > 0)
        {
            for (int c = 0; c < cols; ++c)
            {
                strcpy(regSection, buffer);
                char num[32];
                _itoa(c, num, 10);
                strcat(regSection, num);
                strcat(regSection, "\\");

                CK_ARRAYTYPE type = array->GetColumnType(c);
                for (int i = 0; i < rows; ++i)
                {
                    switch (type)
                    {
                    case CKARRAYTYPE_INT:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        int val = ReadIntegerFromRegistry(regSection, beh, context, regEntry);
                        array->SetElementValue(i, c, &val);
                    }
                    break;

                    case CKARRAYTYPE_FLOAT:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        float val = ReadFloatFromRegistry(regSection, beh, context, regEntry);
                        array->SetElementValue(i, c, &val);
                    }
                    break;

                    case CKARRAYTYPE_PARAMETER:
                    {
                        CKParameter *parameter = *(CKParameter **)array->GetElement(i, c);
                        if (parameter->GetGUID() != CKPGUID_BOOL)
                        {
                            context->OutputToConsoleExBeep("TT_ReadRegistry: ArrayColumnType invalid(use string/bool/int/float)");
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        int val = ReadIntegerFromRegistry(regSection, beh, context, regEntry);
                        array->SetElementValue(i, c, &val);
                    }
                    break;
                    case CKARRAYTYPE_STRING:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        char *str = ReadStringFromRegistry(regSection, beh, context, regEntry);
                        array->SetElementValue(i, c, str);
                    }
                    default:
                        break;
                    }
                }
            }
        }
    }
    else
    {
        CKGUID guid = beh->GetOutputParameter(0)->GetGUID();
        if (guid == CKPGUID_INT || guid == CKPGUID_BOOL)
        {
            int val = ReadIntegerFromRegistry(regSection, beh, context, regEntry);
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_FLOAT)
        {
            float val = ReadFloatFromRegistry(regSection, beh, context, regEntry);
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_STRING)
        {
            char *str = ReadStringFromRegistry(regSection, beh, context, regEntry);
            beh->SetOutputParameterValue(0, str, 256);
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR ReadRegistryCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    char buffer[16];

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORATTACH:
    case CKM_BEHAVIORLOAD:
        break;
    case CKM_BEHAVIOREDITED:
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        CKParameterOut *data = beh->GetOutputParameter(0);
        if (!data)
        {
            sprintf(buffer, "Data");
            beh->CreateOutputParameter(buffer, CKPGUID_INT);
        }

        CKGUID guid = data->GetGUID();
        if (guid != CKPGUID_INT &&
            guid != CKPGUID_FLOAT &&
            guid != CKPGUID_BOOL &&
            guid != CKPGUID_STRING)
        {
            sprintf(buffer, "Data");
            context->DestroyObject(data);
            beh->CreateOutputParameter(buffer, CKPGUID_INT);
            int zero = 0;
            data->SetValue(&zero, sizeof(zero));
            context->OutputToConsoleExBeep("TT_ReadRegistry: ArrayColumnType invalid(use string/bool/int/float)");
        }
    }
    default:
        break;
    }

    BOOL saveArrayMode = false;
    beh->GetLocalParameterValue(0, &saveArrayMode);
    if (saveArrayMode)
    {
        beh->EnableInputParameter(1, FALSE);
        beh->EnableInputParameter(2, TRUE);
        beh->EnableInputParameter(3, FALSE);
        beh->EnableOutputParameter(0, FALSE);
    }
    else
    {
        beh->EnableInputParameter(1, TRUE);
        beh->EnableInputParameter(2, FALSE);
        beh->EnableInputParameter(3, TRUE);
        beh->EnableOutputParameter(0, TRUE);
    }

    return CKBR_OK;
}

int ReadIntegerFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName)
{
    HKEY hkResult;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : %s\\%s", HKEY_LOCAL_MACHINE, subKey);
        beh->ActivateOutput(1);
        return 0;
    }

    int value;
    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(int);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)&value, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s %d.", valueName, value);
        beh->ActivateOutput(1);
        return 0;
    }

    ::RegCloseKey(hkResult);
    return value;
}

float ReadFloatFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName)
{
    HKEY hkResult;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : %s\\%s", HKEY_LOCAL_MACHINE, subKey);
        beh->ActivateOutput(1);
        return 0;
    }

    float value;
    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(float);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)&value, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s %f.", valueName, value);
        beh->ActivateOutput(1);
        return 0;
    }

    ::RegCloseKey(hkResult);
    return value;
}

char *ReadStringFromRegistry(const char *subKey, CKBehavior *beh, CKContext *context, const char *valueName)
{
    HKEY hkResult;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : %s\\%s", HKEY_LOCAL_MACHINE, subKey);
        beh->ActivateOutput(1);
        return NULL;
    }

    static char str[256];
    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(str);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)str, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s %s.", valueName, str);
        beh->ActivateOutput(1);
        return NULL;
    }

    ::RegCloseKey(hkResult);
    return &str[0];
}
