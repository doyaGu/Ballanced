/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetMaterialSprite3D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetMaterialSprite3DDecl();
CKERROR CreateSetMaterialSprite3DProto(CKBehaviorPrototype **);
int SetMaterialSprite3D(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetMaterialSprite3DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Material 3DSprite");
    od->SetDescription("Sets the material of the 3D Sprite.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Material: </SPAN>material for the 3D Sprite.<BR>
    */
    /* warning:
    This behavior is equivalent to using the behavior 'Remove From Group' with a 'Target Parameter'.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3ddd6d33, 0xa54d5c48));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetMaterialSprite3DProto);
    od->SetCompatibleClassId(CKCID_SPRITE3D);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetMaterialSprite3DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Material 3DSprite");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetMaterialSprite3D);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetMaterialSprite3D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSprite3D *spr3d = (CKSprite3D *)beh->GetTarget();
    if (!spr3d)
        return CKBR_OWNERERROR;

    CKMaterial *material = (CKMaterial *)beh->GetInputParameterObject(0);

    spr3d->SetMaterial(material);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
