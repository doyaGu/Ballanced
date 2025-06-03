/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ActivateLink
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorActivateLinkDecl();
CKERROR CreateActivateLinkProto(CKBehaviorPrototype **);
int ActivateLink(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorActivateLinkDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Link");
    od->SetDescription("Activates or deactivates a link between two nodes of a Nodal Path.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Nodal Path: </SPAN>nodal path to which the link belongs.<BR>
    <SPAN CLASS=pin>Start Node: </SPAN>link's starting point (node).<BR>
    <SPAN CLASS=pin>End Node: </SPAN>link's ending point (node).<BR>
    <SPAN CLASS=pin>Activate: </SPAN>link will be activated if TRUE, otherwise link will be deactivated.<BR>
    <SPAN CLASS=pin>TwoWay: </SPAN>building block will be applied to both directions if TRUE and if link is two way.<BR>
    <BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also: other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    /* warning:
    - If the two nodes aren't directly linked together, the "Activate Link" building block won't activate all the links that lead from one to another.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x51df79d8, 0x294163a3));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateLinkProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateActivateLinkProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Activate Link");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Nodal Path", CKPGUID_GROUP);
    proto->DeclareInParameter("Start Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("End Node", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Activate", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Two Way", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ActivateLink);

    *pproto = proto;
    return CK_OK;
}

int ActivateLink(const CKBehaviorContext &behcontext)
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
    CK3dEntity *e = (CK3dEntity *)beh->GetInputParameterObject(2);
    CKBOOL b = FALSE;
    beh->GetInputParameterValue(3, &b);

    // the core code
    graph->ActivateEdge(s, e, b);
    CKBOOL tw = FALSE;
    beh->GetInputParameterValue(4, &tw);
    if (tw)
        graph->ActivateEdge(e, s, b);

    return CKBR_OK;
}
