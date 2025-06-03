/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Draw Rectangle
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "CKFontManager.h"

CKObjectDeclaration *FillBehaviorDrawRectangleDecl();
CKERROR CreateDrawRectangleProto(CKBehaviorPrototype **);
int DrawRectangle(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDrawRectangleDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("Draw Rectangle");
	od->SetDescription("Draws a rectangle and a border at a specified position.");
	/* rem:
	<SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
	<SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
	<BR>
	<SPAN CLASS=pin>Interior: </SPAN>should the interior of the rectangle be drawn.<BR>
	<SPAN CLASS=pin>Color: </SPAN>color of the rectangle.<BR>
	<SPAN CLASS=pin>Material: </SPAN>material to use for the rectangle rendering.<BR>
	<SPAN CLASS=pin>Screen Coordinates: </SPAN>position and size of the rectangle on screen.<BR>
	<SPAN CLASS=pin>Texture Coordinates: </SPAN>texture coordinates of the rectangle.<BR>
	<SPAN CLASS=pin>Border: </SPAN>should the border of the rectangle be drawn.<BR>
	<SPAN CLASS=pin>Border Color: </SPAN>color of the border.<BR>
	<SPAN CLASS=pin>Border Size: </SPAN>size of the border, in screen coordinates.<BR>
	*/
	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	od->SetGuid(CKGUID(0x4095766f, 0x291161c1));
	od->SetAuthorGuid(VIRTOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);
	od->SetCreationFunction(CreateDrawRectangleProto);
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	od->SetCategory("Interface/Primitives");
	return od;
}

CKERROR CreateDrawRectangleProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Draw Rectangle");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	proto->DeclareInput("In");
	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Interior", CKPGUID_BOOL, "TRUE");
	proto->DeclareInParameter("Color", CKPGUID_COLOR, "255,255,255,255");
	proto->DeclareInParameter("Material", CKPGUID_MATERIAL);
	proto->DeclareInParameter("Screen Coordinates", CKPGUID_RECT, "(10,10),(100,100)");
	proto->DeclareInParameter("Texture Coordinates", CKPGUID_RECT, "(0,0),(1,1)");
	proto->DeclareInParameter("Border", CKPGUID_BOOL, "FALSE");
	proto->DeclareInParameter("Border Color", CKPGUID_COLOR, "255,255,255,255");
	proto->DeclareInParameter("Border Size", CKPGUID_FLOAT, "1.0");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetFunction(DrawRectangle);

	*pproto = proto;
	return CK_OK;
}

int DrawRectangle(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	CKContext *ctx = behcontext.Context;

	beh->ActivateInput(0, FALSE);
	beh->ActivateOutput(0);

	CKFontManager *fm = (CKFontManager *)ctx->GetManagerByGuid(FONT_MANAGER_GUID);

	///
	// Reading Inputs

	CKBOOL in = TRUE;
	beh->GetInputParameterValue(0, &in);

	VxColor col;
	beh->GetInputParameterValue(1, &col);

	CKMaterial *mat = (CKMaterial *)beh->GetInputParameterObject(2);

	VxRect screen;
	beh->GetInputParameterValue(3, &screen);

	VxRect uvs(0, 0, 1.0f, 1.0f);
	beh->GetInputParameterValue(4, &uvs);

	CKBOOL border = FALSE;
	beh->GetInputParameterValue(5, &border);

	if (!in && !border)
		return CKBR_OK;

	VxColor bcol;
	beh->GetInputParameterValue(6, &bcol);

	float bsize = 0.0f;
	beh->GetInputParameterValue(7, &bsize);

	// Filling of the structure
	RectangleData rd(in, col.GetRGBA(), mat, screen, uvs, border, bcol.GetRGBA(), bsize);

	// The call to the font manager
	fm->DrawRectangle(rd);

	return CKBR_OK;
}
