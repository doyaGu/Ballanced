/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FindPath
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorFindPathDecl();
CKERROR CreateFindPathProto(CKBehaviorPrototype **);
int FindPath(const CKBehaviorContext &behcontext);
CKERROR FindPathCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFindPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Find Path");
    od->SetDescription("Returns a group of nodes to go by to reach a goal from a starting point.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>Path Found: </SPAN>is activated if a path is found.<BR>
    <SPAN CLASS=out>No Path Found: </SPAN>is activated if no path is found.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>group of nodal paths to search in.<BR>
    <SPAN CLASS=pin>Start Object: </SPAN>starting point.<BR>
    <SPAN CLASS=pin>Goal Object: </SPAN>destination to reach.<BR>
    <BR>
    <SPAN CLASS=pout>Object List: </SPAN>list of nodes passed though on way to destination. List will be empty if no path is found.<BR>
    <BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also : other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78c60de, 0x6c744614));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFindPathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateFindPathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Find Path");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Path Found");
    proto->DeclareOutput("No Path Found");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Start Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Goal Object", CKPGUID_3DENTITY);

    proto->DeclareOutParameter("Object List", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FindPath);
    proto->SetBehaviorCallbackFct(FindPathCallback);

    *pproto = proto;
    return CK_OK;
}

int FindPath(const CKBehaviorContext &behcontext)
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
        return CKBR_PARAMETERERROR;
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(2);
    if (graph->SeekPosition(CKOBJID(e)) == -1)
        return CKBR_PARAMETERERROR;

    CKGroup *path = (CKGroup *)beh->GetOutputParameterObject(0);
    path->Clear();

    // the core code
    graph->FindPath(s, e, path);

    // Output Activation
    if (path->GetObjectCount() > 0)
        beh->ActivateOutput(0); // Path found
    else
        beh->ActivateOutput(1); // No path found

    return CKBR_OK;
}

CKERROR FindPathCallback(const CKBehaviorContext &behcontext)
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
        beh->SetOutputParameterObject(0, group);
    }
    break;

    case CKM_BEHAVIORDELETE:
    {
        CKGroup *group = (CKGroup *)beh->GetOutputParameterObject(0);
        CKDestroyObject(group);
    }
    break;
    }
    return CKBR_OK;
}
