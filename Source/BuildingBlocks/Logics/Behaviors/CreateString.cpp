/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CreateString
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#ifndef _WIN32
#pragma cpp_extensions on
#endif

CKObjectDeclaration *FillBehaviorCreateStringDecl();
CKERROR CreateCreateStringProto(CKBehaviorPrototype **);
int CreateString(const CKBehaviorContext &behcontext);
CKERROR CreateStringCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorCreateStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create String");
    od->SetDescription("Creates a string by concatenating elements, separating them by an optionnal delimiter.");
    /* rem:
    <SPAN CLASS=in>Create: </SPAN>creates the string by reading the inputs parameters.<BR>
    <SPAN CLASS=in>Add Line: </SPAN>adds to the currently created string a new line by reading the inputs parameters.<BR>
    <SPAN CLASS=in>Clear: </SPAN>clears the currently created string.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Delimiter: </SPAN>the delimiter character or string to insert between given elements.<BR>
    <BR>
    <SPAN CLASS=pout>Text: </SPAN>the created string.<BR>
    <BR>
    You can add as many Input Parameters as you want (of any type !).<BR>
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4bcd2f9d, 0x382652e2));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateCreateStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCreateStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Create String");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Add Line");
    proto->DeclareInput("Clear");

    proto->DeclareOutput("Create Out");
    proto->DeclareOutput("Add Line Out");
    proto->DeclareOutput("Clear Out");

    proto->DeclareInParameter("Delimiter", CKPGUID_STRING);

    proto->DeclareOutParameter("Text", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDLOCALPARAMS));
    proto->SetFunction(CreateString);
    proto->SetBehaviorCallbackFct(CreateStringCallBack, CKCB_BEHAVIORLOAD | CKCB_BEHAVIOREDITED);

    *pproto = proto;
    return CK_OK;
}

int CreateString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    if (beh->GetVersion() < 0x00020000)
        beh->ActivateOutput(0);

    int pincount = beh->GetInputParameterCount();

    // We get the in
    CKSTRING delim = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!delim)
        delim = "";

    // We get the string
    CKSTRING string = (CKSTRING)beh->GetOutputParameterWriteDataPtr(0);

    ///
    // Clear
    if (beh->IsInputActive(2))
    {
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(2);

        char nullstring[1] = "";
        beh->SetOutputParameterValue(0, nullstring, 1);
        if (!beh->IsInputActive(0) && !beh->IsInputActive(1))
            return CKBR_OK;
    }

    // We create the string
    CKParameterIn *pin;
    CKParameter *pout;
    CKSTRING format;

    char *paramstring = NULL;
    XString buffer;

    int i;
    for (i = 1; i < pincount; ++i)
    {
        pin = beh->GetInputParameter(i);
        pout = pin->GetRealSource();
        if (!pout)
            continue;

        // We reformat
        format = (CKSTRING)beh->GetLocalParameterReadDataPtr(i - 1);
        if (format && (*format))
        {
            if (pout->GetGUID() == CKPGUID_INT)
            {
                int v = 0;
                pout->GetValue(&v);
                paramstring = new char[32];
                sprintf(paramstring, format, v);
            }
            else
            {
                if (pout->GetGUID() == CKPGUID_FLOAT)
                {
                    float v = 0;
                    pout->GetValue(&v);
                    paramstring = new char[32];
                    sprintf(paramstring, format, v);
                }
                else
                {
                    int paramsize = pout->GetStringValue(NULL);
                    if (paramsize)
                    {
                        XAP<char> paramtemp(new char[paramsize]);
                        pout->GetStringValue(paramtemp, FALSE);
                        paramstring = new char[paramsize];
                        if (format[strlen(format) - 1] != 's')
                            sprintf(paramstring, "%s", &(*paramtemp));
                        else
                            sprintf(paramstring, format, &(*paramtemp));
                    }
                    else
                        paramstring = NULL;
                }
            }
        }
        else
        { // no format was provided
            int paramsize = pout->GetStringValue(NULL);
            if (paramsize)
            {
                paramstring = new char[paramsize],
                pout->GetStringValue(paramstring, FALSE);
            }
            else
                paramstring = NULL;
        }

        if (i != 1)
            buffer << delim;
        buffer << paramstring;

        // Deletion of the string of the current parameter
        delete[] paramstring;
        paramstring = NULL;
    }

    if (beh->IsInputActive(0))
    {
        ///
        // Create
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        beh->SetOutputParameterValue(0, buffer.Str(), buffer.Length() + 1);
    }
    else
    {
        ///
        // Add Line
        if (beh->IsInputActive(1))
        {
            beh->ActivateInput(1, FALSE);
            beh->ActivateOutput(1);

            if (!string)
                string = "";
            int size = strlen(string) + buffer.Length() + 2;
            char *temp = new char[size];
            if (!beh->GetLocalParameter(0) || string[0] != '\0') // Old Version if no local param
            {
                sprintf(temp, "%s\n%s", string, buffer.CStr());
            }
            else
            {
                sprintf(temp, "%s", buffer.CStr());
                size -= 1;
            }
            beh->SetOutputParameterValue(0, temp, size);
            delete[] temp;
        }
    }

    return CKBR_OK;
}

/*******************************************************/
/*                     CALLBACK                        */
/*******************************************************/
CKERROR CreateStringCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    char buffer[64] = {0};

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORLOAD:
    {
        for (int i = 0; i < beh->GetLocalParameterCount(); i++)
        {
            CKParameterLocal *plocal = beh->GetLocalParameter(i);
            plocal->ModifyObjectFlags(CK_PARAMETEROUT_SETTINGS, 0);
        }
    }
    break;
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        CKParameter *pout;
        int i;
        for (i = 1; i < beh->GetInputParameterCount(); i++)
        {
            pin = beh->GetInputParameter(i);
            pout = beh->GetLocalParameter(i - 1);
            if (pout) // the output exist, we only have to check its type
            {
                if (pout->GetType() != pin->GetType())
                {
                    pout->SetName(pin->GetName());
                }
            }
            else // we have to create the output
            {
                CKParameterLocal *plocal = beh->CreateLocalParameter(pin->GetName(), CKPGUID_STRING);
                plocal->ModifyObjectFlags(CK_PARAMETEROUT_SETTINGS, 0);
                if (pin->GetGUID() == CKPGUID_INT)
                {
                    strcpy(buffer, "%d");
                }
                else
                {
                    if (pin->GetGUID() == CKPGUID_FLOAT)
                    {
                        strcpy(buffer, "%g");
                    }
                    else
                    {
                        strcpy(buffer, "%s");
                    }
                }

                // We set the format string
                plocal->SetValue(buffer, strlen(buffer) + 1);
            }
        }
        i -= 1; // offset the local to the input (delimiter)
        for (; i < beh->GetLocalParameterCount();)
        {
            CKDestroyObject(beh->RemoveLocalParameter(i));
        }
    }
    }

    return CKBR_OK;
}
