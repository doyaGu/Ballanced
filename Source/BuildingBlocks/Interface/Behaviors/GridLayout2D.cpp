/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            GridLayout2D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "Layout.h"

CKObjectDeclaration *FillBehaviorGridLayout2DDecl();
CKERROR CreateGridLayout2DProto(CKBehaviorPrototype **pproto);
int GridLayout2D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorGridLayout2DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("2D Grid Layout");

    od->SetDescription("Organize as a grid a set of 2D entities into a 2D Frame");
    // Category in Virtools interface
    od->SetCategory("Interface/Layouts");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Components Array: </SPAN>array containing only objects (Base Type only, not Parameter).<BR>
    <SPAN CLASS=pin>Margin: </SPAN>margins to add to the dimension of the target frame.<BR>
    <SPAN CLASS=pin>Horizontal Gap Size: </SPAN>horizontal space between the entities displaced.<BR>
    <SPAN CLASS=pin>Vertical Gap Size: </SPAN>vertical space between the entities displaced.<BR>
    <SPAN CLASS=pin>Resize: </SPAN>if checked, resizes the target frame to match the extents of the generated grid.<BR>
    <SPAN CLASS=pin>Set Parent: </SPAN>if checked, set the target as parent of the entities in the array.<BR>
    <BR>

    The target determine the position of the grid.
    */

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x265f342c, 0x95e466e6));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateGridLayout2DProto);
    // Class ID of the objects to which the behavior can applied
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

//-------------------------------------------------
// GridLayout2D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateGridLayout2DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("2D Grid Layout");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Components Array", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Margins", CKPGUID_RECT, "(10,10),(10,10)");
    proto->DeclareInParameter("Horizontal Gap Size", CKPGUID_INT, "10");
    proto->DeclareInParameter("Vertical Gap Size", CKPGUID_INT, "10");
    proto->DeclareInParameter("Resize", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Set Parent", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    // Function that will be called upon behavior execution
    proto->SetFunction(GridLayout2D);

    *pproto = proto;
    return CK_OK;
}

//-------------------------------------------------
// GridLayout2D behavior execution function
//
//-------------------------------------------------
int GridLayout2D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    // Get the reference 2D frame
    CK2dEntity *rootFrame = (CK2dEntity *)beh->GetTarget();
    if (!(rootFrame && CKIsChildClassOf(rootFrame, CKCID_2DENTITY)))
        return CKBR_OWNERERROR;

    // Array to display
    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!array)
        return CKBR_PARAMETERERROR;
    int nbRow = array->GetRowCount();
    int nbColumn = array->GetColumnCount();
    if (nbRow == 0)
    {
        behcontext.Context->OutputToConsole("Invalid target array : No row");
        return CKBR_PARAMETERERROR;
    }
    for (int c = 0; c < nbColumn; c++)
    {
        if (array->GetColumnType(c) != CKARRAYTYPE_OBJECT)
        {
            behcontext.Context->OutputToConsole("Invalid target array : column type");
            return CKBR_PARAMETERERROR;
        }
    }

    // Get the layout parameters
    int horizontalGap = 10, verticalGap = 10;
    VxRect margins(10, 10, 10, 10);
    CKBOOL resize = TRUE, setParent = FALSE;
    beh->GetInputParameterValue(1, &margins);
    beh->GetInputParameterValue(2, &horizontalGap);
    beh->GetInputParameterValue(3, &verticalGap);
    beh->GetInputParameterValue(4, &resize);
    beh->GetInputParameterValue(5, &setParent);

    // Position of each entity in the group relative to the reference frame
    Vx2DVector currentPosition, currentSize;
    rootFrame->GetPosition(currentPosition); // Top-Left corner => must center the coordinates
    rootFrame->GetSize(currentSize);

    currentPosition.x += margins.left;
    currentPosition.y += margins.top;

    Vx2DVector pos(currentPosition), cellSize;
    int *maxH = NULL;
    int *maxV = NULL;

    if (resize)
    {
        float hMarginSize = margins.left + margins.right + (nbColumn - 1) * horizontalGap;
        float vMarginSize = margins.top + margins.bottom + (nbRow - 1) * verticalGap;
        cellSize.x = (currentSize.x - hMarginSize) / nbColumn;
        cellSize.y = (currentSize.y - vMarginSize) / nbRow;
    }
    else
    {
        maxV = new int[nbRow];
        maxH = new int[nbColumn];

        for (int j = 0; j < nbColumn; j++)
            maxH[j] = 0;

        for (int i = 0; i < nbRow; i++)
        {
            maxV[i] = 0;
            for (int j = 0; j < nbColumn; j++)
            {
                CKObject *obj = (CKObject *)array->GetElementObject(i, j);
                if (CKIsChildClassOf(obj, CKCID_2DENTITY))
                {
                    ((CK2dEntity *)obj)->GetSize(cellSize);
                    if (cellSize.x > maxH[j])
                        maxH[j] = (int)cellSize.x;
                    if (cellSize.y > maxV[i])
                        maxV[i] = (int)cellSize.y;
                }
            }
        }
    }

    for (int i = 0; i < nbRow; i++)
    {
        for (int j = 0; j < nbColumn; j++)
        {
            CKObject *obj = (CKObject *)array->GetElementObject(i, j);
            if (CKIsChildClassOf(obj, CKCID_2DENTITY))
            {
                CK2dEntity *ent = (CK2dEntity *)obj;
                if (resize)
                    ent->SetSize(cellSize);
                ent->SetPosition(currentPosition);
                if (setParent)
                    ent->SetParent(rootFrame);
            }
            currentPosition.x += (((resize) ? (cellSize.x) : (maxH[j])) + horizontalGap);
        }
        currentPosition.x = pos.x;
        currentPosition.y += (((resize) ? (cellSize.y) : (maxV[i])) + verticalGap);
    }

    if (!resize)
    {
        delete[] maxH;
        delete[] maxV;
    }

    // 'Out'
    beh->ActivateOutput(0);
    return CKBR_OK;
}
