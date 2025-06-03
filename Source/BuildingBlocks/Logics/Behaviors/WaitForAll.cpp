/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            WaitForAll
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorWaitForAllDecl();
CKERROR CreateWaitForAllProto(CKBehaviorPrototype **);
int WaitForAll(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWaitForAllDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Wait For All");
    od->SetDescription("Waits until all behavior Inputs have been activated to activate the output");
    /* rem:
    <SPAN CLASS=in>In 0: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>In 1: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated only when all inputs have been activated once.<BR>
    <BR>
    When all inputs have been activated, the building block activates the output and resets all the inputs.<BR>
    <BR>
    You can add as many Input as needed.<BR>
    When the ouptut is activated, all the inputs are reseted.<BR>
    All the activated inputs remains activate until the output is triggered.
    Therefore they needn't to be active all at the same time.<BR>
    */
    od->SetCategory("Logics/Synchro");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xc044a999, 0xfdfefaf7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWaitForAllProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWaitForAllProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Wait For All");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In 0");
    proto->DeclareInput("In 1");

    proto->DeclareOutput("Out");

    proto->SetBehaviorFlags(CKBEHAVIOR_VARIABLEINPUTS);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(WaitForAll);

    *pproto = proto;
    return CK_OK;
}

int WaitForAll(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    int c = beh->GetInputCount();
    int a;
    for (a = 0; a < c; a++)
        if (!beh->IsInputActive(a))
            break;

    if (a == c) // Ok ... ALL TRUE
    {
        for (a = 0; a < c; a++)
        {
            beh->ActivateInput(a, FALSE);
        }
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
