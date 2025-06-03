/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ActivateNode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorActivateNodeDecl();
CKERROR CreateActivateNodeProto(CKBehaviorPrototype **);
int ActivateNode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorActivateNodeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Node");
    od->SetDescription("Activates or deactivates a node in a Nodal Path.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>nodal path to which the node belongs.<BR>
    <SPAN CLASS=pin>Node: </SPAN>object representing the node to activate or deactivate.<BR>
    <SPAN CLASS=pin>Activate: </SPAN>node will be activated if TRUE, otherwise node will be deactivated.<BR>
    <BR>
    If a node is deactivated, then all links leading to or from the node will also be deactivated.<BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also: other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x92a4e59, 0x4009769c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateNodeProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateActivateNodeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Activate Node");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Activate", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ActivateNode);

    *pproto = proto;
    return CK_OK;
}

int ActivateNode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;
    CKAttributeManager *attman = ctx->GetAttributeManager();

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKGroup *group = (CKGroup *)beh->GetInputParameterObject(0);
    CKParameterOut *param = group->GetAttributeParameter(attman->GetAttributeTypeByName(Network3dName));
    if (!param)
        throw "Given Group isn't a Network";
    N3DGraph *graph;
    param->GetValue(&graph);

    CK3dEntity *s = (CK3dEntity *)beh->GetInputParameterObject(1);
    CKBOOL b = FALSE;
    beh->GetInputParameterValue(2, &b);

    // the core code
    graph->ActivateState(s, b);

    return CKBR_OK;
}
