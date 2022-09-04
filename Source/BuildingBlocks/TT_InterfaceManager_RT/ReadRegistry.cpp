/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          TT Read Registry
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_InterfaceManager_RT.h"

#include <stdlib.h>
#include <string.h>

#include "InterfaceManager.h"

CKObjectDeclaration *FillBehaviorReadRegistryDecl();
CKERROR CreateReadRegistryProto(CKBehaviorPrototype **pproto);
int ReadRegistry(const CKBehaviorContext &behcontext);
CKERROR ReadRegistryCallBack(const CKBehaviorContext &behcontext);

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

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS));
    proto->SetBehaviorCallbackFct(ReadRegistryCallBack);

    *pproto = proto;
    return CK_OK;
}

int ReadIntegerFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini);
float ReadFloatFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini);
char *ReadStringFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini);

static const char *SUBKEY = "Software\\Ballance\\";

int ReadRegistry(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    InterfaceManager *man = InterfaceManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_ReadRegistry: im==NULL.");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    char *ini = man->GetIniName();
    if (!ini)
    {
        context->OutputToConsoleExBeep("TT_ReadRegistry: System was not sent by the TT player");
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    BOOL saveArrayMode = false;
    beh->GetLocalParameterValue(0, &saveArrayMode);

    char regSection[200] = {0};
    char regEntry[200] = {0};
    char arrayToLoad[200] = {0};
    beh->GetInputParameterValue(0, regSection);

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

    if (saveArrayMode)
        beh->GetInputParameterValue(3, arrayToLoad);
    else
        beh->GetInputParameterValue(1, regEntry);

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
                    int val = ReadIntegerFromIni(section, beh, context, regEntry, ini);
                    array->SetElementValue(i, c, &val);
                }
                break;

                case CKARRAYTYPE_FLOAT:
                {
                    _itoa(i, num, 10);
                    strcpy(regEntry, num);
                    float val = ReadFloatFromIni(section, beh, context, regEntry, ini);
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
                    int val = ReadIntegerFromIni(section, beh, context, regEntry, ini);
                    array->SetElementValue(i, c, &val);
                }
                break;
                case CKARRAYTYPE_STRING:
                {
                    _itoa(i, num, 10);
                    strcpy(regEntry, num);
                    char *str = ReadStringFromIni(section, beh, context, regEntry, ini);
                    array->SetElementValue(i, c, str);
                }
                default:
                    break;
                }
            }
        }
    }
    else
    {
        CKGUID guid = beh->GetOutputParameter(0)->GetGUID();
        if (guid == CKPGUID_INT || guid == CKPGUID_BOOL)
        {
            int val = ReadIntegerFromIni(section, beh, context, regEntry, ini);
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_FLOAT)
        {
            float val = ReadFloatFromIni(section, beh, context, regEntry, ini);
            beh->SetOutputParameterValue(0, &val);
        }
        else if (guid == CKPGUID_STRING)
        {
            char *str = ReadStringFromIni(section, beh, context, regEntry, ini);
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

int ReadIntegerFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini)
{
    return ::GetPrivateProfileIntA(section, key, 0, ini);
}

float ReadFloatFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini)
{
    static char buffer[64];

    ::GetPrivateProfileStringA(section, key, "", buffer, 64, ini);
    if (strcmp(buffer, "") == 0)
        return 0;

    return (float)atof(buffer);
}

char *ReadStringFromIni(const char *section, CKBehavior *beh, CKContext *context, const char *key, const char *ini)
{
    static char buffer[256];

    ::GetPrivateProfileStringA(section, key, "", buffer, 256, ini);
    if (strcmp(buffer, "") == 0)
        return NULL;

    return (char *)buffer;
}
