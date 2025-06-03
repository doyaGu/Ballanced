/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            While
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorWhileDecl();
CKERROR CreateWhileProto(CKBehaviorPrototype **);
int While(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorWhileDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("While");
    od->SetDescription("Loops while 'Condition' is TRUE.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Loop In: </SPAN>triggers the next step in a process loop.<BR>
    <BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when 'Condition' becomes False.<BR>
    <SPAN CLASS=out>Loop Out: </SPAN>is activated while 'Condition' is True.<BR>
    <BR>
    <SPAN CLASS=pin>Condition: </SPAN>condition to evaluate.<BR>
    <BR>
    This behavior activates the 'Loop Out' output while the 'condition' is TRUE.<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x010015a6, 0xef597665));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateWhileProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateWhileProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("While");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Loop In");

    proto->DeclareOutput("Out");
    proto->DeclareOutput("Loop Out");

    proto->DeclareInParameter("Condition", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(While);

    *pproto = proto;

    return CK_OK;
}

int While(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(0))
        beh->ActivateInput(0, FALSE);
    else if (beh->IsInputActive(1))
        beh->ActivateInput(1, FALSE);

    CKBOOL k = TRUE;
    beh->GetInputParameterValue(0, &k);

    if (k)
        beh->ActivateOutput(1);
    else
        beh->ActivateOutput(0);

    return CKBR_OK;
}
