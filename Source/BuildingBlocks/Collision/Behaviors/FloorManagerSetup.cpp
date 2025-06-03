/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		         Floor Manager Setup
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFloorManagerSetupDecl();
CKERROR CreateFloorManagerSetupProto(CKBehaviorPrototype **);
int FloorManagerSetup(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFloorManagerSetupDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Floor Manager Setup");
    od->SetDescription("Configure the Floor Manager.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Angle Limit: </SPAN>angle between a face normal and the world Up direction beyond which the face is no more accepted. Typically angle less than 60 degrees are used. Bigger angles won't look realistic.<BR>
    <SPAN CLASS=pin>Cache Size: </SPAN>the current number of positions kept by the cache of the FloorManager.
    If you have two objects keeped on floors by behaviors, you should put 2 as cache size if they are kept by
    Object Keep On Floor or by Object Keep On Floor V2 with point as object extents setting or 8 as cache size if
    they are kept by Object Keep On Floor V2 with bounding box as object extents setting.<BR>
    <SPAN CLASS=pin>Cache Threshold: </SPAN>a floating point value representing the threshold in all the 2 axis, x and z. This value is relative to the
    coarseness of the floor and the velocity of the objects. Values ranging from 0.1 to 1.0 should do fine generally.<BR>
    */
    od->SetCategory("Collisions/Floors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3a271561, 0x1d8c4b45));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFloorManagerSetupProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateFloorManagerSetupProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Floor Manager Setup");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Angle Limit", CKPGUID_ANGLE, "0:80");
    proto->DeclareInParameter("Cache Size", CKPGUID_INT, "8");
    proto->DeclareInParameter("Cache Threshold", CKPGUID_FLOAT, "0.1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FloorManagerSetup);

    *pproto = proto;
    return CK_OK;
}

int FloorManagerSetup(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    float angle = 0;
    beh->GetInputParameterValue(0, &angle);
    int cachesize = 8;
    beh->GetInputParameterValue(1, &cachesize);
    float threshold = 0.1f;
    beh->GetInputParameterValue(2, &threshold);

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    floorman->SetCacheSize(cachesize);
    floorman->SetCacheThreshold(threshold);
    floorman->SetLimitAngle(angle);

    return CKBR_OK;
}
