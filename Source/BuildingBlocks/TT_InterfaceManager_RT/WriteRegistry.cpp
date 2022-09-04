/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Write Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include <stdlib.h>
#include <string.h>

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorWriteRegistryDecl();
CKERROR CreateWriteRegistryProto(CKBehaviorPrototype **pproto);
int WriteRegistry(const CKBehaviorContext &behcontext);
CKERROR WriteRegistryCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWriteRegistryDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_WriteRegistry");
    od->SetDescription("Writes an integer value to the registry");
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

int WriteIntegerToIni(const char *section, CKBehavior *beh, CKContext *context, int value, const char *key, const char *ini);
int WriteFloatToIni(const char *section, CKBehavior *beh, CKContext *context, float value, const char *key, const char *ini);
int WriteStringToIni(const char *section, CKBehavior *beh, CKContext *context, const char *value, const char *key, const char *ini);

static const char *SUBKEY = "Software\\Ballance\\";

int WriteRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    BOOL saveArrayMode = false;
    beh->GetLocalParameterValue(0, &saveArrayMode);

    char regSection[200] = {0};
    char regEntry[200] = {0};
    beh->GetInputParameterValue(0, regSection);
    beh->GetInputParameterValue(1, regEntry);

    char section[256] = {0};
    if (strncmp(regSection, SUBKEY, strlen(SUBKEY)) == 0)
    {
        char *str = &regSection[strlen(SUBKEY)];
        char *sep = strchr(str, '\\');
        if (sep)
        {
            strncpy(section, str, sep - str);
            section[sep - str] = '\0';
        }
        else
        {
            strncpy(section, str, strlen(str) + 1);
            section[strlen(str)] = '\0';
        }
    }

    char buffer[256];

    if (saveArrayMode)
    {
        CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(2);
        int cols = array->GetColumnCount();
        int rows = array->GetRowCount();

        strcat(section, ".");
        strcat(section, array->GetName());

        for (int c = 0; c < cols; ++c)
        {
            char num[32];
            _itoa(c, num, 10);
            strcat(section, ".");
            strcat(section, num);

            CK_ARRAYTYPE type = array->GetColumnType(c);
            for (int i = 0; i < rows; ++i)
            {
                switch (type)
                {
                case CKARRAYTYPE_INT:
                {
                    _itoa(i, num, 10);
                    strcpy(regEntry, num);
                    int val;
                    array->GetElementValue(i, c, &val);
                    WriteIntegerToIni(section, beh, context, val, regEntry, ini);
                }
                break;

                case CKARRAYTYPE_FLOAT:
                {
                    _itoa(i, num, 10);
                    strcpy(regEntry, num);
                    float val;
                    array->GetElementValue(i, c, &val);
                    WriteFloatToIni(section, beh, context, val, regEntry, ini);
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
                    WriteIntegerToIni(section, beh, context, val, regEntry, ini);
                }
                break;
                case CKARRAYTYPE_STRING:
                {
                    _itoa(i, num, 10);
                    strcpy(regEntry, num);

                    array->GetElementStringValue(i, c, buffer);
                    WriteStringToIni(section, beh, context, buffer, regEntry, ini);
                }
                default:
                    break;
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
            WriteIntegerToIni(section, beh, context, val, regEntry, ini);
        }
        else if (guid == CKPGUID_FLOAT)
        {
            float val;
            beh->GetInputParameterValue(3, &val);
            WriteFloatToIni(section, beh, context, val, regEntry, ini);
        }
        else if (guid == CKPGUID_STRING)
        {
            beh->GetInputParameterValue(3, buffer);
            WriteStringToIni(section, beh, context, buffer, regEntry, ini);
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
            beh->CreateOutputParameter(buffer, CKPGUID_INT);
        }

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

int WriteIntegerToIni(const char *section, CKBehavior *beh, CKContext *context, int value, const char *key, const char *ini)
{
    static char buffer[64];
    _itoa(value, buffer, 10);
    WriteStringToIni(section, beh, context, buffer, key, ini);

    return CKBR_OK;
}

int WriteFloatToIni(const char *section, CKBehavior *beh, CKContext *context, float value, const char *key, const char *ini)
{
    static char buffer[64];
    sprintf(buffer, "%f", value);
    WriteStringToIni(section, beh, context, buffer, key, ini);

    return CKBR_OK;
}

int WriteStringToIni(const char *section, CKBehavior *beh, CKContext *context, const char *value, const char *key, const char *ini)
{
    if (!::WritePrivateProfileStringA(section, key, value, ini))
    {
        context->OutputToConsoleExBeep("TT_WriteRegistry: Failed to write %s to %s.", section, ini);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    return CKBR_OK;
}