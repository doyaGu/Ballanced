/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SplitString
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSplitStringDecl();
CKERROR CreateSplitStringProto(CKBehaviorPrototype **);
int SplitString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSplitStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Scan String");
    od->SetDescription("Retrieves values (float, vector or anything else) from a string.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Text: </SPAN>the string to scan.<BR>
    <SPAN CLASS=pin>Delimiter: </SPAN>the delimiter character or string to skip between the different elements.<BR>
    <BR>
    <SPAN CLASS=pout>Elements Found: </SPAN>counts the number of elements succesfuly retrieved from the string.<BR>
    <BR>
    eg:<BR>
    Let "Hello_3.14_(0,1,0)" be the string.<BR>
    Let "_" be the delimiter.<BR>
    Then just create 3 outputs : a String, a Float and a Vector.<BR>
    You should then get the string "Hello", the float 3.14 and the vector (0,1,0).<BR>
    */
    /* warning:
    - Try to use appropriated delimiters (like * or / or _ ). Because some Parameter Types contains {Spaces} inside their representation.
    Like TIME ( 0m 5s 3ms ).<BR>
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4afa2f11, 0x34872e2));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSplitStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSplitStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Scan String");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Delimiter", CKPGUID_STRING);

    proto->DeclareOutParameter("Elements Found", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETEROUTPUTS));
    proto->SetFunction(SplitString);

    *pproto = proto;
    return CK_OK;
}

int SplitString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // We get the string
    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!string)
        return CKBR_OK;

    // we have to test is the input is not the same than one of the output
    string = CKStrdup(string);

    // We get the in
    CKSTRING delim = (CKSTRING)beh->GetInputParameterReadDataPtr(1);
    if (!delim)
        delim = " ";
    if (*delim == '\0')
        delim = " ";
    int delimsize = strlen(delim);

    int elts = 0;
    CKSTRING str = string;
    CKSTRING del = NULL;

    char buffer[256];
    *buffer = '\0';

    while (del = strstr(str, delim))
    {
        if (del != str) // delimiter at the start of the string, we skip it
        {
            // found a new item
            ++elts;
            CKParameterOut *pout = beh->GetOutputParameter(elts);
            if (!pout)
                break;

            int size = del - str;
            if (size)
            {
                // we copy the characters to the buffer
                memcpy(buffer, str, size);
                *(buffer + size) = '\0';
                // we fill the parameter out
                pout->SetStringValue(buffer);
            }
        }

        str = del + delimsize;
    }

    // Now the last element
    int sizeleft = strlen(str);
    if (sizeleft)
    {
        // found a new item
        ++elts;
        CKParameterOut *pout = beh->GetOutputParameter(elts);
        if (!pout)
            return CKBR_OK;

        // we copy the characters to the buffer
        memcpy(buffer, str, sizeleft);
        *(buffer + sizeleft) = '\0';
        // we fill the parameter out
        pout->SetStringValue(buffer);
    }

    beh->SetOutputParameterValue(0, &elts);

    // if it exists, we delete it
    CKDeletePointer(string);

    return CKBR_OK;
}