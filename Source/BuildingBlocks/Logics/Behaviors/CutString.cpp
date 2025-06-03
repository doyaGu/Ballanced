/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CutString
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCutStringDecl();
CKERROR CreateCutStringProto(CKBehaviorPrototype **);
int CutString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCutStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get SubString");
    od->SetDescription("Gets a sub string from an initial string.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>String: </SPAN>the initial string.<BR>
    <SPAN CLASS=pin>Start: </SPAN>the position of the first character.<BR>
    <SPAN CLASS=pin>Length: </SPAN>the length of the substring to copy. 0 means the whole string.<BR>
    <BR>
    <SPAN CLASS=pout>SubString: </SPAN>the sub string.<BR>
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x10b16051, 0x26173b39));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCutStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCutStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get SubString");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("String", CKPGUID_STRING);
    proto->DeclareInParameter("Start", CKPGUID_INT, "0");
    proto->DeclareInParameter("Length", CKPGUID_INT, "1");

    proto->DeclareOutParameter("SubString", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CutString);

    *pproto = proto;
    return CK_OK;
}

int CutString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    ///
    // Input / Outputs
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // We get the string
    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!string)
        return CKBR_OK;

    int len = strlen(string);

    // We get the string
    CKParameterOut *pout = beh->GetOutputParameter(0);
    int reslen = pout->GetDataSize();

    int cutstart = 1;
    beh->GetInputParameterValue(1, &cutstart);

    int cutlen = 0;
    beh->GetInputParameterValue(2, &cutlen);

    if (!cutlen) // no size specified, we want to copy all
    {
        cutlen = len - cutstart;
    }
    else // size specified, we must check for out bound
    {
        if (cutstart + cutlen > len)
        {
            cutlen = len - cutstart;
        }
    }

    // If what we have to copy is larger than the buffer we have, we reallocate
    if (cutlen >= reslen)
    {
        pout->SetValue(NULL, cutlen + 1);
    }

    CKSTRING result = (CKSTRING)pout->GetWriteDataPtr();
    if (cutstart < 0)
    {
        cutlen += cutstart;
        cutstart = 0;
    }

    // We test if we are not too much to the left of the source string
    if (cutlen <= 0 || cutstart >= len)
        *result = 0;
    else
    {
        strncpy(result, string + cutstart, cutlen);
        *(result + cutlen) = 0;
    }

    pout->DataChanged();

    return CKBR_OK;
}