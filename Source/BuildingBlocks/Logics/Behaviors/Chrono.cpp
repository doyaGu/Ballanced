/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Chrono
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorChronoDecl();
CKERROR CreateChronoProto(CKBehaviorPrototype **);
int Chrono(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChronoDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Chrono");
    od->SetDescription("Chronometer (ON=Start, OFF=Stop).");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated when the "On" input is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated when the needs to loop.<BR>
    <BR>
    <SPAN CLASS=pout>Elapsed Time: </SPAN>current time elapsed since the building block has been activated by ON.<BR>
    <BR>
    When activated by ON, the "Elapsed Time" output parameter is set to 0.<BR>
    */
    od->SetCategory("Logics/Loops");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5e2f1788, 0x4f965a44));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChronoProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateChronoProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Chrono");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareOutParameter("Elapsed Time", CKPGUID_TIME, "0m 0s 0ms");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Chrono);

    *pproto = proto;
    return CK_OK;
}

int Chrono(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    float elapsed = 0;

    if (beh->IsInputActive(0))
    { // ON
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        beh->SetOutputParameterValue(0, &elapsed);
    }

    beh->GetOutputParameterValue(0, &elapsed);
    elapsed += behcontext.TimeManager->GetLastDeltaTimeFree();
    beh->SetOutputParameterValue(0, &elapsed);

    if (beh->IsInputActive(1))
    { // OFF
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
