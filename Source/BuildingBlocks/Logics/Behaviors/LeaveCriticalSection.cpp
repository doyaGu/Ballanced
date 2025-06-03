/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Leave Critical Section
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLeaveCriticalSectionDecl();
CKERROR CreateLeaveCriticalSectionProto(CKBehaviorPrototype **);
int LeaveCriticalSection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLeaveCriticalSectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Leave Critical Section");
    od->SetDescription("Makes the attached object leave the critical section.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Critical Section : </SPAN>the critical section name.<BR>
    <BR>
    When the object leaves the Critical Section, the Critical Section becomes accessible for the others.
    Only one object can be in the Critical Section at the same time.
    */
    od->SetCategory("Logics/Synchro");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45db3138, 0x4db80f47));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLeaveCriticalSectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLeaveCriticalSectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Leave Critical Section");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Critical Section", CKPGUID_CRITICALSECTION);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LeaveCriticalSection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int LeaveCriticalSection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OK;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);
    CKCriticalSectionObject *rdv = (CKCriticalSectionObject *)beh->GetInputParameterObject(0);
    if (!rdv)
        throw "You didn(t provide a Critical Section...";
    rdv->LeaveCriticalSection(beo);

    return CKBR_OK;
}
