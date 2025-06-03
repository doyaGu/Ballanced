/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            EnterCriticalSection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorEnterCriticalSectionDecl();
CKERROR CreateEnterCriticalSectionProto(CKBehaviorPrototype **);
int EnterCriticalSection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorEnterCriticalSectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Enter Critical Section");
    od->SetDescription("Makes the object enter a critical section.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Critical Section: </SPAN>the critical section name.<BR>
    <BR>
    The output is activated only when the object has entered the critical section.
    Critical sections can be accessed by only one object at a time.
    Therefore the building block will be blocked until the critical section becomes free again.<BR>
    */
    /* warning:
    - If you use a target pIn for the "Enter Critical Section" BB, be sure NOT TO MODIFY this
    target if the "Enter Critical Section" BB is activated. You should only change the target
    (if you need to do so) only before entering by "In" input, or after exiting by "Out" output...
    But never while the building block is internally looping.<BR>
    */
    od->SetCategory("Logics/Synchro");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3c442002, 0x5988257c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateEnterCriticalSectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateEnterCriticalSectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Enter Critical Section");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("CriticalSection", CKPGUID_CRITICALSECTION);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(EnterCriticalSection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int EnterCriticalSection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OK;

    beh->ActivateInput(0, FALSE);
    CKCriticalSectionObject *rdv = (CKCriticalSectionObject *)beh->GetInputParameterObject(0);
    if (!rdv)
        return CKBR_OK;

    if (rdv->EnterCriticalSection(beo))
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
