/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FindCurvedPath
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorFindCurvedPathDecl();
CKERROR CreateFindCurvedPathProto(CKBehaviorPrototype **);
int FindCurvedPath(const CKBehaviorContext &behcontext);
CKERROR FindCurvedPathCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFindCurvedPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Find Curved Nodal Path");
    od->SetDescription("Retrieves a curve to follow between a starting point and a destination point.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Path Found: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>No Path Found: </SPAN>is activated if no curved path can be found.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>group of nodal paths to search in.<BR>
    <SPAN CLASS=pin>Start Object: </SPAN>starting point.<BR>
    <SPAN CLASS=pin>Goal Object: </SPAN>destination to reach.<BR>
    <SPAN CLASS=pin>Fitting Coef: </SPAN>percentage value [0% to 100%] which will soften the curve produced.<BR>
    <BR>
    <SPAN CLASS=pout>Curve: </SPAN>3D Curve to follow to reach the goal. Path Follow or
    Set Curve Step can be used to go through the 3D Curve.<BR>
    <BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also : other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x72542b8a, 0x6fe901c7));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFindCurvedPathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateFindCurvedPathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Find Curved Nodal Path");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Path Found");
    proto->DeclareOutput("No Path Found");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Start Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Goal Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Fitting Coef", CKPGUID_PERCENTAGE);

    proto->DeclareOutParameter("Curve", CKPGUID_CURVE);

    // Object List
    proto->DeclareLocalParameter(NULL, CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FindCurvedPath);
    proto->SetBehaviorCallbackFct(FindCurvedPathCallback);

    *pproto = proto;
    return CK_OK;
}

int FindCurvedPath(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKAttributeManager *attman = ctx->GetAttributeManager();

    beh->ActivateInput(0, FALSE);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    if (!group)
        throw "No group given";
    CKParameterOut *param = group->GetAttributeParameter(attman->GetAttributeTypeByName(Network3dName));
    if (!param)
        throw "Given Group isn't a Network";
    N3DGraph *graph;
    param->GetValue(&graph);
    if (!graph)
        throw "There is no Graph attached";

    CK3dEntity *s = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (graph->SeekPosition(CKOBJID(s)) == -1)
        throw "Start point isn't in the Network";
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(2);
    if (graph->SeekPosition(CKOBJID(e)) == -1)
        throw "End point isn't in the Network";

    CKGroup *path = (CKGroup *)beh->GetLocalParameterObject(0);
    path->Clear();

    // the core code
    graph->FindPath(s, e, path);

    if (path->GetObjectCount() == 0)
    { // no path were found
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKCurve *curve = (CKCurve *)beh->GetOutputParameterObject(0);
    if (!curve)
        return CKBR_PARAMETERERROR;

    // we delete all the existing curve point
    CKCurvePoint *curvepoint;
    int i;
    for (i = 0; i < curve->GetControlPointCount(); i++)
    {
        curvepoint = curve->GetControlPoint(i);
        CKDestroyObject(curvepoint);
    }
    curve->RemoveAllControlPoints();

    // now we must create the curve
    VxVector pos;
    for (i = 0; i < path->GetObjectCount(); i++)
    {
        curvepoint = (CKCurvePoint *)ctx->CreateObject(CKCID_CURVEPOINT, "CurvePoint", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        CK3dEntity *e = (CK3dEntity *)path->GetObject(i);
        e->GetPosition(&pos);
        curvepoint->SetPosition(&pos);
        curve->AddControlPoint(curvepoint);
    }

    float fitting = 0;
    beh->GetInputParameterValue(3, &fitting);
    curve->SetFittingCoeff(fitting);

    beh->ActivateOutput(0);
    return CKBR_OK;
}

CKERROR FindCurvedPathCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = beh->GetCKContext();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        CKGroup *group = (CKGroup *)ctx->CreateObject(CKCID_GROUP, "FindPathGroup", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        group->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        beh->SetLocalParameterObject(0, group);
        CKCurve *curve = (CKCurve *)ctx->CreateObject(CKCID_CURVE, "FindCurvedPath", beh->IsDynamic() ? CK_OBJECTCREATION_DYNAMIC : CK_OBJECTCREATION_NONAMECHECK);
        curve->ModifyObjectFlags(CK_OBJECT_NOTTOBELISTEDANDSAVED, 0);
        beh->SetOutputParameterObject(0, curve);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        CKGroup *group = (CKGroup *)beh->GetLocalParameterObject(0);
        CKDestroyObject(group);
        CKCurve *curve = (CKCurve *)beh->GetOutputParameterObject(0);
        CKDestroyObject(curve);
    }
    break;
    }
    return CKBR_OK;
}
