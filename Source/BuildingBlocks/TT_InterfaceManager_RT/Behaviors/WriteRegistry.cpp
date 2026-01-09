/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Write Registry
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

CKObjectDeclaration *FillBehaviorWriteRegistryDecl();
CKERROR CreateWriteRegistryProto(CKBehaviorPrototype **pproto);
int WriteRegistry(const CKBehaviorContext &behcontext);
CKERROR WriteRegistryCallBack(const CKBehaviorContext &behcontext);

static CKBOOL WriteIntegerToRegistry(const char *subKey, CKContext *context, int value, const char *valueName);
static CKBOOL WriteFloatToRegistry(const char *subKey, CKContext *context, float value, const char *valueName);
static CKBOOL WriteStringToRegistry(const char *subKey, CKContext *context, const char *str, const char *valueName);

CKObjectDeclaration *FillBehaviorWriteRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_WriteRegistry");
    od->SetDescription("Writes a value to the registry");
    od->SetCategory("TT InterfaceManager/Registry");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7aa8084e, 0x1ee229b7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWriteRegistryProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWriteRegistryProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_WriteRegistry");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("OK");
    proto->DeclareOutput("FAILED");

    proto->DeclareInParameter("Reg-Section  ...\\..\\", CKPGUID_STRING);
    proto->DeclareInParameter("Reg-Entry", CKPGUID_STRING);
    proto->DeclareInParameter("Destination-Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Data", CKPGUID_INT);

    proto->DeclareSetting("SaveArray-Mode", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WriteRegistry);

    proto->SetBehaviorCallbackFct(WriteRegistryCallBack);

    *pproto = proto;
    return CK_OK;
}

int WriteRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    BOOL saveArrayMode = false;
    beh->GetLocalParameterValue(0, &saveArrayMode);

    char regSection[200] = {0};
    char regEntry[200] = {0};
    beh->GetInputParameterValue(0, regSection);
    beh->GetInputParameterValue(1, regEntry);

    char buffer[256];

    if (saveArrayMode)
    {
        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(2);
        if (!array)
        {
            context->OutputToConsoleExBeep("TT_WriteRegistry: Destination array is NULL");
            beh->ActivateOutput(1);
            return CKBR_OK;
        }
        int cols = array->GetColumnCount();
        int rows = array->GetRowCount();
        strcat(regSection, "\\");
        strcat(regSection, array->GetName());
        strcat(regSection, "\\");

        strcpy(buffer, regSection);

        if (cols > 0)
        {
            for (int c = 0; c < cols; ++c)
            {
                CK_ARRAYTYPE type = array->GetColumnType(c);
                for (int i = 0; i < rows; ++i)
                {
                    strcpy(regSection, buffer);
                    char num[32];
                    _itoa(c, num, 10);
                    strcat(regSection, num);
                    strcat(regSection, "\\");

                    switch (type)
                    {
                    case CKARRAYTYPE_INT:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        int val;
                        array->GetElementValue(i, c, &val);
                        if (!WriteIntegerToRegistry(regSection, context, val, regEntry))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                    }
                    break;

                    case CKARRAYTYPE_FLOAT:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        float val;
                        array->GetElementValue(i, c, &val);
                        if (!WriteFloatToRegistry(regSection, context, val, regEntry))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                    }
                    break;

                    case CKARRAYTYPE_PARAMETER:
                    {
                        CKParameter *parameter = *(CKParameter **)array->GetElement(i, c);
                        if (parameter->GetGUID() != CKPGUID_BOOL)
                        {
                            context->OutputToConsoleExBeep("TT_WriteRegistry: ArrayColumnType invalid(use string/bool/int/float)");
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        int val;
                        array->GetElementValue(i, c, &val);
                        if (!WriteIntegerToRegistry(regSection, context, val, regEntry))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
                    }
                    break;
                    case CKARRAYTYPE_STRING:
                    {
                        _itoa(i, num, 10);
                        strcpy(regEntry, num);
                        array->GetElementStringValue(i, c, buffer);
                        if (!WriteStringToRegistry(regSection, context, buffer, regEntry))
                        {
                            beh->ActivateOutput(1);
                            return CKBR_OK;
                        }
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
        CKGUID guid = beh->GetInputParameter(3)->GetGUID();
        if (guid == CKPGUID_INT || guid == CKPGUID_BOOL)
        {
            int val;
            beh->GetInputParameterValue(3, &val);
            if (!WriteIntegerToRegistry(regSection, context, val, regEntry))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
        }
        else if (guid == CKPGUID_FLOAT)
        {
            float val;
            beh->GetInputParameterValue(3, &val);
            if (!WriteFloatToRegistry(regSection, context, val, regEntry))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
        }
        else if (guid == CKPGUID_STRING)
        {
            beh->GetInputParameterValue(3, buffer);
            if (!WriteStringToRegistry(regSection, context, buffer, regEntry))
            {
                beh->ActivateOutput(1);
                return CKBR_OK;
            }
        }
    }

    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR WriteRegistryCallBack(const CKBehaviorContext &behcontext)
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
        CKParameterIn *data = beh->GetInputParameter(3);
        if (!data)
        {
            sprintf(buffer, "Data");
            data = beh->CreateInputParameter(buffer, CKPGUID_INT);
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
                data->SetGUID(CKPGUID_INT, TRUE, buffer);
                context->OutputToConsoleExBeep("TT_WriteRegistry: ArrayColumnType invalid(use string/bool/int/float)");
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
        beh->EnableInputParameter(3, FALSE);
    }
    else
    {
        beh->EnableInputParameter(1, TRUE);
        beh->EnableInputParameter(2, FALSE);
        beh->EnableInputParameter(3, TRUE);
    }

    return CKBR_OK;
}

static CKBOOL WriteIntegerToRegistry(const char *subKey, CKContext *context, int value, const char *valueName)
{
    HKEY hkResult = NULL;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to create : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to set : %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    return TRUE;
}

static CKBOOL WriteFloatToRegistry(const char *subKey, CKContext *context, float value, const char *valueName)
{
    HKEY hkResult = NULL;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to create : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_DWORD, (LPBYTE)&value, sizeof(value)) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to set : %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    return TRUE;
}

static CKBOOL WriteStringToRegistry(const char *subKey, CKContext *context, const char *str, const char *valueName)
{
    if (!str)
        str = "";

    HKEY hkResult = NULL;
    DWORD dwDisposition;
    if (::RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, 0, 0, KEY_ALL_ACCESS, 0, &hkResult, &dwDisposition) != ERROR_SUCCESS)
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to create : HKEY_CURRENT_USER\\%s", subKey);
        return FALSE;
    }

    if (::RegSetValueExA(hkResult, valueName, 0, REG_SZ, (LPBYTE)str, (DWORD)strlen(str) + 1) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hkResult);
        context->OutputToConsoleExBeep("TT_WriteRegistry: failed to set : %s.", valueName);
        return FALSE;
    }

    ::RegCloseKey(hkResult);
    return TRUE;
}
