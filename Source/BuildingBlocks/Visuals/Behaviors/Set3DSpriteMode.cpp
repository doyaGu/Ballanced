/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set3DSpriteMode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSet3DSpriteModeDecl();
CKERROR CreateSet3DSpriteModeProto(CKBehaviorPrototype **pproto);
int Set3DSpriteMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSet3DSpriteModeDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("Set 3DSprite Mode");
	od->SetDescription("Sets the orientation mode of the 3D Sprite.");
	/* rem:
	<SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
	<SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
	<BR>
	<SPAN CLASS=pin>Mode : </SPAN><BR>
	BILLBOARD (completely attached to the camera, and therefore always facing it).<BR>
	XROTATE (always facing when camera rotates around the X local axis of the 3D Sprite)<BR>
	YROTATE (always facing when camera rotates around the Y local axis of the 3D Sprite)<BR>
	ORIENTABLE (completely independent)<BR>
	*/
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x0c5d0457, 0xc64c7000));
	od->SetAuthorGuid(VIRTOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateSet3DSpriteModeProto);
	od->SetCompatibleClassId(CKCID_SPRITE3D);
	od->SetCategory("Visuals/Sprite");
	return od;
}

CKERROR CreateSet3DSpriteModeProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = NULL;
	proto = CreateCKBehaviorPrototype("Set 3DSprite Mode");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");
	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Mode", CKPGUID_3DSPRITEMODE, "XRotate");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(Set3DSpriteMode);
	proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

	*pproto = proto;
	return CK_OK;
}

int Set3DSpriteMode(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;

	CKSprite3D *spr3d = (CKSprite3D *)beh->GetTarget();
	if (!spr3d)
		return CKBR_OWNERERROR;

	int mode;
	beh->GetInputParameterValue(0, &mode);

	spr3d->SetMode((VXSPRITE3D_TYPE)mode);

	beh->ActivateInput(0, FALSE);
	beh->ActivateOutput(0);

	return CKBR_OK;
}
