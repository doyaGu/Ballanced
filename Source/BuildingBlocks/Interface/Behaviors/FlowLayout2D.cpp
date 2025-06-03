/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FlowLayout2D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "Layout.h"

CKObjectDeclaration *FillBehaviorFlowLayout2DDecl();
CKERROR CreateFlowLayout2DProto(CKBehaviorPrototype **pproto);
int FlowLayout2D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorFlowLayout2DDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("2D Flow Layout");
	od->SetDescription("Organize a set of 2D entities horizontally or vertically");
	/* rem:
	<SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
	<SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
	<BR>
	<SPAN CLASS=pin>Components: </SPAN>group containing the 2D entities to organize.<BR>
	<SPAN CLASS=pin>Flow Alignment: </SPAN>alignment to use / target.<BR>
	<SPAN CLASS=pin>Top Margin: </SPAN>margin from the top of the target.<BR>
	<SPAN CLASS=pin>Side Margin: </SPAN>margin from the left or right side of the target (depends on alignment).<BR>
	<SPAN CLASS=pin>Gap Size: </SPAN>space between the entities displaced.<BR>
	<SPAN CLASS=pin>Horizontal Flow: </SPAN>if checked, create an horizontal flow else a vertical ones.<BR>
	<SPAN CLASS=pin>Set Parent: </SPAN>if checked, set the target as parent of the entities in group.<BR>
	<BR>

	The target determine the position of the flow.
	*/

	// Category in Virtools interface
	od->SetCategory("Interface/Layouts");

	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	// Unique identifier of this prototype
	od->SetGuid(CKGUID(0x295f342c, 0x95e467e6));

	od->SetAuthorGuid(VIRTOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);

	// Function that will create the behavior prototype
	od->SetCreationFunction(CreateFlowLayout2DProto);
	// Class ID of the objects to which the behavior can applied
	od->SetCompatibleClassId(CKCID_BEOBJECT);
	return od;
}

//-------------------------------------------------
// FlowLayout2D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateFlowLayout2DProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("2D Flow Layout");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	// Input/Outputs declaration
	proto->DeclareInput("In");
	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Components", CKPGUID_GROUP);
	proto->DeclareInParameter("Flow Alignment", CKPGUID_FLOW_ALIGNMENT, "Left");
	proto->DeclareInParameter("Top Margin", CKPGUID_INT, "10");
	proto->DeclareInParameter("Side Margin", CKPGUID_INT, "10");
	proto->DeclareInParameter("Gap Size", CKPGUID_INT, "10");
	proto->DeclareInParameter("Horizontal Flow", CKPGUID_BOOL, "TRUE");
	proto->DeclareInParameter("Set Parent", CKPGUID_BOOL, "FALSE");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

	// Function that will be called upon behavior execution
	proto->SetFunction(FlowLayout2D);

	*pproto = proto;
	return CK_OK;
}

#define LEFT 1
#define CENTER 2
#define RIGHT 4

//-------------------------------------------------
// FlowLayout2D behavior execution function
//
//-------------------------------------------------
int FlowLayout2D(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	beh->ActivateInput(0, FALSE);

	// Get the reference 2D frame
	CK2dEntity *rootFrame = (CK2dEntity *)beh->GetTarget();
	if (!(rootFrame && CKIsChildClassOf(rootFrame, CKCID_2DENTITY)))
		return CKBR_OWNERERROR;

	// Get the group in the flow layout
	CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
	if (!group)
		return CKBR_OK;

	// Get the layout parameters
	int alignment = 1;
	int topMargin = 10, sideMargin = 10, gapSize = 10;
	CKBOOL horizontal = TRUE, setParent = FALSE;
	beh->GetInputParameterValue(1, &alignment);
	beh->GetInputParameterValue(2, &topMargin);
	beh->GetInputParameterValue(3, &sideMargin);
	beh->GetInputParameterValue(4, &gapSize);
	beh->GetInputParameterValue(5, &horizontal);
	beh->GetInputParameterValue(6, &setParent);

	// Position of each entity in the group relative to the reference frame
	Vx2DVector currentPosition, currentSize;
	rootFrame->GetPosition(currentPosition);
	rootFrame->GetSize(currentSize);

	int objectCount = group->GetObjectCount();

	if (alignment & LEFT) // Left
		currentPosition.x += sideMargin;
	else if (alignment & CENTER) // Center
	{
		currentPosition.x += currentSize.x / 2;
		if (horizontal)
		{
			int real2DEntity = 0;
			float totalSize = 0;
			for (int i = 0; i < objectCount; i++)
			{
				CKBeObject *obj = group->GetObject(i);
				if (CKIsChildClassOf(obj, CKCID_2DENTITY))
				{
					CK2dEntity *ent = (CK2dEntity *)obj;
					ent->GetSize(currentSize);
					real2DEntity++;
					totalSize += currentSize.x;
				}
			}
			currentPosition.x -= (totalSize + (real2DEntity - 1) * gapSize) / 2;
		}
	}
	else // Right
		currentPosition.x += (currentSize.x - sideMargin);

	currentPosition.y += topMargin;

	for (int i = 0; i < objectCount; i++)
	{
		CKBeObject *obj = group->GetObject(i);
		if (CKIsChildClassOf(obj, CKCID_2DENTITY))
		{
			CK2dEntity *ent = (CK2dEntity *)obj;

			ent->GetSize(currentSize);
			Vx2DVector objPosition(currentPosition);
			if (!horizontal && (alignment & CENTER)) // Center vertical
				objPosition.x -= currentSize.x / 2;
			else if (alignment & RIGHT) // Right
				objPosition.x -= currentSize.x;
			ent->SetPosition(objPosition);
			if (setParent)
				ent->SetParent(rootFrame);
			if (horizontal)
			{
				if (alignment & (LEFT | CENTER)) // Left or center
					currentPosition.x += (currentSize.x + gapSize);
				else // Right
					currentPosition.x -= (currentSize.x + gapSize);
			}
			else // Vertical
			{
				currentPosition.y += (currentSize.y + gapSize);
			}
		}
	}

	// 'Out'
	beh->ActivateOutput(0);
	return CKBR_OK;
}
