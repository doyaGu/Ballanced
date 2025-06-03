/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Material
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetMaterialDecl();
CKERROR CreateSetMaterialProto(CKBehaviorPrototype **);
int SetMaterial(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetMaterialDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Material");
    od->SetDescription("Sets the material of the 3D Entity (or replace an existing one).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>material to be apply to the 3D Entity's current mesh.<BR>
    <SPAN CLASS=pin>Material To Replace: </SPAN>material to be replaced. If no "Material To Replace" is specified the first material is applied to the whole mesh.<BR>
    <BR>
    */
    /* warning:
    - The "Set Material" building block can also be used to change the material of a 3D Sprite.<BR>
    - If no "Material To Replace" is specified the first material is applied to the whole mesh.<BR>
    */
    od->SetCategory("Materials-Textures/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x37865d50, 0x2e285fab));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMaterialProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetMaterialProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Material");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
    proto->DeclareInParameter("Material to replace", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetMaterial);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetMaterial(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get Material
    CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(0);

    switch (ent->GetClassID())
    {
    case CKCID_SPRITE3D:
        ((CKSprite3D *)ent)->SetMaterial(mat);
        break;
    default:
        // Capture the corresponding mesh
        CKMesh *mesh = ent->GetCurrentMesh();
        if (!mesh)
            return CKBR_OK;

        // Get Material to replace
        CKMaterial *replace_mat = (CKMaterial *)beh->GetInputParameterObject(1);
        if (!replace_mat)
            mesh->ApplyGlobalMaterial(mat);
        else
            mesh->ReplaceMaterial(replace_mat, mat);
    }

    return CKBR_OK;
}
