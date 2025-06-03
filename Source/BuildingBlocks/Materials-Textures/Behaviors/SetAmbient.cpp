/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Ambient
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetAmbientDecl();
CKERROR CreateSetAmbientProto(CKBehaviorPrototype **);
int SetAmbient(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetAmbientDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Ambient");
    od->SetDescription("Sets the Ambient Color of a Material.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Ambient Color: </SPAN>ambient color.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xaaaaaaaa, 0xaeae1287));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetAmbientProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetAmbientProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Ambient");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Ambient Color", CKPGUID_COLOR, "0,0,0,255");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetAmbient);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetAmbient(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    VxColor amb(0, 0, 0);
    beh->GetInputParameterValue(0, &amb);

    mat->SetAmbient(amb);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
