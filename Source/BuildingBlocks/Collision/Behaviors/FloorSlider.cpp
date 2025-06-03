/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            FloorSlider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFloorSliderDecl();
CKERROR CreateFloorSliderProto(CKBehaviorPrototype **);
int FloorSlider(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFloorSliderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Floor Slider");
    od->SetDescription("Constrain an object to the floor boundaries, with a given radius.");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetCategory("Collisions/Floors");
    od->SetGuid(CKGUID(0x136044b0, 0x1c59b21a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFloorSliderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateFloorSliderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Floor Slider");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Contact");
    proto->DeclareOutput("No Contact");

    proto->DeclareInParameter("Radius", CKPGUID_FLOAT, "1");

    proto->DeclareLocalParameter(NULL, CKPGUID_VECTOR);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FloorSlider);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

/***********************************************/
/*               Main Function                 */
/***********************************************/
int FloorSlider(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(1))
    { // enter by OFF
        beh->ActivateInput(1, FALSE);
        return CKBR_OK;
    }

    CK3dEntity *ball = (CK3dEntity *)beh->GetTarget();
    if (!ball)
    {
        if (beh->IsInputActive(0))
            beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(1);
        return CKBR_OWNERERROR;
    }

    ///
    // inputs

    VxVector pos = ball->GetWorldMatrix()[3];

    VxVector oldPos = pos;
    beh->GetLocalParameterValue(0, &oldPos);

    float radius = 1.0f;
    beh->GetInputParameterValue(0, &radius);

    CKBOOL touched = FALSE;

    // first determine the floor inside
    CKFloorManager *floorManager = (CKFloorManager *)behcontext.Context->GetManagerByGuid(FLOOR_MANAGER_GUID);

    VxVector outPos;
    touched = floorManager->ConstrainToFloor(oldPos, pos, radius, &outPos);
    if (touched)
    {
        ball->SetPosition(&outPos);
        beh->SetLocalParameterValue(0, &outPos);
    }
    else
    {
        beh->SetLocalParameterValue(0, &pos);
    }

    if (touched)
        beh->ActivateOutput(0);
    else
        beh->ActivateOutput(1);

    return CKBR_ACTIVATENEXTFRAME;
}
