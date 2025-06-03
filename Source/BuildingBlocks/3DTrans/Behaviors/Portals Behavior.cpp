/////////////////////////////////////////////////////
//		           Portal Management
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "PortalsManager.h"

CKObjectDeclaration *FillBehaviorPortalsBehaviorDecl();
CKERROR CreatePortalsBehaviorProto(CKBehaviorPrototype **pproto);
int PortalsBehavior(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorPortalsBehaviorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Portal Management");
    od->SetDescription("Automatically hide places which can't be see through visible portals.");
    /* rem:
    <SPAN CLASS=in>On: </SPAN>activates the process.<BR>
    <SPAN CLASS=in>Off: </SPAN>deactivates the process.<BR>
    <BR>
    <SPAN CLASS=out>Exit On: </SPAN>is activated if the building block is activated.<BR>
    <SPAN CLASS=out>Exit Off: </SPAN>is activated if the building block is deactivated.<BR>
    <BR>
    <SPAN CLASS=pin>Debug Cameras: </SPAN>the group of cameras on which to apply the portal
    management, allowing a visual "debugging" of the portal management, when viewing from another camera.<BR>
    NULL [which is the default value] means the portal management will be processed for the current camera.<BR>
    <SPAN CLASS=pin>Traversal Depth: </SPAN>number of places visible in depth from the current one. 0 means infinity.<BR>
    <BR>
    <SPAN CLASS=pout>Current Places: </SPAN>a collection of all places in which the camera is.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreatePortalsBehaviorProto);
    od->SetCategory("Optimizations/Portals");
    od->SetGuid(CKGUID(0x55041717, 0x30817370));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_BEOBJECT);

    return od;
}

CKERROR CreatePortalsBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Portal Management");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("Debug Cameras", CKPGUID_GROUP);
    proto->DeclareInParameter("Traversal Depth", CKPGUID_INT, "0");
    proto->DeclareInParameter("TransPlace Objects", CKPGUID_GROUP);

    proto->DeclareOutParameter("Current Places", CKPGUID_OBJECTARRAY);
    proto->DeclareOutParameter("Rendered Places", CKPGUID_OBJECTARRAY);
    proto->DeclareOutParameter("Rendered TransPlace Objects", CKPGUID_OBJECTARRAY);

    proto->DeclareSetting("Automatically Create Portals", CKPGUID_BOOL);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);

    proto->SetFunction(PortalsBehavior);

    *pproto = proto;
    return CK_OK;
}

int PortalsBehavior(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    PortalsManager *pm = (PortalsManager *)behcontext.Context->GetManagerByGuid(PORTALS_MANAGER_GUID);
    if (!pm)
        return CKBR_GENERICERROR;

    CKGroup *cam = (CKGroup *)beh->GetInputParameterObject(0);
    pm->SetDebugCameras(cam);
    pm->SetBehavior(beh);

    int rl = 0;
    beh->GetInputParameterValue(1, &rl);
    pm->SetRecursivityLevel(rl);

    if (beh->IsInputActive(0))
    { // we enter by on
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);

        // Automatically Create Portals ?
        CKBOOL autoCreatePortal = FALSE;
        beh->GetLocalParameterValue(0, &autoCreatePortal);

        if (autoCreatePortal)
        {
            pm->AutomaticallyGeneratePortals();
        }

        pm->Activate(TRUE);
    }

    if (beh->IsInputActive(1))
    { // we enter by off
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);
        pm->Activate(FALSE);

        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
