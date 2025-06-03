/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         Add Control Point
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddCurvePointDecl();
CKERROR CreateAddCurvePointProto(CKBehaviorPrototype **);
int AddCurvePoint(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddCurvePointDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Add Control Point");
    od->SetDescription("Allows to extend a curve with a new control point");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>position expressed in a given referential.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>object used as point of reference for the position of new point.<BR>
    <SPAN CLASS=pin>Prior Point Index: </SPAN>range of the point which comes before the new one .<BR>
    <BR>
    <SPAN CLASS=pout>Curve Point: </SPAN>created curve point.<BR>
    <SPAN CLASS=pout>Percentage: </SPAN>position percentage on the curve (0%=begining, 100%=finnish).<BR>
    <BR>
    This behavior adds a new control point to a curve.<BR>
    */
    od->SetCategory("3D Transformations/Curve");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1234eefa, 0xffaec112));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddCurvePointProto);
    od->SetCompatibleClassId(CKCID_CURVE);
    return od;
}

CKERROR CreateAddCurvePointProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Add Control Point");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Prior Point Index", CKPGUID_INT, "-1");

    proto->DeclareSetting("Dynamic", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Update Percentage", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("Curve Point", CKPGUID_CURVEPOINT);
    proto->DeclareOutParameter("Percentage", CKPGUID_PERCENTAGE, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(AddCurvePoint);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddCurvePoint(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKCurve *ent = (CKCurve *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get position
    VxVector vector(0, 0, 0);
    beh->GetInputParameterValue(0, &vector);

    // Get referential
    CK3dEntity *ref = (CK3dEntity *)beh->GetInputParameterObject(1);

    // Get precursor

    int range = 0;
    beh->GetInputParameterValue(2, &range);
    if (range > (ent->GetControlPointCount()))
        return CKBR_PARAMETERERROR;

    // Dynamic ?
    CKBOOL dynamic = TRUE;
    beh->GetLocalParameterValue(0, &dynamic);

    CK_OBJECTCREATION_OPTIONS creaoptions = CK_OBJECTCREATION_NONAMECHECK;
    if (dynamic)
        creaoptions = (CK_OBJECTCREATION_OPTIONS)(CK_OBJECTCREATION_NONAMECHECK | CK_OBJECTCREATION_DYNAMIC);

    VxVector echelle(0.4f, 0.4f, 0.4f);

    // get precursor

    CKCurvePoint *precursor = ent->GetControlPoint(range);

    CKCurvePoint *newPoint = (CKCurvePoint *)behcontext.Context->CreateObject(CKCID_CURVEPOINT, "X", creaoptions);
    newPoint->SetPosition(&vector, ref, FALSE);
    newPoint->SetScale(&echelle, FALSE);

    // insert new point in curve
    ent->InsertControlPoint(precursor, newPoint);

    // set output parameter value

    beh->SetOutputParameterObject(0, newPoint); // this new point

    CKBOOL updatep = TRUE;
    beh->GetLocalParameterValue(1, &updatep);

    if (updatep)
    {
        float prct = (newPoint->GetLength()) / (ent->GetLength());
        beh->SetOutputParameterValue(1, &prct); // percentage
    }

    return CKBR_OK;
}
