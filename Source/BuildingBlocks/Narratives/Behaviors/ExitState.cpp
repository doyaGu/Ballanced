/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ExitState
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorExitStateDecl();
CKERROR CreateExitStateProto(CKBehaviorPrototype **);
int ExitState(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorExitStateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Deactivate Global State");
    od->SetDescription("Exit the system from the given global state.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>State : </SPAN>global state that must be deactivate.<BR>
    <BR>
    States are only boolean values belonging to Levels, and shared by every object and script.<BR>
    Use 'Is Global State Active' to test if the system is in a specific global state.<BR>
    <BR>
    See also : Is Global State Active, Activate Global State.<BR>
    */
    od->SetCategory("Narratives/States");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x520d14a5, 0x797502ae));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExitStateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateExitStateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Deactivate Global State");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("State", CKPGUID_STATE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ExitState);

    *pproto = proto;
    return CK_OK;
}

int ExitState(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    CKStateObject *rdv = (CKStateObject *)beh->GetInputParameterObject(0);
    if (!rdv)
        throw "You didn't provide a valide State...";
    rdv->LeaveState();

    return CKBR_OK;
}
