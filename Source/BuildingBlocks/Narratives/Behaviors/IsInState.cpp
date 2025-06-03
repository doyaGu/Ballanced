/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Is In State
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorIsInStateDecl();
CKERROR CreateIsInStateProto(CKBehaviorPrototype **);
int IsInState(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorIsInStateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Is Global State Active");
    od->SetDescription("Checks to see if the system is in a specified global state.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>The 3D Entity is in the given state.<BR>
    <SPAN CLASS=out>False: </SPAN>The 3D Entity is not in the given state.<BR>
    <BR>
    <SPAN CLASS=pin>State: </SPAN>the global state name.<BR>
    <BR>
    Global states are only boolean values belonging to Levels, and shared by every object and script.<BR>
    <BR>
    See also: Activate Global State, Deactivate Global State.<BR>
    */
    od->SetCategory("Narratives/States");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4e65bdd, 0x473c3681));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateIsInStateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateIsInStateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Is Global State Active");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("State", CKPGUID_STATE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(IsInState);

    *pproto = proto;
    return CK_OK;
}

int IsInState(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    CKStateObject *rdv = (CKStateObject *)beh->GetInputParameterObject(0);

    if (!rdv)
    {
        throw "You didn't provide a valid State...";
        return CKBR_OK;
    }

    if (rdv->IsStateActive())
    {
        beh->ActivateOutput(0);
    }
    else
    {
        beh->ActivateOutput(1);
    }

    return CKBR_OK;
}
