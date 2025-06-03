/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetCaretPosition
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetCaretPositionDecl();
CKERROR CreateSetCaretPositionProto(CKBehaviorPrototype **);
int SetCaretPosition(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetCaretPositionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Caret Position");
    od->SetDescription("Insert a caret into a string.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>activated when the behavior's process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Text: </SPAN>string to insert caret into.<BR>
    <SPAN CLASS=pin>Caret Position: </SPAN>position of the caret in the string. -1 means no caret.<BR>
    <SPAN CLASS=pout>Text: </SPAN>string with caret into.<BR>
    */
    /* warning:
    - the caret will only be displayed by 2D Text and 3D Text building blocks.
    */
    od->SetCategory("Interface/Text");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xf760d6e, 0x7ce7589a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetCaretPositionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return (od);
}

CKERROR CreateSetCaretPositionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Caret Position");
    if (!proto)
        return (CKERR_OUTOFMEMORY);

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Text", CKPGUID_STRING);
    proto->DeclareInParameter("Caret Position", CKPGUID_INT);

    proto->DeclareOutParameter("Text", CKPGUID_STRING);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetCaretPosition);

    *pproto = proto;
    return (CK_OK);
}

int SetCaretPosition(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!string)
        return CKBR_OK;

    int pos = -1;
    beh->GetInputParameterValue(1, &pos);

    int count = strlen(string);

    CKSTRING news = new char[2 + count];
    CKSTRING newp = news;

    // we remove an eventual caret
    CKSTRING s = string;
    while (*s)
    {
        if (*s != '\b')
        {
            *newp = *s;
            ++newp;
        }
        else
        {
            --count;
        }
        ++s;
    }
    // the null char
    *newp = '\0';

    XString tempString(news);

    // we insert the caret
    if ((pos >= 0) && (pos <= count))
    {
        newp = news;
        s = tempString.Str();

        // before caret
        int i;
        for (i = 0; i < pos; ++i)
        {
            *newp = *s;
            ++newp;
            ++s;
        }

        // the caret
        *newp = '\b';
        ++newp;

        // after caret
        for (; i <= count; ++i)
        {
            *newp = *s;
            ++newp;
            ++s;
        }
        ++count;
    }

    beh->SetOutputParameterValue(0, news, count + 1);

    return (CKBR_OK);
}
