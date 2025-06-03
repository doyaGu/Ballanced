/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetLinearAttenuation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetLinearAttenuationDecl();
CKERROR CreateSetLinearAttenuationProto(CKBehaviorPrototype **);
int SetLinearAttenuation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLinearAttenuationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Linear Attenuation");
    od->SetDescription("Sets the value of the 'Linear Attenuation' of a Light");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Linear Attenuation: </SPAN>positive float value.<BR>
    <BR>
    Useful for flashing lights: set the constant to a high value like 10.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45895551, 0x59486677));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLinearAttenuationProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetLinearAttenuationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Linear Attenuation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Linear Attenuation", CKPGUID_FLOAT, "0.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetLinearAttenuation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetLinearAttenuation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    float value = 0.0f;
    beh->GetInputParameterValue(0, &value);

    light->SetLinearAttenuation(value);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
