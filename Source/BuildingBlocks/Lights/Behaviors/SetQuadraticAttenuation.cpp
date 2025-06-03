/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetQuadraticAttenuation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetQuadraticAttenuationDecl();
CKERROR CreateSetQuadraticAttenuationProto(CKBehaviorPrototype **);
int SetQuadraticAttenuation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetQuadraticAttenuationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Quadratic Attenuation");
    od->SetDescription("Sets the value of the 'Quadratic Attenuation' of a Light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Quadratic Attenuation: </SPAN>positive float value.<BR>
    <BR>
    Useful for flashing lights: set the constant to a high value like 10.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x0a0e0f18, 0x7a01ef09));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetQuadraticAttenuationProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetQuadraticAttenuationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Quadratic Attenuation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Quadratic Attenuation", CKPGUID_FLOAT, "0.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetQuadraticAttenuation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetQuadraticAttenuation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    float value = 0.0f;
    beh->GetInputParameterValue(0, &value);

    light->SetQuadraticAttenuation(value);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
