/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Specular
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetSpecularDecl();
CKERROR CreateSetSpecularProto(CKBehaviorPrototype **);
int SetSpecular(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetSpecularDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Specular");
    od->SetDescription("Sets the Specular Color of a Material");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Specular Color: </SPAN>the specular color (useful to give metalic effects).<BR>
    <SPAN CLASS=pin>Specular Power: </SPAN>positive float value.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x20202020, 0x30303030));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetSpecularProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetSpecularProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Specular");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Specular Color", CKPGUID_COLOR, "0,0,0,255");
    proto->DeclareInParameter("Specular Power", CKPGUID_FLOAT, "1.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetSpecular);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetSpecular(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    VxColor amb(0, 0, 0);
    beh->GetInputParameterValue(0, &amb);

    mat->SetSpecular(amb);

    float power = 1.0f;
    if (!beh->GetInputParameterValue(1, &power))
    {
        mat->SetPower(power);
    }

    return CKBR_OK;
}
