/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              OutputToConsole
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorOutputToConsoleDecl();
CKERROR CreateOutputToConsoleProto(CKBehaviorPrototype **);
int OutputToConsole(const CKBehaviorContext &behcontext);
CKERROR OutputToConsoleCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorOutputToConsoleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Output To Console");
    od->SetDescription("Displays a string in the Dev console.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    */
    od->SetCategory("Logics/Strings");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x18655b3f, 0x68291dc3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateOutputToConsoleProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateOutputToConsoleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Output To Console");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("String", CKPGUID_STRING);
    proto->DeclareInParameter("Output Target?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Beep?", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(OutputToConsole);

    *pproto = proto;
    return CK_OK;
}

int OutputToConsole(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // string to display
    CKSTRING string = NULL;
    string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);
    if (!string)
        string = "";

    // display the target of the behavior before displaying the actual message
    CKBOOL outputOwner = FALSE;
    beh->GetInputParameterValue(1, &outputOwner);

    // Beep when outputting
    CKBOOL beep = FALSE;
    beh->GetInputParameterValue(2, &beep);

    if (outputOwner)
    {
        CKBeObject *beo = beh->GetTarget();

        CKSTRING targetName = "No Target";
        if (beo && beo->GetName())
            targetName = beo->GetName();

        if (beep)
            behcontext.Context->OutputToConsoleExBeep("%s : %s", targetName, string);
        else
            behcontext.Context->OutputToConsoleEx("%s : %s", targetName, string);
    }
    else
    {
        behcontext.Context->OutputToConsole(string, beep);
    }

    return CKBR_OK;
}
