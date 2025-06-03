/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CreateNetwork3d
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "N3dGraph.h"

CKObjectDeclaration *FillBehaviorCreateNetwork3dDecl();
CKERROR CreateCreateNetwork3dProto(CKBehaviorPrototype **);
CKERROR CreateNetwork3dCallback(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCreateNetwork3dDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Create Nodal Path");
    od->SetDescription("Creates a Nodal Path out of a group of frames.");
    /* rem:
    <SPAN CLASS=setting>Debug Display: </SPAN>check this if you want the nodal path to remain visible while in play.<BR>
    <BR>
    This building block allows the user to create a Nodal Path (through a customized dialog box).<BR>
    <BR>
    <a href="nodal.htm">More information on Nodal Paths.</a><BR>
    See also : other Nodal related building blocks in the Nodal Path folder of 3D Transformations.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x435b3d9c, 0x3a8e41f6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCreateNetwork3dProto);
    od->SetCompatibleClassId(CKCID_GROUP);
    od->SetCategory("3D Transformations/Nodal Path");
    return od;
}

CKERROR CreateCreateNetwork3dProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Create Nodal Path");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    // Graph
    proto->DeclareLocalParameter(NULL, CKPGUID_VOIDBUF); // Mac Safe
    // Path Array
    proto->DeclareLocalParameter(NULL, CKPGUID_NONE);
    // Graph Data
    proto->DeclareLocalParameter(NULL, CKPGUID_STATECHUNK);

    proto->DeclareSetting("Debug Display", CKPGUID_BOOL, "FALSE");

    proto->SetBehaviorFlags(CKBEHAVIOR_CUSTOMEDITDIALOG);
    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorCallbackFct(CreateNetwork3dCallback);

    *pproto = proto;
    return CK_OK;
}

CKERROR CreateNetwork3dCallback(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = beh->GetCKContext();
    CKAttributeManager *attman = ctx->GetAttributeManager();

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // First we get the graph
        N3DGraph *graph;
        beh->GetLocalParameterValue(0, &graph);
        // we remove the graph from the render contexts of the level
        CKRenderManager *rman = ctx->GetRenderManager();
        int es;
        for (es = 0; es < rman->GetRenderContextCount(); es++)
        {
            CKRenderContext *rcontext = rman->GetRenderContext(es);
            if (rcontext)
                rcontext->RemovePostRenderCallBack(GraphRender, graph);
        }

        // we add the graph drawing at the level
        CKBOOL display;
        beh->GetLocalParameterValue(3, &display);
        if (display)
        {
            beh->GetLocalParameterValue(0, &graph);
            for (es = 0; es < rman->GetRenderContextCount(); es++)
            {
                CKRenderContext *rcontext = rman->GetRenderContext(es);
                if (rcontext)
                    rcontext->AddPostRenderCallBack(GraphRender, graph);
            }
        }
    }
    break;
    case CKM_BEHAVIOREDITED:
    {
        // First we get the graph
        N3DGraph *graph;
        beh->GetLocalParameterValue(0, &graph);
        // we remove the graph from the render contexts of the level
        CKRenderManager *rman = ctx->GetRenderManager();
        int es;
        for (es = 0; es < rman->GetRenderContextCount(); es++)
        {
            CKRenderContext *rcontext = rman->GetRenderContext(es);
            if (rcontext)
                rcontext->RemovePostRenderCallBack(GraphRender, graph);
        }

        CKInterfaceManager *UIMan = (CKInterfaceManager *)ctx->GetManagerByGuid(INTERFACE_MANAGER_GUID);
        if (UIMan)
            UIMan->CallBehaviorEditionFunction(beh, 0);

        // we add the graph drawing at the level
        CKBOOL display;
        beh->GetLocalParameterValue(3, &display);
        if (display)
        {
            beh->GetLocalParameterValue(0, &graph);
            for (es = 0; es < rman->GetRenderContextCount(); es++)
            {
                CKRenderContext *rcontext = rman->GetRenderContext(es);
                if (rcontext)
                    rcontext->AddPostRenderCallBack(GraphRender, graph);
            }
        }
    }
    break;
    case CKM_BEHAVIORATTACH:
    {
        //. we get the owner group
        CKGroup *group = (CKGroup *)beh->GetOwner();

        // we check if the group isn't already a nodal path
        if (group->HasAttribute(attman->GetAttributeTypeByName(Network3dName)))
            return CKBR_ATTACHFAILED;

        // if not we create the attribute and put it to the group
        int att = attman->RegisterNewAttributeType(Network3dName, CKPGUID_VOIDBUF, CKCID_GROUP, (CK_ATTRIBUT_FLAGS)(CK_ATTRIBUT_SYSTEM | CK_ATTRIBUT_HIDDEN));
        group->SetAttribute(att);
        CKParameterOut *param = group->GetAttributeParameter(att);

        // First we create graph
        N3DGraph *graph = new N3DGraph(ctx);

        // we save the pointer to a local parameter
        beh->SetLocalParameterValue(0, &graph, sizeof(N3DGraph *));
        // and we save it to the attribute (for he other behaviors to get it back)
        param->SetValue(&graph, sizeof(N3DGraph *));

        // we add the graph drawing at the level
        CKBOOL display;
        beh->GetLocalParameterValue(3, &display);
        if (display)
        {
            CKRenderManager *rman = ctx->GetRenderManager();
            for (int es = 0; es < rman->GetRenderContextCount(); es++)
            {
                CKRenderContext *rcontext = rman->GetRenderContext(es);
                if (rcontext)
                    rcontext->AddPostRenderCallBack(GraphRender, graph);
            }
        }
    }
    break;
    case CKM_BEHAVIORLOAD:
    {
        // we initialize the graph
        N3DGraph *graph = NULL;
        beh->SetLocalParameterValue(0, &graph, sizeof(N3DGraph *));

        // Now we must reload the data in the buffer
        CKStateChunk *chunk;
        beh->GetLocalParameterValue(2, &chunk);

        // First we create graph
        graph = new N3DGraph(ctx);

        // we recreate the graph from the chunk
        graph->CreateFromChunk(chunk);

        // we can free the chunk memory by now
        chunk->Clear();

        // we save the pointer to a local parameter
        beh->SetLocalParameterValue(0, &graph, sizeof(N3DGraph *));

        // we now have to save the graph in the attribute
        CKGroup *group = (CKGroup *)beh->GetOwner();
        CKAttributeManager *attman = ctx->GetAttributeManager();
        int att = attman->RegisterNewAttributeType("Nodal Path", CKPGUID_VOIDBUF);
        CKParameterOut *param = group->GetAttributeParameter(att);
        param->SetValue(&graph, sizeof(N3DGraph *));

        // we add the graph drawing at the level
        CKBOOL display;
        beh->GetLocalParameterValue(3, &display);
        if (display)
        {
            CKRenderManager *rman = ctx->GetRenderManager();
            for (int es = 0; es < rman->GetRenderContextCount(); es++)
            {
                CKRenderContext *rcontext = rman->GetRenderContext(es);
                if (rcontext)
                    rcontext->AddPostRenderCallBack(GraphRender, graph);
            }
        }
    }
    break;

    case CKM_BEHAVIORDETACH:
    {
        CKGroup *group = (CKGroup *)beh->GetOwner();
        if (group)
        {
            group->RemoveAttribute(attman->GetAttributeTypeByName(Network3dName));

            // First we get the graph
            N3DGraph *graph;
            beh->GetLocalParameterValue(0, &graph);

            // we remove the graph from the render contexts of the level
            CKRenderManager *rman = ctx->GetRenderManager();
            for (int es = 0; es < rman->GetRenderContextCount(); es++)
            {
                CKRenderContext *rcontext = rman->GetRenderContext(es);
                if (rcontext)
                    rcontext->RemovePostRenderCallBack(GraphRender, graph);
            }

            // we clear the local parameter
            delete graph;
            graph = NULL;
            beh->SetLocalParameterValue(0, &graph, sizeof(N3DGraph *));
        }
    }
    break;
    case CKM_BEHAVIORPRESAVE:
    {
        // First we get the graph
        N3DGraph *graph;
        beh->GetLocalParameterValue(0, &graph);

        // we get the chunk stocked in local parameter
        CKStateChunk *chunk;
        beh->GetLocalParameterValue(2, &chunk);

        // we save the graph
        graph->SaveToChunk(chunk);
    }
    break;
    case CKM_BEHAVIORPOSTSAVE:
    {
        // we get the chunk stocked in local parameter
        CKStateChunk *chunk;
        beh->GetLocalParameterValue(2, &chunk);

        // all we have to do now is clear the chunk to gain memory
        chunk->Clear();
    }
    break;
    case CKM_BEHAVIORRESET:
    {
        // First we get the graph
        N3DGraph *graph;
        beh->GetLocalParameterValue(0, &graph);

        // we reset the dynamic activity of the graph
        graph->ResetActivity();
    }
    }
    return CKBR_OK;
}
