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

static CKBOOL ReadIntegerFromRegistry(const char *subKey, CKContext *context, const char *valueName, int *value);
static CKBOOL ReadFloatFromRegistry(const char *subKey, CKContext *context, const char *valueName, float *value);
static CKBOOL ReadStringFromRegistry(const char *subKey, CKContext *context, const char *valueName, char *buffer, DWORD bufferSize);

CKObjectDeclaration *FillBehaviorReadRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ReadRegistry");
    od->SetDescription("Reads a value from the registry");
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

    char regSection[200] = {0};
    char regEntry[64] = {0};
    char arrayToLoad[64] = {0};
    beh->GetInputParameterValue(0, regSection);

    if (saveArrayMode)
        beh->GetInputParameterValue(3, arrayToLoad);
    else
        beh->GetInputParameterValue(1, regEntry);

    if (saveArrayMode)
    {
        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(2);
        if (!array)
        {
            context->OutputToConsoleExBeep("TT_ReadRegistry: Destination array is NULL");
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        int cols = array->GetColumnCount();
        int rows = array->GetRowCount();
        const char *arrayName = (arrayToLoad[0] != '\0') ? arrayToLoad : array->GetName();
        if (!arrayName || arrayName[0] == '\0')
        {
            context->OutputToConsoleExBeep("TT_ReadRegistry: Array name is empty");
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        strcat(regSection, "\\");
        strcat(regSection, arrayName);
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
                        int val = 0;
                        if (!ReadIntegerFromRegistry(regSection, context, regEntry, &val))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        array->SetElementValue(i, c, &val, sizeof(val));
                    }
                    break;

                    case CKARRAYTYPE_FLOAT:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        float val = 0.0f;
                        if (!ReadFloatFromRegistry(regSection, context, regEntry, &val))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        array->SetElementValue(i, c, &val, sizeof(val));
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
                        int val = 0;
                        if (!ReadIntegerFromRegistry(regSection, context, regEntry, &val))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        array->SetElementValue(i, c, &val, sizeof(val));
                    }
                    break;
                    case CKARRAYTYPE_STRING:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        char str[256] = {0};
                        if (!ReadStringFromRegistry(regSection, context, regEntry, str, sizeof(str)))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        array->SetElementStringValue(i, c, str);
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
            int val = 0;
            if (!ReadIntegerFromRegistry(regSection, context, regEntry, &val))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_FLOAT)
        {
            float val = 0.0f;
            if (!ReadFloatFromRegistry(regSection, context, regEntry, &val))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_STRING)
        {
            char str[256] = {0};
            if (!ReadStringFromRegistry(regSection, context, regEntry, str, sizeof(str)))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
            beh->SetOutputParameterValue(0, str, (int)strlen(str) + 1);
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
            data = beh->CreateOutputParameter(buffer, CKPGUID_INT);
        }

        if (data)
        {
            CKGUID guid = data->GetGUID();
            if (guid != CKPGUID_INT &&
                guid != CKPGUID_FLOAT &&
                guid != CKPGUID_BOOL &&
                guid != CKPGUID_STRING)
            {
                sprintf(buffer, "Data");
                context->DestroyObject(data);
                data = beh->CreateOutputParameter(buffer, CKPGUID_INT);
                if (data)
                {
                    int zero = 0;
                    data->SetValue(&zero, sizeof(zero));
                }
                context->OutputToConsoleExBeep("TT_ReadRegistry: ArrayColumnType invalid(use string/bool/int/float)");
            }
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
        beh->EnableInputParameter(3, TRUE);
        beh->EnableOutputParameter(0, FALSE);
    }
    else
    {
        beh->EnableInputParameter(1, TRUE);
        beh->EnableInputParameter(2, FALSE);
        beh->EnableInputParameter(3, FALSE);
        beh->EnableOutputParameter(0, TRUE);
    }

    return CKBR_OK;
}

static CKBOOL ReadIntegerFromRegistry(const char *subKey, CKContext *context, const char *valueName, int *value)
{
    if (!value)
        return FALSE;

    HKEY hkResult = NULL;
    if (::RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(*value);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)value, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    return TRUE;
}

static CKBOOL ReadFloatFromRegistry(const char *subKey, CKContext *context, const char *valueName, float *value)
{
    if (!value)
        return FALSE;

    HKEY hkResult = NULL;
    if (::RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    DWORD dwType = REG_DWORD;
    DWORD cbData = sizeof(*value);
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)value, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    return TRUE;
}

static CKBOOL ReadStringFromRegistry(const char *subKey, CKContext *context, const char *valueName, char *buffer, DWORD bufferSize)
{
    if (!buffer || bufferSize == 0)
        return FALSE;

    buffer[0] = '\0';

    HKEY hkResult = NULL;
    if (::RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_ReadRegistry: failed to open : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    DWORD dwType = REG_SZ;
    DWORD cbData = bufferSize;
    if (::RegQueryValueExA(hkResult, valueName, NULL, &dwType, (LPBYTE)buffer, &cbData) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_ReadRegistry:  ReadError: %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    buffer[bufferSize - 1] = '\0';
    return TRUE;
}
