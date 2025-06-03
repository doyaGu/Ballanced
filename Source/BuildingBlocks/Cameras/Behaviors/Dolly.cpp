/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Dolly Camera
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDollyDecl();
CKERROR CreateDollyProto(CKBehaviorPrototype **pproto);
int Dolly(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDollyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Dolly");
    od->SetDescription("Keeps the projection plane at the same position while translating the camera on its Z local axis.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Translation Value: </SPAN>translation value on the Camera Z local axis.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    Keeps the projection plane at the same position while translating the camera on its Z local axis.<BR>
    */
    od->SetCategory("Cameras/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xb98b83bb, 0x87787878));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDollyProto);
    od->SetCompatibleClassId(CKCID_TARGETCAMERA);
    return od;
}

CKERROR CreateDollyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Dolly");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Translation Value", CKPGUID_FLOAT, "1.0");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Dolly);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Dolly(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTargetCamera *tcam = (CKTargetCamera *)beh->GetTarget();
    if (!tcam)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get Translation value
    float value = 1.0f;
    beh->GetInputParameterValue(0, &value);

    // Hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1, &k);
    k = !k;

    tcam->Translate3f(0.0f, 0.0f, value, tcam, k);

    return CKBR_OK;
}
