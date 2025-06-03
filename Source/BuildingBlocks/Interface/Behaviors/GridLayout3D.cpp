/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GridLayout3D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "Layout.h"

CKObjectDeclaration *FillBehaviorGridLayout3DDecl();
CKERROR CreateGridLayout3DProto(CKBehaviorPrototype **pproto);
int GridLayout3D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorGridLayout3DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("3D Grid Layout");

    od->SetDescription("Organize a set of 3D entities into a 3D Grid");
    // Category in Virtools interface
    od->SetCategory("Interface/Layouts");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Model: </SPAN>Model determining the position and orientation of the grid. Entities will be
    parented to it.<BR>
    <SPAN CLASS=pin>Std Margin: </SPAN>margins in the axis X,Y to add to the dimension of the target frame.<BR>
    <SPAN CLASS=pin>Front Margin: </SPAN>margins in the axis +Z to add to the dimension of the target frame.<BR>
    <SPAN CLASS=pin>Rear Margin: </SPAN>margins in the axis -Z to add to the dimension of the target frame.<BR>
    <SPAN CLASS=pin>Support: </SPAN>type of alignment, on the Z axis.<BR>
    <SPAN CLASS=pin>Horizontal Gap Size: </SPAN>horizontal space between the entities displaced.<BR>
    <SPAN CLASS=pin>Vertical Gap Size: </SPAN>vertical space between the entities displaced.<BR>
    <SPAN CLASS=pin>Depth Gap Size: </SPAN>depth space between the entities displaced.<BR>
    <BR>
    The target group must contain Arrays, representing a layer of the 3D grid. This array must only contains objects (Basic Type not Parameter).
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x665f342c, 0x9e1366e6));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateGridLayout3DProto);
    // Class ID of the objects to which the behavior can apply
    od->SetCompatibleClassId(CKCID_GROUP);
    return od;
}

//-------------------------------------------------
// GridLayout3D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateGridLayout3DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("3D Grid Layout");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Model", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Std Margins", CKPGUID_RECT, "(1,1),(1,1)");
    proto->DeclareInParameter("Front Margin", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Rear Margin", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Support", CKPGUID_FLOW_SUPPORT, "Front");
    proto->DeclareInParameter("Horizontal Gap Size", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Vertical Gap Size", CKPGUID_FLOAT, "0.5");
    proto->DeclareInParameter("Depth Gap Size", CKPGUID_FLOAT, "0.5");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    // Function that will be called upon behavior execution
    proto->SetFunction(GridLayout3D);

    *pproto = proto;
    return CK_OK;
}

#define FRONT 1
#define G_IN 2
#define REAR 4

//-------------------------------------------------
// GridLayout3D behavior execution function
//
//-------------------------------------------------
int GridLayout3D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Group of Array
    CKGroup *group = (CKGroup *)beh->GetTarget();
    if (!group)
        return CKBR_OWNERERROR;

    // Check if the group is only formed of 3D entities
    if (!CKIsChildClassOf(group->GetCommonClassID(), CKCID_DATAARRAY))
        return CKBR_OK;

    int nbLayer = group->GetObjectCount();
    if (nbLayer == 0)
        return CKBR_OK;

    // Get the reference 3D frame
    CK3dEntity *rootFrame = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (!rootFrame)
        return CKBR_PARAMETERERROR;

    // Get the layout parameters
    int support = 1;
    VxRect margins(1, 1, 1, 1);
    float horizontalGap = 0.5, verticalGap = 0.5;
    float frontMargin = 0.5, rearMargin = 0.5, depthGap = 0.5;
    CKBOOL keepChildren = FALSE;
    beh->GetInputParameterValue(1, &margins);
    beh->GetInputParameterValue(2, &frontMargin);
    beh->GetInputParameterValue(3, &rearMargin);
    beh->GetInputParameterValue(4, &support);
    beh->GetInputParameterValue(5, &horizontalGap);
    beh->GetInputParameterValue(6, &verticalGap);
    beh->GetInputParameterValue(7, &depthGap);

    VxVector currentPosition;
    VxVector rootUp, rootDir;
    VxBbox currentBoundingBox;

    rootFrame->GetPosition(&currentPosition);
    rootFrame->GetOrientation(&rootDir, &rootUp);

    VxVector currentScale;
    rootFrame->GetScale(&currentScale, FALSE);

    currentPosition.x += (margins.left - currentScale.x);
    currentPosition.y += (currentScale.y - margins.top);

    if (support & FRONT) // Front
        currentPosition.z += (frontMargin - currentScale.z * 2);
    else if (support & G_IN) // In
        currentPosition.z += (frontMargin - currentScale.z);
    else // Rear
        currentPosition.z += frontMargin;

    VxVector cellSize, pos(currentPosition);

    for (int l = 0; l < nbLayer; l++)
    {
        CKBeObject *obj = group->GetObject(l);
        if (CKIsChildClassOf(obj, CKCID_DATAARRAY))
        {
            CKDataArray *array = (CKDataArray *)obj;
            int nbRow = array->GetRowCount(), nbColumn = array->GetColumnCount();
            cellSize.x = (currentScale.x - (margins.left + margins.right + (nbColumn - 1) * horizontalGap) / 2) / nbColumn;
            cellSize.y = (currentScale.y - (margins.top + margins.bottom + (nbRow - 1) * verticalGap) / 2) / nbRow;
            cellSize.z = (currentScale.z - (frontMargin + rearMargin + (nbLayer - 1) * depthGap) / 2) / nbLayer;
            for (int i = 0; i < nbRow; i++)
            {
                for (int j = 0; j < nbColumn; j++)
                {
                    CKObject *obj = array->GetElementObject(i, j);
                    if (CKIsChildClassOf(obj, CKCID_3DENTITY))
                    {
                        CK3dEntity *ent = (CK3dEntity *)obj;
                        ent->SetParent(rootFrame);

                        VxVector objPosition(currentPosition);
                        objPosition.x += cellSize.x;
                        objPosition.y -= cellSize.y;
                        objPosition.z += cellSize.z;
                        ent->SetPosition(&objPosition, NULL);

                        ent->SetOrientation(&rootDir, &rootUp, NULL, NULL, keepChildren);

                        // TO change retrieves value from user ?
                        if (FALSE)
                        {
                            VxVector objSize(cellSize);
                            objSize /= currentScale;
                            ent->SetScale(&objSize, TRUE, FALSE);
                        }
                    }
                    currentPosition.x += cellSize.x * 2 + horizontalGap;
                }
                currentPosition.x = pos.x;
                currentPosition.y -= cellSize.y * 2 + verticalGap;
            }
            currentPosition.x = pos.x;
            currentPosition.y = pos.y;
            currentPosition.z += cellSize.z * 2 + depthGap;
        }
    }

    return CKBR_OK;
}
