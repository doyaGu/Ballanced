/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Lit Mode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetLitModeDecl();
CKERROR CreateSetLitModeProto(CKBehaviorPrototype **);
int SetLitMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLitModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Lighting Mode");
    od->SetDescription("Sets the Lit mode of a mesh (lighting / no lighting).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Lighting Mode: </SPAN>PreLitMesh (no light calculation will be performed for this mesh) / LitMesh (perform light calculation).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x45899665, 0x45211254));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLitModeProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetLitModeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Lighting Mode");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Lighting Mode", CKPGUID_LITMODE, "Prelit");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLitMode);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetLitMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    CKMesh *mesh = ent->GetCurrentMesh();
    if (!mesh)
        return CKBR_PARAMETERERROR;

    int mode = VX_PRELITMESH;
    beh->GetInputParameterValue(0, &mode);

    mesh->SetLitMode((VXMESH_LITMODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
