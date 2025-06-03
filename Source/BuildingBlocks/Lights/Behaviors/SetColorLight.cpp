/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetColorLight
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetColorLightDecl();
CKERROR CreateSetColorLightProto(CKBehaviorPrototype **);
int SetColorLight(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetColorLightDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Light Color");
    od->SetDescription("Sets the Color of a Light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Color : </SPAN>color expressed in RGBA.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x32115590, 0x78951230));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetColorLightProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetColorLightProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Light Color");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetColorLight);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetColorLight(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    VxColor color;
    beh->GetInputParameterValue(0, &color);

    light->SetColor(color);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
