/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetRangeLight
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetRangeLightDecl();
CKERROR CreateSetRangeLightProto(CKBehaviorPrototype **);
int SetRangeLight(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetRangeLightDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Light Range");
    od->SetDescription("Sets the range of the light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Range: </SPAN>distance from which meshes no longer receive light.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x22252552, 0x00200020));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetRangeLightProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    od->SetCategory("Lights/Basic");
    return od;
}

CKERROR CreateSetRangeLightProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Light Range");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Range", CKPGUID_FLOAT, "50");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetRangeLight);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetRangeLight(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKLight *light = (CKLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    float value = 50.0f;
    beh->GetInputParameterValue(0, &value);

    light->SetRange(value);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
