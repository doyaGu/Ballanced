/////////////////////////////////////////////////////
//		            Set Portal
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "PortalsManager.h"

CKObjectDeclaration *FillBehaviorSetPortalDecl();
CKERROR CreateSetPortalProto(CKBehaviorPrototype **pproto);
int SetPortal(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPortalDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Portal");
    od->SetDescription("Defines a 3D Entity as a portal between 2 places.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Place 1: </SPAN>First place.<BR>
    <SPAN CLASS=pin>Place 2: </SPAN>Second place.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPortalProto);
    od->SetCategory("Optimizations/Portals");
    od->SetGuid(CKGUID(0x465c4732, 0x30996468));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_3DENTITY);

    return od;
}

CKERROR CreateSetPortalProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Portal");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Place 1", CKPGUID_PLACE);
    proto->DeclareInParameter("Place 2", CKPGUID_PLACE);
    proto->DeclareInParameter("Auto Fit", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(SetPortal);

    *pproto = proto;
    return CK_OK;
}

/**********************************************/
/*				FUNCTION
/**********************************************/
int SetPortal(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *portal = (CK3dEntity *)beh->GetTarget();
    if (!portal)
        return CKBR_OWNERERROR;

    //----------------------------------
    //--- we set the object as a portal
    portal->SetFlags(portal->GetFlags() | CK_3DENTITY_PORTAL);

    CKPlace *p1 = (CKPlace *)beh->GetInputParameterObject(0);
    CKPlace *p2 = (CKPlace *)beh->GetInputParameterObject(1);

    if (p1 == p2)
        return CKBR_PARAMETERERROR;

    if (p1)
    {
        p1->AddPortal(p2, portal);
    }

    CKBOOL fit = FALSE;
    beh->GetInputParameterValue(2, &fit);

    //----------------------------
    //--- we fit the bounding box
    if (fit)
    {
        if (!p1 || !p2)
            return CKBR_PARAMETERERROR;
        VxMatrix BBoxMatrix;
        if (p1->ComputeBestFitBBox(p2, BBoxMatrix))
        {
            portal->SetWorldMatrix(BBoxMatrix);
        }
    }

    return CKBR_OK;
}
