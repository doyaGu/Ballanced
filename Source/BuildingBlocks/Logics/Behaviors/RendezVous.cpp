/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Wait Message
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRendezVousDecl();
CKERROR CreateRendezVousProto(CKBehaviorPrototype **);
int RendezVous(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRendezVousDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Rendezvous");
    od->SetDescription("Waits for a group of objects/characters expected to be at the rendez-vous to all be present in order to allow them to pass through. You need to choose a Meeting Name and a Number of participants.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>RendezVous (1): </SPAN>a meeting name.<BR>
    <SPAN CLASS=pin>RendezVous (2): </SPAN>number of participants.<BR>
    <BR>
    <SPAN CLASS=pout>Number already arrived : </SPAN>number of objects arrived to the rendez-vous.<BR>
    <BR>
    This building block is to be applied on each participant.<BR>
    If the given 'Number of participants' is 5, each participant will be blocked until they are 5. (if they are only 4... the building block will still loop internally until they are 5...)<BR>
    <BR>
    */
    od->SetCategory("Logics/Synchro");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3a1a16c7, 0x20095c45));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRendezVousProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRendezVousProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Rendezvous");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Rendezvous", CKPGUID_SYNCHRO);

    proto->DeclareOutParameter("Number already arrived", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RendezVous);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RendezVous(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBeObject *beo = (CKBeObject *)beh->GetTarget();
    if (!beo)
        return CKBR_OK;

    beh->ActivateInput(0, FALSE);
    CKSynchroObject *rdv = (CKSynchroObject *)beh->GetInputParameterObject(0);
    if (!rdv)
        throw "You didn't provide a valid Rendez Vous";
    int arr = rdv->GetRendezVousNumberOfArrivedObjects();
    beh->SetOutputParameterValue(0, &arr);
    if (rdv->CanIPassRendezVous(beo))
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
