/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Power
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetPowerDecl();
CKERROR CreateSetPowerProto(CKBehaviorPrototype **);
int SetPower(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPowerDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Specular Power");
    od->SetDescription("Sets the Power value of the Material (specular shape of the reflected light).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Power: </SPAN>positive float value (Typically between 0 and 1).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45454545, 0x15151515));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPowerProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetPowerProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Specular Power");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Power", CKPGUID_FLOAT, "0.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetPower);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetPower(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    float power = 0.0f;
    beh->GetInputParameterValue(0, &power);

    mat->SetPower(power);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
