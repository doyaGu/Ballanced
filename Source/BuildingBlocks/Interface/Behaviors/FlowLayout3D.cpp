/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FlowLayout3D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "Layout.h"

CKObjectDeclaration *FillBehaviorFlowLayout3DDecl();
CKERROR CreateFlowLayout3DProto(CKBehaviorPrototype **pproto);
int FlowLayout3D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorFlowLayout3DDecl()
{
	CKObjectDeclaration *od = CreateCKObjectDeclaration("3D Flow Layout");
	od->SetDescription("Organize a set of 3D entities belong a direction in space");
	/* rem:
	<SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
	<SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
	<BR>
	<SPAN CLASS=pin>Model: </SPAN>3D Entity determining the position and direction of the flow.<BR>
	<SPAN CLASS=pin>Flow Direction:</SPAN>on which axis of the target reference the flow must lie.<BR>
	<SPAN CLASS=pin>Flow Alignment: </SPAN>alignment to use / target.<BR>
	<SPAN CLASS=pin>Support: </SPAN>select depth start of the flow / target.<BR>
	<SPAN CLASS=pin>Top Margin: </SPAN>margin from the top of the target.<BR>
	<SPAN CLASS=pin>Side Margin: </SPAN>margin from the left or right side of the target (depends on alignment).<BR>
	<SPAN CLASS=pin>Offset Margin: </SPAN>margin from the XY plan of the target (depends on alignment).<BR>
	<SPAN CLASS=pin>Gap Size: </SPAN>space between the displaced entities.<BR>
	<SPAN CLASS=pin>Keep Children?: </SPAN>if checked, displace the children of the entities in group.<BR>
	<BR>
	*/

	// Category in Virtools interface
	od->SetCategory("Interface/Layouts");

	od->SetType(CKDLL_BEHAVIORPROTOTYPE);
	// Unique identifier of this prototype
	od->SetGuid(CKGUID(0x245fb72c, 0xa5e437e6));

	od->SetAuthorGuid(VIRTOOLS_GUID);
	od->SetAuthorName("Virtools");
	od->SetVersion(0x00010000);

	// Function that will create the behavior prototype
	od->SetCreationFunction(CreateFlowLayout3DProto);
	// Class ID of the objects to which the behavior can apply
	od->SetCompatibleClassId(CKCID_GROUP);
	return od;
}

//-------------------------------------------------
// FlowLayout3D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateFlowLayout3DProto(CKBehaviorPrototype **pproto)
{
	CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("3D Flow Layout");
	if (!proto)
		return CKERR_OUTOFMEMORY;

	// Input/Outputs declaration
	proto->DeclareInput("In");
	proto->DeclareOutput("Out");

	proto->DeclareInParameter("Model", CKPGUID_3DENTITY);
	proto->DeclareInParameter("Flow Direction", CKPGUID_FLOW_DIRECTION, "Left/Right");
	proto->DeclareInParameter("Flow Alignment", CKPGUID_FLOW_ALIGNMENT, "Left");
	proto->DeclareInParameter("Support", CKPGUID_FLOW_SUPPORT, "Front");
	proto->DeclareInParameter("Top Margin", CKPGUID_FLOAT, "0.5");
	proto->DeclareInParameter("Side Margin", CKPGUID_FLOAT, "0.5");
	proto->DeclareInParameter("Offset Margin", CKPGUID_FLOAT, "0");
	proto->DeclareInParameter("Gap Size", CKPGUID_FLOAT, "0.5");
	proto->DeclareInParameter("Keep Children ?", CKPGUID_BOOL, "FALSE");

	proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
	proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

	// Function that will be called upon behavior execution
	proto->SetFunction(FlowLayout3D);

	*pproto = proto;
	return CK_OK;
}

#define DIR_LEFT_RIGHT 1
#define DIR_UP_DOWN 2
#define DIR_REAR_FRONT 4

#define ALIGN_LEFT 1
#define ALIGN_CENTER 2
#define ALIGN_RIGHT 4

#define ALIGN_FRONT 1
#define ALIGN_IN 2
#define ALIGN_REAR 4

//-------------------------------------------------
// FlowLayout3D behavior execution function
//
//-------------------------------------------------
int FlowLayout3D(const CKBehaviorContext &behcontext)
{
	CKBehavior *beh = behcontext.Behavior;
	// Deactivate Input
	beh->ActivateInput(0, FALSE);
	beh->ActivateOutput(0);

	// Group of Array
	CKGroup *group = (CKGroup *)beh->GetTarget();
	if (!group)
		return CKBR_OWNERERROR;

	// Check if the group is only formed of 3D entities
	if (!CKIsChildClassOf(group->GetCommonClassID(), CKCID_3DENTITY))
		return CKBR_OK;

	int objectCount = group->GetObjectCount();
	if (objectCount == 0)
		return CKBR_OK;

	// Get the reference 3D frame
	CK3dEntity *rootFrame = (CK3dEntity *)beh->GetInputParameterObject(0);
	if (!rootFrame)
		return CKBR_PARAMETERERROR;

	// Get the layout parameters
	int direction = 1, alignment = ALIGN_LEFT, support = ALIGN_FRONT;
	float topMargin = 0.5, sideMargin = 0.5, offsetMargin = 0.0, gapSize = 0.5;
	CKBOOL keepChildren = FALSE;
	beh->GetInputParameterValue(1, &direction);
	beh->GetInputParameterValue(2, &alignment);
	beh->GetInputParameterValue(3, &support);
	beh->GetInputParameterValue(4, &topMargin);
	beh->GetInputParameterValue(5, &sideMargin);
	beh->GetInputParameterValue(6, &offsetMargin);
	beh->GetInputParameterValue(7, &gapSize);
	beh->GetInputParameterValue(8, &keepChildren);

	// Position of each entity in the group relative to the reference frame
	VxBbox currentBoundingBox;
	VxVector currentPosition;
	VxVector rootDir, rootUp;

	// Position and orientation of the reference frame
	rootFrame->GetPosition(&currentPosition);
	rootFrame->GetOrientation(&rootDir, &rootUp);

	// Dimension of the root frame
	VxVector currentScale;
	rootFrame->GetScale(&currentScale, FALSE);

	currentPosition.y += (currentScale.y - topMargin);
	if (support & ALIGN_FRONT) // Front
		currentPosition.z -= (currentScale.z + offsetMargin);
	else if (support & ALIGN_IN) // In
		currentPosition.z -= offsetMargin;
	else // Rear
		currentPosition.z += (currentScale.z - offsetMargin);

	if (alignment & ALIGN_LEFT) // Left
		currentPosition.x += (sideMargin - currentScale.x);
	else if (alignment & ALIGN_CENTER) // Center
	{
		if (direction & DIR_LEFT_RIGHT) // Left/Right
		{
			int real3DEntity = 0;
			float totalSize = 0.0;
			for (int i = 0; i < objectCount; i++)
			{
				CKBeObject *obj = group->GetObject(i);
				if (CKIsChildClassOf(obj, CKCID_3DENTITY))
				{
					CK3dEntity *ent = (CK3dEntity *)obj;
					const VxMatrix &mat = ent->GetWorldMatrix();
					real3DEntity++;
					// totalSize += currentScale.x ;
					totalSize += Magnitude(mat[0]) * 2.0f; // scale X
				}
			}
			if (real3DEntity == 0)
				return CKBR_OK;
			currentPosition.x -= ((totalSize + (real3DEntity - 1) * gapSize) / 2);
		}
	}
	else // alignment == 3 : Right
		currentPosition.x += (currentScale.x - sideMargin);

	for (int i = 0; i < objectCount; i++)
	{
		CKBeObject *obj = group->GetObject(i);

		if (CKIsChildClassOf(obj, CKCID_3DENTITY))
		{
			CK3dEntity *ent = (CK3dEntity *)obj;
			ent->GetScale(&currentScale, FALSE);

			VxVector objPosition(currentPosition);
			if ((direction & DIR_LEFT_RIGHT) && (alignment & (ALIGN_LEFT | ALIGN_CENTER))) // (Left or Center) and Left/Right
			{
				objPosition.x += currentScale.x;
			}
			else
			{
				if (alignment & ALIGN_RIGHT) // Right
					objPosition.x -= currentScale.x;
			}
			objPosition.y -= currentScale.y;

			ent->SetPosition(&objPosition, NULL, keepChildren);
			ent->SetOrientation(&rootDir, &rootUp, NULL, NULL, keepChildren);
			ent->SetParent(rootFrame);

			if (direction & DIR_LEFT_RIGHT) // Left/Right
			{
				if (alignment & (ALIGN_LEFT | ALIGN_CENTER)) // Left or Center
					currentPosition.x += (2 * currentScale.x + gapSize);
				else if (alignment & ALIGN_RIGHT) // Right
					currentPosition.x -= (2 * currentScale.x + gapSize);
			}
			else
			{
				if (direction & DIR_UP_DOWN) // Up/Down
					currentPosition.y -= (2 * currentScale.y + gapSize);
				else // direction == 3 Rear/Front
					currentPosition.z -= (2 * currentScale.z + gapSize);
			}
		}
	}

	return CKBR_OK;
}