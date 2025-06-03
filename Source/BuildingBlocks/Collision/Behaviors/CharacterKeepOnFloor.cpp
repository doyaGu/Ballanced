/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CharacterKeepOnFloor
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateCharacterKeepOnFloorBehaviorProto(CKBehaviorPrototype **);
int DoCharacterKeepOnFloor(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCharacterKeepOnFloorDecl()
{

    CKObjectDeclaration *od = CreateCKObjectDeclaration("Character Keep On Floor");
    od->SetDescription("Forces a character to stay on previously defined floors.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    To have a floor reference object (for example feet), the character must have an object containing
    ("FloorRef","Foot","Pas") in its name, and be a child of the root of the character in order to be
    recognized.<BR>
    If this object is not present, the building block uses a character's bounding box to make contact.
    Objects representing floors must be declared as floors first, either by using the behavior
    DeclareFloors or giving them the Floor attribute.<BR>
    See also : Enhanced Character Keep On Floor<BR>
    */
    /* warning:
    - This building block should be looped if the character is to keep to on the floor all the time.<BR>
    */
    od->SetCategory("Characters/Constraint");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xdbac2124, 0x6adcbfa4));
    od->SetAuthorGuid(CKGUID());
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCharacterKeepOnFloorBehaviorProto);
    od->SetCompatibleClassId(CKCID_CHARACTER);
    od->NeedManager(FLOOR_MANAGER_GUID);
    return od;
}

CKERROR DaKeepOnFloorCB(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORREADSTATE:
    case CKM_BEHAVIORLOAD:
    case CKM_BEHAVIORCREATE:
    {
        beh->SetPriority(-32100);
        return CKBR_OK;
    }
    break;
    }
    return CKBR_OK;
}

CKERROR CreateCharacterKeepOnFloorBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Character Keep On Floor");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DoCharacterKeepOnFloor);
    proto->SetBehaviorCallbackFct(DaKeepOnFloorCB);
    *pproto = proto;
    return CK_OK;
}

int DoCharacterKeepOnFloor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKCharacter *carac = (CKCharacter *)beh->GetOwner();

    CKContext *ctx = behcontext.Context;
    CKFloorManager *floorman = (CKFloorManager *)ctx->GetManagerByGuid(FLOOR_MANAGER_GUID);

    beh->ActivateInput(0, FALSE);

    CK3dEntity *pas = carac->GetFloorReferenceObject();
    if (pas)
    {
        VxVector pos;
        pas->GetPosition(&pos);
        float distup, distdown;
        CK3dEntity *eup, *edown;

        CKFloorPoint fp;
        if (floorman->GetNearestFloors(pos, &fp))
        {
            distup = fp.m_UpDistance;
            distdown = fp.m_DownDistance;
            eup = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
            edown = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);

            carac->GetPosition(&pos);
            if (eup)
                pos.y += distup;
            else if (edown)
                pos.y += distdown;
            carac->SetPosition(&pos);
        }
    }
    else
    {
        const VxBbox &box = carac->GetBoundingBox();
        VxVector pos = (box.Max + box.Min) / 2.0f;
        pos.y = box.Min.y;

        float distup, distdown;
        CK3dEntity *eup, *edown;
        CKFloorPoint fp;
        if (floorman->GetNearestFloors(pos, &fp))
        {
            distup = fp.m_UpDistance;
            distdown = fp.m_DownDistance;
            eup = (CK3dEntity *)ctx->GetObject(fp.m_UpFloor);
            edown = (CK3dEntity *)ctx->GetObject(fp.m_DownFloor);
            carac->GetPosition(&pos);
            if (eup)
                pos.y += distup;
            else if (edown)
                pos.y += distdown;
            carac->SetPosition(&pos);
        }
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}
