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
	od->SetCompatibleClassId(CKCID_BEOBJECT);
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
	// TODO: To be finished.
	return CKBR_OK;
}