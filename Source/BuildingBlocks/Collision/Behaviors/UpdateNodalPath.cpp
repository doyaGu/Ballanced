/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Update Nodal Path
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"
#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorUpdateNodalPathDecl();
CKERROR CreateUpdateNodalPathProto(CKBehaviorPrototype **);
int UpdateNodalPath(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorUpdateNodalPathDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Update Nodal Path");
    od->SetDescription("Reevaluates the nodes distance of the given nodal path.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>the nodal path.<BR>
    <BR>
    This building block must be used if you move in real time the nodes
    of a nodal path, to reevaluate the distance between nodes.
    If you don't, you'll not be sure to always get the shortest path using the "GoToNode" building block.
    */
    od->SetCategory("3D Transformations/Nodal Path");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x21647449, 0x51126c00));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateUpdateNodalPathProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateUpdateNodalPathProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Update Nodal Path");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(UpdateNodalPath);

    *pproto = proto;
    return CK_OK;
}

int UpdateNodalPath(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKAttributeManager *attman = ctx->GetAttributeManager();

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

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

    graph->UpdateDistance();

    return CKBR_OK;
}
