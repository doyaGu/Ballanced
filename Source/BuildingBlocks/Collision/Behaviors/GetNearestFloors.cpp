/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Get Nearest Floors
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateGetNearestFloorsBehaviorProto(CKBehaviorPrototype **);
int DoGetNearestFloors(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetNearestFloorsDecl()
{

    CKObjectDeclaration *od = CreateCKObjectDeclaration("Get Nearest Floors");
    od->SetDescription("Retrieves the nearest 'floors' from given position");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>None: </SPAN>is activated when no floors are found above or below the point.<BR>
    <SPAN CLASS=out>Lower: </SPAN>is activated when the lower floor is the nearest (or the only) floor from the point.<BR>
    <SPAN CLASS=out>Upper: </SPAN>is activated when the upper floor is the nearest (or the only) floor from the point.<BR>
    <BR>
    <SPAN CLASS=pin>Position: </SPAN>position vector expressed in a given referential.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>coordinate system in which the position is expressed.<BR>
    <BR>
    <SPAN CLASS=pout>Outputs: </SPAN>all the properties of the down and up floors.<BR>
    <BR>
    You need to declare objects as "floors" first, either by using the "DeclareFloors" building block or by setting the
    "Floor" attributes to them.<BR>
    */
    od->SetCategory("Collisions/Floors");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x6dad424e, 0x54b6003d));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetNearestFloorsBehaviorProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR CreateGetNearestFloorsBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Get Nearest Floors");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("None");
    proto->DeclareOutput("Lower");
    proto->DeclareOutput("Upper");

    proto->DeclareInParameter("Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);

    proto->DeclareOutParameter("Lower Floor", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Lower Floor Type", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Lower Floor Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Lower Floor Distance", CKPGUID_FLOAT, "0");
    proto->DeclareOutParameter("Lower Floor Face Index", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Upper Floor", CKPGUID_3DENTITY);
    proto->DeclareOutParameter("Upper Floor Type", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Upper Floor Normal", CKPGUID_VECTOR);
    proto->DeclareOutParameter("Upper Floor Distance", CKPGUID_FLOAT, "0");
    proto->DeclareOutParameter("Upper Floor Face Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(DoGetNearestFloors);
    *pproto = proto;
    return CK_OK;
}

int DoGetNearestFloors(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CK3dEntity *entity = (CK3dEntity *)beh->GetTarget();
    if (!entity)
    {
        beh->ActivateOutput(0);
        return CKBR_OWNERERROR;
    }

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    beh->ActivateInput(0, FALSE);

    CKFloorPoint fp;
    CK_FLOORNEAREST floorn;

    VxVector pos;
    beh->GetInputParameterValue(0, &pos);

    CK3dEntity *ref = NULL;
    ref = (CK3dEntity *)beh->GetInputParameterObject(1);

    if (ref)
        ref->Transform(&pos, &pos);

    floorn = floorman->GetNearestFloors(pos, &fp);

    if (!floorn)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    // we now fill the Output parameters
    int type = 0;

    beh->SetOutputParameterValue(0, &(fp.m_DownFloor));
    floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_DownFloor), NULL, NULL, &type);
    beh->SetOutputParameterValue(1, &type);
    beh->SetOutputParameterValue(2, &fp.m_DownNormal);
    beh->SetOutputParameterValue(3, &fp.m_DownDistance);
    beh->SetOutputParameterValue(4, &fp.m_DownFaceIndex);

    // up floor
    beh->SetOutputParameterValue(5, &(fp.m_UpFloor));
    floorman->ReadAttributeValues((CK3dEntity *)ctx->GetObject(fp.m_UpFloor), NULL, NULL, &type);
    beh->SetOutputParameterValue(6, &type);
    beh->SetOutputParameterValue(7, &fp.m_UpNormal);
    beh->SetOutputParameterValue(8, &fp.m_UpDistance);
    beh->SetOutputParameterValue(9, &fp.m_UpFaceIndex);

    switch (floorn)
    {
    case CKFLOOR_UP:
        beh->ActivateOutput(2);
        break;
    case CKFLOOR_DOWN:
        beh->ActivateOutput(1);
        break;
    }

    return CKBR_OK;
}
