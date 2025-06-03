/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Declare Obstacles
//
/////////////////////////////////////////////////////
////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDeclareObstaclesDecl();
CKERROR CreateDeclareObstaclesProto(CKBehaviorPrototype **);
int DeclareObstacles(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorDeclareObstaclesDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Declare Obstacles");
    od->SetDescription("Identifies obstacle objects by their name");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Key Substring: </SPAN>any object in the level which name containes this string becomes an obstacle.<BR>
    <SPAN CLASS=pin>Moving: </SPAN>whether you want the objects to be defined as moving obstacles.<BR>
    <SPAN CLASS=pin>Geometry: </SPAN>the type of geometry you want to consider in collision testing.<BR>
    <BR>
    This building block can be applied to any object in the environment. It should be triggered only once to initialize collision management.
    */
    od->SetCategory("Collisions/Obstacle");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x21dc34e8, 0xd512cd89));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeclareObstaclesProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(COLLISION_MANAGER_GUID);
    return od;
}

CKERROR CreateDeclareObstaclesProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Declare Obstacles");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("In");

    proto->DeclareInParameter("Key Substring", CKPGUID_STRING, "Obstacle");
    proto->DeclareInParameter("Moving", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Geometry", CKPGUID_OBSTACLE, "BoundingBox;FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DeclareObstacles);

    *pproto = proto;
    return CK_OK;
}

int DeclareObstacles(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKCollisionManager *cm = (CKCollisionManager *)ctx->GetManagerByGuid(COLLISION_MANAGER_GUID);

    char string[512];
    beh->GetInputParameterValue(0, &string);

    CKBOOL moving = FALSE;
    beh->GetInputParameterValue(1, &moving);

    CK_GEOMETRICPRECISION geomtype = (CK_GEOMETRICPRECISION)1;
    CKBOOL hiera = FALSE;

    CKParameterIn *pin;
    CKParameter *pout;
    pin = beh->GetInputParameter(2);
    if (pin)
    {
        pout = pin->GetRealSource();
        if (pout && pout->GetGUID() == CKPGUID_OBSTACLE)
        {
            CK_ID *paramids = NULL;
            paramids = (CK_ID *)pout->GetReadDataPtr();
            CKParameterOut *geompout = (CKParameterOut *)ctx->GetObject(paramids[0]);
            CKParameterOut *hierapout = (CKParameterOut *)ctx->GetObject(paramids[1]);

            geompout->GetValue(&geomtype);
            hierapout->GetValue(&hiera);
        }
    }

    CKLevel *level = ctx->GetCurrentLevel();
    cm->AddObstaclesByName(level, string, moving, geomtype, hiera);

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
