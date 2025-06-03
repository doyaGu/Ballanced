/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetFalloff
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetFalloffDecl();
CKERROR CreateSetFalloffProto(CKBehaviorPrototype **);
int SetFalloff(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetFalloffDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Spot Light Properties");
    od->SetDescription("Sets the properties of a spotlight (internal and external diameter of the light cone).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Fall Off: </SPAN>external angle of the light cone.<BR>
    <SPAN CLASS=pin>Fall Off Shape: </SPAN>shape of the attenuation of the light cone (linear=1.0, fade with an asymptote at 0 = 5.0, fade with an asymptote to 1.0 = 0.2 ... etc).<BR>
    <SPAN CLASS=pin>Hot Spot: </SPAN>inner angle of the light cone.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x01ed010b, 0x01fd010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetFalloffProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetFalloffProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Spot Light Properties");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Fall Off", CKPGUID_ANGLE, "0:50");
    proto->DeclareInParameter("Fall Off Shape", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Hot Spot", CKPGUID_ANGLE, "0:40");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetFalloff);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetFalloff(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    // FallOff
    float falloff = 0.87f;
    beh->GetInputParameterValue(0, &falloff);
    light->SetFallOff(falloff);

    // FallOff SHape
    float shape = 1.0f;
    beh->GetInputParameterValue(1, &shape);
    light->SetFallOffShape(shape);

    // Hot Spot
    float hotspot = 0.7f;
    beh->GetInputParameterValue(2, &hotspot);
    light->SetHotSpot(hotspot);

    return CKBR_OK;
}
