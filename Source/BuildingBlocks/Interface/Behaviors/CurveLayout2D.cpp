/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CurveLayout2D
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCurveLayout2DDecl();
CKERROR CreateCurveLayout2DProto(CKBehaviorPrototype **pproto);
int CurveLayout2D(const CKBehaviorContext &behcontext);

//-------------------------------------------------
//  Creates a declaration of this behavior
//
//-------------------------------------------------
CKObjectDeclaration *FillBehaviorCurveLayout2DDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("2D Curve Layout");
    od->SetDescription("Organize a set of 2D belong a 3D curve in space");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Components: </SPAN>group containing the 2D entities to organize.<BR>
    <SPAN CLASS=pin>Center: </SPAN>if checked, we place the center of the frame on the curve.<BR>
    <BR>

    The entities are placed on screen like if they were along the 3D curve
    */

    // Category in Virtools interface
    od->SetCategory("Interface/Layouts");

    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    // Unique identifier of this prototype
    od->SetGuid(CKGUID(0x2956342c, 0x95246746));

    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);

    // Function that will create the behavior prototype
    od->SetCreationFunction(CreateCurveLayout2DProto);
    // Class ID of the objects to which the behavior can applied
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

//-------------------------------------------------
// CurveLayout2D behavior prototype creation function
//
//-------------------------------------------------
CKERROR CreateCurveLayout2DProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("2D Curve Layout");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Input/Outputs declaration
    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Components", CKPGUID_DATAARRAY);
    proto->DeclareInParameter("Center", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    // Function that will be called upon behavior execution
    proto->SetFunction(CurveLayout2D);

    *pproto = proto;
    return CK_OK;
}

//-------------------------------------------------
// CurveLayout2D behavior execution function
//
//-------------------------------------------------
int CurveLayout2D(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    // Get the reference curve
    CKCurve *curve = (CKCurve *)beh->GetTarget();
    if (!(curve && CKIsChildClassOf(curve, CKCID_CURVE)))
        return CKBR_PARAMETERERROR;

    // Get the group of 2DEntities
    CKDataArray *array = (CKDataArray *)beh->GetInputParameterObject(0);
    if (!array)
        return CKBR_OK;
    int objectCount = array->GetRowCount();
    if (objectCount == 0)
        return CKBR_OK;
    if (array->GetColumnCount() == 0)
        return CKBR_PARAMETERERROR;
    if (array->GetColumnType(0) != CKARRAYTYPE_OBJECT)
        return CKBR_PARAMETERERROR;
    int nbColumn = array->GetColumnCount();
    if (nbColumn > 1)
    {
        if (array->GetColumnType(1) != CKARRAYTYPE_PARAMETER)
            return CKBR_PARAMETERERROR;
        if (array->GetColumnParameterGuid(1) != CKPGUID_2DVECTOR)
            return CKBR_PARAMETERERROR;
    }
    CKBOOL center = FALSE;
    beh->GetInputParameterValue(1, &center);

    // Position of each entity in the group relative to the reference frame
    VxVector currentPosition, tmpPosition;
    Vx2DVector objPosition, objSize;
    CKRenderContext *render = behcontext.CurrentRenderContext;
    CKCamera *cam = render->GetAttachedCamera();

    // TODO resize/ visibilite quand z derriere camera
    for (int i = 0; i < objectCount; i++)
    {
        CKObject *obj = array->GetElementObject(i, 0);
        if (CKIsChildClassOf(obj, CKCID_2DENTITY))
        {
            CK2dEntity *ent = (CK2dEntity *)obj;
            curve->GetLocalPos((float)i / (objectCount - 1), &currentPosition);
            render->Transform(&tmpPosition, &currentPosition, curve);
            objPosition.x = tmpPosition.x;
            objPosition.y = tmpPosition.y;
            curve->Transform(&tmpPosition, &currentPosition, cam);
            ent->SetZOrder((int)tmpPosition.z);
            if (array && (nbColumn > 1))
            {
                array->GetElementValue(i, 1, &objSize);
                // redimensionner par rapport aux plans de clip ?
                ent->SetSize(objSize);
            }
            if (center)
            {
                ent->GetSize(objSize);
                objPosition -= (objSize / 2);
            }
            ent->SetPosition(objPosition);
        }
    }

    // 'Out'
    beh->ActivateOutput(0);
    return CKBR_OK;
}
