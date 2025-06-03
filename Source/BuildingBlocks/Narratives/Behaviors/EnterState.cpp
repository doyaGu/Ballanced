/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            EnterState
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorEnterStateDecl();
CKERROR CreateEnterStateProto(CKBehaviorPrototype **);
int EnterState(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorEnterStateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Global State");
    od->SetDescription("Enters the system in the given global state.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>State : </SPAN>the global state name.<BR>
    <BR>
    States are only boolean values belonging to Levels, and shared by every object and script.<BR>
    Use 'Is Global State Active' to test if the system is in a specific global state.<BR>
    <BR>
    See also : Is Global State Active, Deactivate Global State.<BR>
    */
    od->SetCategory("Narratives/States");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x373a615b, 0x769a6d53));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateEnterStateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateEnterStateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Activate Global State");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("State", CKPGUID_STATE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(EnterState);

    *pproto = proto;
    return CK_OK;
}

int EnterState(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKStateObject *rdv = (CKStateObject *)beh->GetInputParameterObject(0);
    if (!rdv)
        throw "You didn't provide a State...";
    rdv->EnterState();

    return CKBR_OK;
}
