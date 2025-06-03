/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetConstantAttenuation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetConstantAttenuationDecl();
CKERROR CreateSetConstantAttenuationProto(CKBehaviorPrototype **);
int SetConstantAttenuation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetConstantAttenuationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Light Attenuation");
    od->SetDescription("Sets the Constant, Linear and Quadratic Attenuation of a light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Constant Attenuation: </SPAN>positive float value for constant light attenuation.<BR>
    <SPAN CLASS=pin>Linear Attenuation: </SPAN>positive float value for linear light attenuation.<BR>
    <SPAN CLASS=pin>Quadratic Attenuation: </SPAN>positive float value for quadratic light attenuation.<BR>
    <BR>
    This building block is useful when used for flashing lights, in which case a value such as 10 would be appropriate.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xd63d004d, 0xd54d8100));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetConstantAttenuationProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetConstantAttenuationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Light Attenuation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Constant Attenuation", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Linear Attenuation", CKPGUID_FLOAT, "0.0");
    proto->DeclareInParameter("Quadratic Attenuation", CKPGUID_FLOAT, "0.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetConstantAttenuation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetConstantAttenuation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    float constant = light->GetConstantAttenuation();
    beh->GetInputParameterValue(0, &constant);

    float linear = light->GetLinearAttenuation();
    beh->GetInputParameterValue(1, &linear);

    float quadratic = light->GetQuadraticAttenuation();
    beh->GetInputParameterValue(2, &quadratic);

    light->SetConstantAttenuation(constant);
    light->SetLinearAttenuation(linear);
    light->SetQuadraticAttenuation(quadratic);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
