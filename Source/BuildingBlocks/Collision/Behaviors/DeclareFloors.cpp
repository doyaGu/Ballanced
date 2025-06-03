/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Declare Floors
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDeclareFloorsDecl();
CKERROR CreateDeclareFloorsProto(CKBehaviorPrototype **);
int DeclareFloors(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeclareFloorsDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Declare Floors");
    od->SetDescription("Identifies floor objects by their name");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Key Substring: </SPAN>any object in the level which name containing this string becomes a floor.<BR>
    <BR>
    This building block can be applied to any object in the enviroment.<BR>
    eg : if 'Key Name'= Quad, then all the objects of the scene containing 'Quad' in their name, will be concidered as floors.<BR>
    It must be triggered once to initialize floor management.<BR>
    You can declare floors also by giving the "Floor" attribute to your floors.<BR>
    */
    od->SetCategory("Collisions/Floors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x8e43cd12, 0x98dc215d));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateDeclareFloorsProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateDeclareFloorsProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Declare Floors");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Key Substring", CKPGUID_STRING, "Floor");
    proto->DeclareInParameter("Attributes", CKPGUID_FLOOR, "0;TRUE;0;FALSE;TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeclareFloors);

    *pproto = proto;
    return CK_OK;
}

int DeclareFloors(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    CKLevel *level = ctx->GetCurrentLevel();

    CKSTRING string = (CKSTRING)beh->GetInputParameterReadDataPtr(0);

    CKDWORD geo = CKFLOOR_FACES;
    CKBOOL moving = TRUE;
    int type = 0;
    CKBOOL hiera = FALSE;
    CKBOOL first = TRUE;

    CK_ID *paramids = (CK_ID *)beh->GetInputParameterReadDataPtr(1);
    if (paramids)
    {
        CKParameterOut *pout = NULL;

        pout = (CKParameterOut *)ctx->GetObject(paramids[0]);
        if (pout)
            pout->GetValue(&geo);

        pout = (CKParameterOut *)ctx->GetObject(paramids[1]);
        if (pout)
            pout->GetValue(&moving);

        pout = (CKParameterOut *)ctx->GetObject(paramids[2]);
        if (pout)
            pout->GetValue(&type);

        pout = (CKParameterOut *)ctx->GetObject(paramids[3]);
        if (pout)
            pout->GetValue(&hiera);

        pout = (CKParameterOut *)ctx->GetObject(paramids[4]);
        if (pout)
            pout->GetValue(&first);
    }

    floorman->AddFloorObjectsByName(level, string, (CK_FLOORGEOMETRY)geo, moving, type, hiera, first);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
