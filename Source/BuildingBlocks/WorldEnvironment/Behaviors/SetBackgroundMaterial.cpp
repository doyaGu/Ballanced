/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetBackgroundMaterial
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetBackgroundMaterialDecl();
CKERROR CreateSetBackgroundMaterialProto(CKBehaviorPrototype **);
int SetBackgroundMaterial(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetBackgroundMaterialDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Background Material");
    od->SetDescription("Sets the Background Material");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Background Material: </SPAN>material to use as the background material.<BR>
    <BR>
    Use this buiding block to easly put in background the same material than the one applied to a 3D Entity.
    This behavior can be applied to any object in the enviroment.<BR>
    */
    od->SetCategory("World Environments/Background");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xddccefec, 0xd010c102));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetBackgroundMaterialProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSetBackgroundMaterialProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Background Material");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Background Material", CKPGUID_MATERIAL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetBackgroundMaterial);

    *pproto = proto;
    return CK_OK;
}

int SetBackgroundMaterial(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get material
    CKMaterial *newmat = (CKMaterial *)beh->GetInputParameterObject(0);
    CKRenderContext *rcontext = behcontext.CurrentRenderContext;
    CKMaterial *mat = rcontext->GetBackgroundMaterial();

    if (mat && newmat)
    {
        mat->SetPower(newmat->GetPower());
        mat->SetAmbient(newmat->GetAmbient());
        mat->SetDiffuse(newmat->GetDiffuse());
        mat->SetSpecular(newmat->GetSpecular());
        mat->SetEmissive(newmat->GetEmissive());

        mat->SetTexture0(newmat->GetTexture());

        mat->SetTextureBlendMode(newmat->GetTextureBlendMode());

        mat->SetTextureMinMode(newmat->GetTextureMinMode());

        mat->SetTextureMagMode(newmat->GetTextureMagMode());

        mat->SetTextureAddressMode(newmat->GetTextureAddressMode());

        mat->SetTextureBorderColor(newmat->GetTextureBorderColor());

        mat->SetSourceBlend(newmat->GetSourceBlend());

        mat->SetDestBlend(newmat->GetDestBlend());

        mat->SetTwoSided(newmat->IsTwoSided());

        mat->SetFillMode(newmat->GetFillMode());

        mat->SetShadeMode(newmat->GetShadeMode());

        mat->SetShadeMode(newmat->GetShadeMode());
    }

    return CKBR_OK;
}
