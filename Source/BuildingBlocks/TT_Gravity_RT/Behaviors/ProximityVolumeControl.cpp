/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//            TT ProximityVolumeControl
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4f4f4c53)

CKObjectDeclaration *FillBehaviorProximityVolumeControlDecl();
CKERROR CreateProximityVolumeControlProto(CKBehaviorPrototype **pproto);
int ProximityVolumeControl(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorProximityVolumeControlDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ProximityVolumeControl");
    od->SetDescription("Sets the volume of a sound dependend to a distance.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x38571b61, 0x64cd2174));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateProximityVolumeControlProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateProximityVolumeControlProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ProximityVolumeControl");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("On");
    proto->DeclareOutput("Off");

    proto->DeclareInParameter("Object #1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Object #2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Near-Distance", CKPGUID_FLOAT, "0");
    proto->DeclareInParameter("Far-Distance", CKPGUID_FLOAT, "10");
    proto->DeclareInParameter("Sound", CKPGUID_WAVESOUND);

    proto->DeclareLocalParameter("Status", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ProximityVolumeControl);

    *pproto = proto;
    return CK_OK;
}

static float CalcGain(float distance)
{
    if (distance > 1.0f)
        return 1.0f;
    else if (distance >= 0.0f)
        return powf(50.0f, distance) * 0.02f;
    else
        return 0.0f;
}

int ProximityVolumeControl(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKBOOL status = FALSE;
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);
        beh->SetLocalParameterValue(0, &status);
    }
    else if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
        status = TRUE;
        beh->SetLocalParameterValue(0, &status);
    }
    else
    {
        beh->GetLocalParameterValue(0, &status);
    }

    CK3dEntity *object1 = (CK3dEntity *)beh->GetInputParameterObject(0);
    CK3dEntity *object2 = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!object1 || !object2)
    {
        return CKBR_PARAMETERERROR;
    }

    VxVector object2Pos;
    object2->GetPosition(&object2Pos, object1);
    float distance = object2Pos.SquareMagnitude();

    float nearDistance;
    float farDistance;
    beh->GetInputParameterValue(2, &nearDistance);
    beh->GetInputParameterValue(3, &farDistance);
    nearDistance = nearDistance * nearDistance;
    farDistance = farDistance * farDistance;

    float range = farDistance - nearDistance;
    if (range > 0.0f)
    {
        if (distance < farDistance)
            distance = (distance > nearDistance) ? 1.0f - (distance - nearDistance) / range : 1.0f;
        else
            distance = 0.0f;
    }
    else
    {
        distance = (distance <= nearDistance) ? 1.0f : 0.0f;
    }

    CKWaveSound *sound = (CKWaveSound *)beh->GetInputParameterObject(4);
    if (!sound)
        return CKBR_PARAMETERERROR;

    sound->SetGain(CalcGain(distance));

    if (status)
        return CKBR_ACTIVATENEXTFRAME;

    beh->ActivateOutput(1);
    return CKBR_OK;
}
