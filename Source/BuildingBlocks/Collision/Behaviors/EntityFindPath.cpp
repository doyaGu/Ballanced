/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            EntityFindPath
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "float.h"
#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorEntityFindPathDecl();
CKERROR CreateEntityFindPathProto(CKBehaviorPrototype **);
int EntityFindPath(const CKBehaviorContext &behcontext);
CKERROR EntityFindPathCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorEntityFindPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Entity Find Nodal Path");
    od->SetDescription("Finds the shortest path to a specified destination for a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>group of nodal paths to search in.<BR>
    <SPAN CLASS=pin>Goal Object: </SPAN>destination to reach.<BR>
    <BR>
    <SPAN CLASS=pout>Group: </SPAN>list of nodes passed though on way to destination. List will be empty if no path is found.<BR>
    <BR>
    The building block will search the node closest to its owner first, finding the shortest path to the destination node.<BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also: other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x170a5187, 0x42f10a3a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateEntityFindPathProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateEntityFindPathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Entity Find Nodal Path");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Path Found");
    proto->DeclareOutput("No Path Found");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Goal Object", CKPGUID_3DENTITY);

    proto->DeclareOutParameter("Object List", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(EntityFindPath);
    proto->SetBehaviorCallbackFct(EntityFindPathCallback);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int EntityFindPath(const CKBehaviorContext &behcontext)
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

    int c = group->GetObjectCount();

    CK3dEntity *object = (CK3dEntity *)beh->GetTarget();
    if (!object)
    {
        beh->ActivateOutput(1); // No path found
        return CKBR_OK;
    }

    VxVector pos;
    object->GetPosition(&pos);

    CK3dEntity *ent;
    CK3dEntity *s = NULL;
    float min = FLT_MAX;
    int i = 0;
    while (i < c)
    {
        ent = (CK3dEntity *)group->GetObject(i);
        if (CKIsChildClassOf(ent, CKCID_3DENTITY))
        {
            VxVector p;
            ent->GetPosition(&p);
            float m = SquareMagnitude(p - pos);
            if (m < min)
            {
                min = m;
                s = ent;
            }
        }
        i++;
    }

    if (graph->SeekPosition(CKOBJID(s)) == -1)
        throw "Start point isn't in the Network";
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (graph->SeekPosition(CKOBJID(e)) == -1)
        throw "End point isn't in the Network";

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

CKERROR EntityFindPathCallback(const CKBehaviorContext &behcontext)
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
