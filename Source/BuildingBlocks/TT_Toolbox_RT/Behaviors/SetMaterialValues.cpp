//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_Set Material Values
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSetMaterialValuesDecl();
CKERROR CreateSetMaterialValuesProto(CKBehaviorPrototype **pproto);
int SetMaterialValues(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorSetMaterialValuesDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Set Material Values");
	od->SetDescription("Sets all material values of the target material (Ambient, Specular, etc. ) using the values of another material.");
	od->SetCategory("TT Toolbox/Material");
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x62963fa9,0x7258745c));
	od->SetAuthorGuid(TERRATOOLS_GUID);
	od->SetAuthorName("Terratools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSetMaterialValuesProto);
	od->SetCompatibleClassId(CKCID_MATERIAL);
	return od;
}

CKERROR CreateSetMaterialValuesProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Set Material Values");
	if(!proto) return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");

	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Get Material", CKPGUID_MATERIAL);

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(SetMaterialValues);

	proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

	*pproto = proto;
	return CK_OK;
}

int SetMaterialValues(const CKBehaviorContext& behcontext)
{
	CKBehavior* beh = behcontext.Behavior;
	
	CKMaterial *targetMat = (CKMaterial *)beh->GetTarget();
	CKMaterial *srcMat = (CKMaterial *)beh->GetInputParameterObject(0);
	
	if (!targetMat || !srcMat)
		return CKBR_PARAMETERERROR;
	
	// Copy all material properties
	targetMat->SetAmbient(srcMat->GetAmbient());
	targetMat->SetDiffuse(srcMat->GetDiffuse());
	targetMat->SetEmissive(srcMat->GetEmissive());
	targetMat->SetPower(srcMat->GetPower());
	targetMat->SetSpecular(srcMat->GetSpecular());
	
	// Texture settings
	targetMat->SetTexture0(srcMat->GetTexture());
	targetMat->SetTextureBlendMode(srcMat->GetTextureBlendMode());
	targetMat->SetTextureMinMode(srcMat->GetTextureMinMode());
	targetMat->SetTextureMagMode(srcMat->GetTextureMagMode());
	targetMat->SetTextureAddressMode(srcMat->GetTextureAddressMode());
	targetMat->SetTextureBorderColor(srcMat->GetTextureBorderColor());
	
	// Rendering settings
	targetMat->EnablePerspectiveCorrection(srcMat->PerspectiveCorrectionEnabled());
	targetMat->SetSourceBlend(srcMat->GetSourceBlend());
	targetMat->SetDestBlend(srcMat->GetDestBlend());
	targetMat->EnableAlphaBlend(srcMat->AlphaBlendEnabled());
	targetMat->SetTwoSided(srcMat->IsTwoSided());
	targetMat->EnableZWrite(srcMat->ZWriteEnabled());
	targetMat->SetZFunc(srcMat->GetZFunc());
	targetMat->SetFillMode(srcMat->GetFillMode());
	targetMat->SetShadeMode(srcMat->GetShadeMode());
	targetMat->EnableAlphaTest(srcMat->AlphaTestEnabled());
	targetMat->SetAlphaFunc(srcMat->GetAlphaFunc());
	targetMat->SetAlphaRef(srcMat->GetAlphaRef());
	
	beh->ActivateInput(0, FALSE);
	beh->ActivateOutput(0, TRUE);
	return CKBR_OK;
}