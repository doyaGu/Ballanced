/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetTypeLight
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetTypeLightDecl();
CKERROR CreateSetTypeLightProto(CKBehaviorPrototype **);
int SetTypeLight(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetTypeLightDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Light Type");
    od->SetDescription("Sets the type of a Light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Light Type: </SPAN>Point, Spot, Directional, Parallel (approximation of a point light, with faster calculation).<BR>
    <BR>
  */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32165a2f, 0x70000000));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetTypeLightProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetTypeLightProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Light Type");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Light Type", CKPGUID_LIGHTTYPE, "Spot");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetTypeLight);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetTypeLight(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    int type = 2;
    beh->GetInputParameterValue(0, &type);

    light->SetType((VXLIGHT_TYPE)type);

    return CKBR_OK;
}
