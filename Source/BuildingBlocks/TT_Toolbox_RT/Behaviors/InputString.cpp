//////////////////////////////////
//////////////////////////////////
//
//        TT InputString
//
//////////////////////////////////
//////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorInputStringDecl();
CKERROR CreateInputStringProto(CKBehaviorPrototype **pproto);
int InputString(const CKBehaviorContext &behcontext);
CKERROR InputStringCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorInputStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT InputString");
    od->SetDescription("Captures text entered by user.");
    od->SetCategory("TT Toolbox/Interface");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x52fd6294, 0x612f51a5));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateInputStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateInputStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT InputString");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("On");
    proto->DeclareOutput("Off");
    proto->DeclareOutput("Updated String");

    proto->DeclareInParameter("Reset String", CKPGUID_STRING);
    proto->DeclareInParameter("End Key", CKPGUID_KEY);

    proto->DeclareOutParameter("String", CKPGUID_STRING);
    proto->DeclareOutParameter("StringWithoutCaret", CKPGUID_STRING);
    proto->DeclareOutParameter("Key", CKPGUID_KEY);

    proto->DeclareSetting("Max Size", CKPGUID_INT, "512");
    proto->DeclareSetting("Keyboard Repetition", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Multiline", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Disable Keyboard Section", CKPGUID_KEYBOARDPART, "0");
    proto->DeclareSetting("Use Caret", CKPGUID_BOOL, "FALSE");
    proto->DeclareLocalParameter("", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(InputString);
    proto->SetBehaviorCallbackFct(InputStringCallBack);

    *pproto = proto;
    return CK_OK;
}

#define IN_ON				    0
#define IN_OFF				    1

#define OUT_ON				    0
#define	OUT_OFF				    1
#define	OUT_UPDATED			    2

#define PIN_STRING			    0
#define PIN_ENDKEY			    1

#define POUT_STRING			    0
#define POUT_STRINGWITHOUTCARET	1
#define	POUT_KEY			    2

#define LOCAL_MAXSIZE		    0
#define LOCAL_KEYBREP		    1
#define LOCAL_MULTI			    2
#define LOCAL_DISABLEKEYB	    3
#define LOCAL_SHOWCARET		    4
#define LOCAL_POS			    5
#define LOCAL_STR			    6

#define ESC					    27

#define DISABLE_NUMPAD			1
#define DISABLE_ARROWS			2

void RemoveCaret(char *str, int &caret, int len)
{
    memmove(str + caret, str + caret + 1, len - caret);
    str[len - 1] = '\0';
    caret = 0;
}

void RemoveAllCaret(char *str)
{
    int i = 0;
    int len = strlen(str);
    char *tmp = str;
    while (*tmp)
    {
        while (*tmp && *tmp != '\b')
        {
            tmp++;
            i++;
        }
        if (*tmp == '\b')
        {
            memmove(str + i, str + i + 1, len - i);
            str[len - 1] = '\0';
            len--;
        }
    }
}

void PageUp(char *str, int &caret)
{
    memmove(str + 1, str, caret);
    *str = '\b';
    caret = 0;
}

CKBOOL Home(CKBOOL multi, char *str, int &caret)
{
    if (caret <= 0 || str[caret - 1] == '\n')
    {
        caret = 0;
        return FALSE;
    }

    if (!multi)
    {
        PageUp(str, caret);
        return TRUE;
    }

    int pos = caret - 1;
    while (pos > 0 && (*(str + pos)) != '\n')
        --pos;
    if (*(str + pos) == '\n')
    {
        str = str + pos + 1;
        caret -= pos + 1;
        ++pos;
    }
    else
        pos = 0;

    memmove(str + 1, str, caret);
    *str = '\b';
    caret = pos;

    return TRUE;
}

CKBOOL PageDown(int currentlength, char *str, int &caret, int len, int max)
{
    if (caret >= (currentlength - 1))
        return FALSE;

    RemoveCaret(str, caret, len);
    if (len - 1 == max)
    {
        str[len - 1] = str[len - 2];
        str[len - 2] = '\b';
        str[len] = '\0';
        caret = len - 2;
    }
    else
    {
        str[len - 1] = '\b';
        str[len] = '\0';
        caret = len - 1;
    }

    return TRUE;
}

CKBOOL End(int currentlength, CKBOOL multi, char *str, int &caret, int len, int max)
{
    if (!multi)
        return PageDown(currentlength, str, caret, len, max);

    char *ptr = strchr(str + caret, '\n');
    if (!ptr)
        return PageDown(currentlength, str, caret, len, max);

    memmove(str + caret, str + caret + 1, ptr - (str + caret) - 1);
    caret = ptr - str - 1;
    str[caret] = '\b';

    return TRUE;
}

CKBOOL AddCharacter(char c, char *str, int &caret, int &currentlength, int size)
{
    if ((currentlength - 1) == size && caret == (currentlength - 2))
    {
        if (c == '\n')
            return FALSE;
        str[caret + 1] = c;
    }
    else
    {
        if (currentlength == size && caret == (currentlength - 1))
        {
            if (c == '\n')
                return FALSE;
            str[caret + 1] = c;
            str[caret + 2] = '\0';
        }
        else
        {
            int length = ((currentlength - 1) == size) ? size : currentlength;
            memmove(str + caret + 1, str + caret, length - caret);
            str[caret] = c;
            ++caret;
            str[length + 1] = '\0';
        }
        if (currentlength <= size)
            ++currentlength;
    }
    return TRUE;
}

int InputString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKParameterLocal *local = beh->GetLocalParameter(LOCAL_STR);
    if (!local)
        return CKBR_PARAMETERERROR;

    char *str = (CKSTRING)local->GetWriteDataPtr();
    if (!str)
        return CKBR_PARAMETERERROR;

    int size = 32;
    beh->GetLocalParameterValue(LOCAL_MAXSIZE, &size);
    if (size < 1)
        size = 1;

    int caret = 0;
    beh->GetLocalParameterValue(LOCAL_POS, &caret);

    int currentlength = strlen(str);

    CKInputManager *input = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);
    if (!input)
    {
        behcontext.Context->OutputToConsoleEx("Can't get the Input Manager");
        return CKBR_GENERICERROR;
    }

    if (beh->IsInputActive(IN_OFF))
    {
        CKBOOL keybrep = FALSE;
        beh->GetLocalParameterValue(LOCAL_KEYBREP, &keybrep);
        if (keybrep)
            input->EnableKeyboardRepetition(FALSE);

        RemoveCaret(str, caret, currentlength);
        beh->SetLocalParameterValue(LOCAL_POS, &caret);

        CKParameterOut *pout = beh->GetOutputParameter(POUT_STRING);
        pout->SetValue(str, strlen(str) + 1);
        beh->ActivateInput(IN_OFF, FALSE);
        beh->ActivateOutput(OUT_OFF);
        return CKBR_OK;
    }

    if (beh->IsInputActive(IN_ON))
    {
        CKBOOL keybrep = FALSE;
        beh->GetLocalParameterValue(LOCAL_KEYBREP, &keybrep);
        input->EnableKeyboardRepetition(keybrep);
        beh->ActivateInput(IN_ON, FALSE);
        char *resetstr = (CKSTRING)beh->GetInputParameterReadDataPtr(PIN_STRING);
        if (!resetstr)
            resetstr = (char*)"";
        CKParameterOut *pout = beh->GetOutputParameter(POUT_STRINGWITHOUTCARET);
        pout->SetValue(resetstr, strlen(resetstr) + 1);

        int len = strlen(resetstr);
        if (len < size)
        {
            strcpy(str, resetstr);
            *(str + len) = '\b';
            caret = len;
            *(str + (len + 1)) = '\0';
        }
        else
        {
            strncpy(str, resetstr, size);
            *(str + size) = *(str + size - 1);
            *(str + size - 1) = '\b';
            caret = size - 1;
            *(str + size + 1) = '\0';
        }
        beh->SetLocalParameterValue(LOCAL_POS, &caret);

        CKBOOL showcaret = FALSE;
        beh->GetLocalParameterValue(LOCAL_SHOWCARET, &showcaret);
        if (showcaret)
        {
            pout = beh->GetOutputParameter(POUT_STRING);
            pout->SetValue(str, strlen(str) + 1);
        }
        else
        {
            char *tmp = new char[strlen(str) + 1];
            memcpy(tmp, str, strlen(str) + 1);
            int tcaret = caret;
            RemoveCaret(tmp, tcaret, strlen(tmp));
            pout = beh->GetOutputParameter(POUT_STRING);
            pout->SetValue(tmp, strlen(tmp) + 1);
            delete[] tmp;
        }

        beh->ActivateOutput(OUT_ON);
        return CKBR_ACTIVATENEXTFRAME;
    }

    CKBOOL multi = FALSE;
    beh->GetLocalParameterValue(LOCAL_MULTI, &multi);
    int endkey = 0;
    if (multi)
        beh->GetInputParameterValue(PIN_ENDKEY, &endkey);

    int dkey = 0;
    beh->GetLocalParameterValue(LOCAL_DISABLEKEYB, &dkey);

    CKBOOL changed = FALSE;
    int keynumber = input->GetNumberOfKeyInBuffer();
    char c = 0;
    CKDWORD key = 0;
    for (int i = 0; i < keynumber; ++i)
    {
        if (input->GetKeyFromBuffer(i, key) == KEY_PRESSED)
        {
            beh->SetOutputParameterValue(POUT_KEY, &key);

            if ((multi && endkey && endkey == key) || (!multi && (key == CKKEY_RETURN || key == CKKEY_NUMPADENTER)))
            {
                char *tmp = new char[strlen(str) + 1];
                memcpy(tmp, str, strlen(str) + 1);
                int tcaret = caret;
                RemoveCaret(tmp, tcaret, strlen(tmp));
                CKParameterOut *pout = beh->GetOutputParameter(POUT_STRING);
                pout->SetValue(tmp, strlen(tmp) + 1);
                delete[] tmp;
                beh->SetLocalParameterValue(LOCAL_POS, &caret);
                beh->ActivateOutput(OUT_OFF, TRUE);
                return CKBR_OK;
            }

            switch (key)
            {
                case CKKEY_BACK:
                    if (caret > 0)
                    {
                        if ((currentlength - 1) == size && caret == (currentlength - 2))
                        {
                            *(str + caret + 1) = '\0';
                            if ((caret + 1) > size + 1)
                            {
                                int i = 0;
                            }
                        }
                        else
                        {
                            memmove(str + caret - 1, str + caret, currentlength - caret);
                            --caret;
                            *(str + currentlength - 1) = '\0';
                            if ((currentlength - 1) > size + 1)
                            {
                                int i = 0;
                            }
                        }
                        changed = TRUE;
                        --currentlength;
                    }
                    break;

                case CKKEY_DELETE:
                    if (currentlength > 1)
                    {
                        if ((currentlength - 1) == size && caret == (currentlength - 2))
                        {
                            *(str + caret + 1) = '\0';
                            if ((caret + 1) > size + 1)
                            {
                                int i = 0;
                            }
                            changed = TRUE;
                            --currentlength;
                        }
                        else if (caret < (currentlength - 1))
                        {
                            memmove(str + caret + 1, str + caret + 2, currentlength - caret - 2);
                            *(str + currentlength - 1) = '\0';
                            if ((currentlength - 1) > size + 1)
                            {
                                int i = 0;
                            }
                            changed = TRUE;
                            --currentlength;
                        }
                    }
                    break;

                case CKKEY_PRIOR:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    if (caret > 0)
                    {
                        PageUp(str, caret);
                        changed = TRUE;
                    }
                    break;

                case CKKEY_HOME:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    changed = Home(multi, str, caret);
                    break;

                case CKKEY_NEXT:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    changed = PageDown(currentlength, str, caret, currentlength, size);
                    break;

                case CKKEY_END:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    changed = End(currentlength, multi, str, caret, currentlength, size);
                    break;

                case CKKEY_LEFT:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    if (caret > 0)
                    {
                        str[caret] = str[caret - 1];
                        --caret;
                        str[caret] = '\b';
                        changed = TRUE;
                    }
                    break;

                case CKKEY_RIGHT:
                    if (dkey & DISABLE_ARROWS)
                        continue;
                    if (caret < (currentlength - 1) && (caret + 1) < size)
                    {
                        str[caret] = str[caret + 1];
                        ++caret;
                        str[caret] = '\b';
                        changed = TRUE;
                    }
                    break;

                case CKKEY_RETURN:
                    AddCharacter('\n', str, caret, currentlength, size);
                    changed = TRUE;
                    break;
                case CKKEY_NUMPADENTER:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('\n', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPADEQUALS:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('=', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPADCOMMA:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter(',', str, caret, currentlength, size);
                    changed = TRUE;
                    break;
                case CKKEY_DIVIDE:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('/', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_MULTIPLY:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('*', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_SUBTRACT:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('-', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_ADD:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('+', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_DECIMAL:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('.', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD0:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('0', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD1:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('1', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD2:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('2', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD3:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('3', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD4:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('4', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD5:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('5', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD6:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('6', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD7:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('7', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD8:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('8', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                case CKKEY_NUMPAD9:
                    if (dkey & DISABLE_NUMPAD)
                        continue;
                    AddCharacter('9', str, caret, currentlength, size);
                    changed = TRUE;
                    break;

                default:
                    c = VxScanCodeToAscii(key, input->GetKeyboardState());
                    if (c && c != ESC)
                    {
                        AddCharacter(c, str, caret, currentlength, size);
                        changed = TRUE;
                    }
                    break;
            }
        }
    }

    if (changed)
    {
        beh->SetLocalParameterValue(LOCAL_POS, &caret);
        CKBOOL showcaret = FALSE;
        beh->GetLocalParameterValue(LOCAL_SHOWCARET, &showcaret);
        if (showcaret)
        {
            CKParameterOut *pout = beh->GetOutputParameter(POUT_STRING);
            pout->SetValue(str, strlen(str) + 1);
        }
        else
        {
            char *tmp = new char[strlen(str) + 1];
            memcpy(tmp, str, strlen(str) + 1);
            int tcaret = caret;
            RemoveCaret(tmp, tcaret, strlen(tmp));
            CKParameterOut *pout = beh->GetOutputParameter(POUT_STRING);
            pout->SetValue(tmp, strlen(tmp) + 1);
            delete[] tmp;
        }
        beh->ActivateOutput(OUT_UPDATED, TRUE);
    }

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR InputStringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
        case CKM_BEHAVIORATTACH:
        case CKM_BEHAVIORLOAD:
        case CKM_BEHAVIORSETTINGSEDITED:
        {
            CKBOOL multi = FALSE;
            beh->GetLocalParameterValue(LOCAL_MULTI, &multi);
            CKParameterIn *pin = beh->GetInputParameter(PIN_ENDKEY);
            if (!multi)
            {
                if (pin)
                    pin->Enable(FALSE);
            }
            else
            {
                if (pin)
                    pin->Enable(TRUE);
            }
            int size = 32;
            beh->GetLocalParameterValue(LOCAL_MAXSIZE, &size);
            if (size < 1)
                size = 1;
            char *str = new char[size + 2];
            memset(str, 0, size + 2);
            CKParameterOut *pout = beh->GetOutputParameter(POUT_STRING);
            pout->SetValue(str, size + 2);
            delete[] str;

            int pos = 0;
            beh->SetLocalParameterValue(LOCAL_POS, &pos);
        }
            break;
    }
    return CKBR_OK;
}