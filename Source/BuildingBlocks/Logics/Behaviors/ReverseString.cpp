/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ReverseString
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorReverseStringDecl();
CKERROR CreateReverseStringProto(CKBehaviorPrototype **);
int ReverseString(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorReverseStringDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Reverse String");
    od->SetDescription("Reverse the given string.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>activated when the behavior's process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Text: </SPAN>string to reverse.<BR>
    */
    /* warning:
    - this behavior will alter the original string.
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x72b17c2e, 0x644e2563));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReverseStringProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return (od);
}

CKERROR CreateReverseStringProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Reverse String");
    if (!proto)
        return (CKERR_OUTOFMEMORY);

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("String", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReverseString);

    *pproto = proto;
    return (CK_OK);
}

int ReverseString(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    int len, i, lim;
    char tmp;

    if (!string)
    {
        beh->ActivateOutput(0);
        return (CKBR_OK);
    }

    len = strlen(string);

    lim = len / 2;
    len--;

    for (i = 0; i < lim; i++)
    {
        tmp = string[i];
        string[i] = string[len - i];
        string[len - i] = tmp;
    }

    beh->ActivateOutput(0);
    return (CKBR_OK);
}
