/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetLightTarget
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetLightTargetDecl();
CKERROR CreateSetLightTargetProto(CKBehaviorPrototype **);
int SetLightTarget(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLightTargetDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Light Target");
    od->SetDescription("Makes a 3D Entity become the target of a light.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Target : </SPAN>3D Entity to target.<BR>
    <BR>
    This building block doesn't need to be looped. (ie: the light will constantly follow the selected object)<BR>
    */
    /* warning:
    - this building block is valid only with a spotlight.<BR>
  - if you delete the light, the target will be deleted.<BR>
    */
    od->SetCategory("Lights/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x02580258, 0x02580258));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLightTargetProto);
    od->SetCompatibleClassId(CKCID_LIGHT);
    return od;
}

CKERROR CreateSetLightTargetProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Light Target");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Target", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLightTarget);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetLightTarget(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTargetLight *light = (CKTargetLight *)beh->GetTarget();
    if (!light)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get target
    CK3dEntity *ent = (CK3dEntity *)beh->GetInputParameterObject(0);

    if (ent && (ent->GetClassID() == CKCID_3DENTITY))
    {
        light->SetTarget(ent);
    }
    else
    {
        light->SetTarget(NULL);
    }

    return CKBR_OK;
}
